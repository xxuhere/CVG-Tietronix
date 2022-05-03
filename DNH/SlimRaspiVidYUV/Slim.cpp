#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <sysexits.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <iostream>

#include "bcm_host.h"
#include "interface/vcos/vcos.h"

#include "interface/mmal/mmal.h"
#include "interface/mmal/mmal_logging.h"
#include "interface/mmal/mmal_buffer.h"
#include "interface/mmal/util/mmal_util.h"
#include "interface/mmal/util/mmal_util_params.h"
#include "interface/mmal/util/mmal_default_components.h"
#include "interface/mmal/util/mmal_connection.h"

#define MMAL_CAMERA_PREVIEW_PORT 0
#define MMAL_CAMERA_VIDEO_PORT 1
#define MMAL_CAMERA_CAPTURE_PORT 2

#define VIDEO_FRAME_RATE_NUM 30
#define VIDEO_FRAME_RATE_DEN 1

#define VIDEO_OUTPUT_BUFFERS_NUM 3

const int ABORT_INTERVAL = 100; // ms

struct RASPICOMMONSETTINGS_PARAMETERS
{
   char camera_name[MMAL_PARAMETER_CAMERA_INFO_MAX_STR_LEN]; // Name of the camera sensor
   int width;                          /// Requested width of image
   int height;                         /// requested height of image
   int cameraNum;                      /// Camera number
   int sensor_mode;                    /// Sensor mode. 0=auto. Check docs/forum for modes selected by other values.
};

typedef struct raspicam_camera_parameters_s
{
   int sharpness;             /// -100 to 100
   int contrast;              /// -100 to 100
   int brightness;            ///  0 to 100
   int ISO;                   ///  TODO : what range?
   int videoStabilisation;    /// 0 or 1 (false or true)
   int exposureCompensation;  /// -10 to +10 ?
   MMAL_PARAM_EXPOSUREMODE_T exposureMode;
   MMAL_PARAM_EXPOSUREMETERINGMODE_T exposureMeterMode;
   MMAL_PARAM_FLICKERAVOID_T flickerAvoidMode;
   int shutter_speed;         /// 0 = auto, otherwise the shutter speed in ms
   float awb_gains_r;         /// AWB red gain
   float awb_gains_b;         /// AWB blue gain
   MMAL_PARAMETER_DRC_STRENGTH_T drc_level;  // Strength of Dynamic Range compression to apply

   float analog_gain;         // Analog gain
   float digital_gain;        // Digital gain

   int settings;
} RASPICAM_CAMERA_PARAMETERS;

struct RASPIVIDYUV_STATE_S;
typedef RASPIVIDYUV_STATE_S RASPIVIDYUV_STATE;

struct PORT_USERDATA
{
   FILE *file_handle;                   /// File handle to write buffer data to.
   RASPIVIDYUV_STATE *pstate;           /// pointer to our state in case required in callback
   int abort;                           /// Set to 1 in callback if an error occurs to attempt to abort the capture
};

/** Structure containing all state information for the current run
 */
struct RASPIVIDYUV_STATE_S
{
   RASPICOMMONSETTINGS_PARAMETERS common_settings;
   int framerate;                      				/// Requested frame rate (fps)
   RASPICAM_CAMERA_PARAMETERS camera_parameters; 	/// Camera setup parameters
   MMAL_COMPONENT_T *camera_component;    			/// Pointer to the camera component
   MMAL_POOL_T *camera_pool;            			/// Pointer to the pool of buffers used by camera video port
   PORT_USERDATA callback_data;         			/// Used to move data to the camera callback
};


inline void check_disable_port(MMAL_PORT_T* port)
{
   if (port && port->is_enabled)
      mmal_port_disable(port);
}

/*
 * If we are configured to use /dev/video0 as unicam (e.g. for libcamera) then
 * these legacy camera apps can't work. Fail immediately with an obvious message.
 */
void check_camera_stack()
{
	int fd = open("/dev/video0", O_RDWR, 0);
	if (fd < 0)
		return;

	struct v4l2_capability caps;
	int ret = ioctl(fd, VIDIOC_QUERYCAP, &caps);
	close(fd);

	if (ret < 0 || strcmp((char *)caps.driver, "unicam"))
		return;

	fprintf(stderr, "ERROR: the system should be configured for the legacy camera stack\n");
	exit(-1);
}

int raspicamcontrol_get_mem_gpu(void)
{
   char response[80] = "";
   int gpu_mem = 0;
   if (vc_gencmd(response, sizeof response, "get_mem gpu") == 0)
      vc_gencmd_number_property(response, "gpu", &gpu_mem);
   return gpu_mem;
}

/**
 * Ask GPU about its camera abilities
 * @param supported None-zero if software supports the camera
 * @param detected  None-zero if a camera has been detected
 */
static void raspicamcontrol_get_camera(int *supported, int *detected)
{
	char response[80] = "";
	if (vc_gencmd(response, sizeof response, "get_camera") == 0)
	{
		if (supported)
			vc_gencmd_number_property(response, "supported", supported);
		if (detected)
			vc_gencmd_number_property(response, "detected", detected);
	}
}

void raspicamcontrol_check_configuration(int min_gpu_mem)
{
	int gpu_mem = raspicamcontrol_get_mem_gpu();
	int supported = 0, detected = 0;
	raspicamcontrol_get_camera(&supported, &detected);
	if (!supported)
		vcos_log_error("Camera is not enabled in this build. Try running \"sudo raspi-config\" and ensure that \"camera\" has been enabled\n");
	else if (gpu_mem < min_gpu_mem)
		vcos_log_error("Only %dM of gpu_mem is configured. Try running \"sudo raspi-config\" and ensure that \"memory_split\" has a value of %d or greater\n", gpu_mem, min_gpu_mem);
	else if (!detected)
		vcos_log_error("Camera is not detected. Please check carefully the camera module is installed correctly\n");
	else
		vcos_log_error("Failed to run camera app. Please check for firmware updates\n");
}

void get_sensor_defaults(int camera_num, char *camera_name, int& width, int& height )
{
   // Default to the (Arducam 5MP Mini Camera) OV5647 setup
   strncpy(camera_name, "OV5647", MMAL_PARAMETER_CAMERA_INFO_MAX_STR_LEN);

   // Try to get the camera name and maximum supported resolution
   MMAL_COMPONENT_T *camera_info;
   MMAL_STATUS_T status = mmal_component_create(MMAL_COMPONENT_DEFAULT_CAMERA_INFO, &camera_info);
   if (status == MMAL_SUCCESS)
   {
      MMAL_PARAMETER_CAMERA_INFO_T param;
      param.hdr.id = MMAL_PARAMETER_CAMERA_INFO;
      param.hdr.size = sizeof(param)-4;  // Deliberately undersize to check firmware version
      status = mmal_port_parameter_get(camera_info->control, &param.hdr);

      if (status != MMAL_SUCCESS)
      {
         // Running on newer firmware
         param.hdr.size = sizeof(param);
         status = mmal_port_parameter_get(camera_info->control, &param.hdr);
         if (status == MMAL_SUCCESS && param.num_cameras > camera_num)
         {
            // Take the parameters from the first camera listed.
            if (width == 0)
               width = param.cameras[camera_num].max_width;
            if (height == 0)
               height = param.cameras[camera_num].max_height;
            strncpy(camera_name, param.cameras[camera_num].camera_name, MMAL_PARAMETER_CAMERA_INFO_MAX_STR_LEN);
            camera_name[MMAL_PARAMETER_CAMERA_INFO_MAX_STR_LEN-1] = 0;
         }
         else
            vcos_log_error("Cannot read camera info, keeping the defaults for OV5647");
      }
      else
      {
         // Older firmware
         // Nothing to do here, keep the defaults for OV5647
      }

      mmal_component_destroy(camera_info);
   }
   else
   {
      vcos_log_error("Failed to create camera_info component");
   }

   // default to OV5647 if nothing detected..
   if (width == 0)
      width = 2592;
   if (height == 0)
      height = 1944;
}

/**
 *  buffer header callback function for camera
 *
 *  Callback will dump buffer data to internal buffer
 *
 * @param port Pointer to port from which callback originated
 * @param buffer mmal buffer header pointer
 */
static void camera_buffer_callback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer)
{
	MMAL_BUFFER_HEADER_T *new_buffer;

	// We pass our file handle and other stuff in via the userdata field.

	PORT_USERDATA *pData = (PORT_USERDATA *)port->userdata;
	RASPIVIDYUV_STATE *pstate = pData->pstate;

	if (pData)
	{
		int bytes_written = 0;
		int bytes_to_write = buffer->length;

		bytes_to_write = 
			vcos_min(
				buffer->length, 
				port->format->es->video.width * port->format->es->video.height);

		vcos_assert(pData->file_handle);

		if (bytes_to_write)
		{
			mmal_buffer_header_mem_lock(buffer);
			bytes_written = fwrite(buffer->data, 1, bytes_to_write, pData->file_handle);
			mmal_buffer_header_mem_unlock(buffer);

			if (bytes_written != bytes_to_write)
			{
				vcos_log_error("Failed to write buffer data (%d from %d)- aborting", bytes_written, bytes_to_write);
				pData->abort = 1;
			}
		}
	}
	else
	{
		vcos_log_error("Received a camera buffer callback with no state");
	}

	// release buffer back to the pool
	mmal_buffer_header_release(buffer);

   // and send one back to the port (if still open)
   if (port->is_enabled)
   {
      
		new_buffer = mmal_queue_get(pData->pstate->camera_pool->queue);

		MMAL_STATUS_T status;
		if (new_buffer)
			status = mmal_port_send_buffer(port, new_buffer);

		if (!new_buffer || status != MMAL_SUCCESS)
			vcos_log_error("Unable to return a buffer to the camera port");
   }
}

// Log a status code as human readable text, and 
int mmal_status_to_int(MMAL_STATUS_T status)
{
	if (status == MMAL_SUCCESS)
		return 0;
	else
	{
		switch (status)
		{
		case MMAL_ENOMEM :
			vcos_log_error("Out of memory");
			break;
		case MMAL_ENOSPC :
			vcos_log_error("Out of resources (other than memory)");
			break;
		case MMAL_EINVAL:
			vcos_log_error("Argument is invalid");
			break;
		case MMAL_ENOSYS :
			vcos_log_error("Function not implemented");
			break;
		case MMAL_ENOENT :
			vcos_log_error("No such file or directory");
			break;
		case MMAL_ENXIO :
			vcos_log_error("No such device or address");
			break;
		case MMAL_EIO :
			vcos_log_error("I/O error");
			break;
		case MMAL_ESPIPE :
			vcos_log_error("Illegal seek");
			break;
		case MMAL_ECORRUPT :
			vcos_log_error("Data is corrupt \attention FIXME: not POSIX");
			break;
		case MMAL_ENOTREADY :
			vcos_log_error("Component is not ready \attention FIXME: not POSIX");
			break;
		case MMAL_ECONFIG :
			vcos_log_error("Component is not configured \attention FIXME: not POSIX");
			break;
		case MMAL_EISCONN :
			vcos_log_error("Port is already connected ");
			break;
		case MMAL_ENOTCONN :
			vcos_log_error("Port is disconnected");
			break;
		case MMAL_EAGAIN :
			vcos_log_error("Resource temporarily unavailable. Try again later");
			break;
		case MMAL_EFAULT :
			vcos_log_error("Bad address");
			break;
		default :
			vcos_log_error("Unknown status error");
			break;
		}

		return 1;
	}
}

void destroy_camera_component(RASPIVIDYUV_STATE* state)
{
	if (state->camera_component)
	{
		mmal_component_destroy(state->camera_component);
		state->camera_component = NULL;
	}
}

/**
 * Handler for sigint signals
 *
 * @param signal_number ID of incoming signal.
 *
 */
void default_signal_handler(int signal_number)
{
   if (signal_number == SIGUSR1)
   {
      // Handle but ignore - prevents us dropping out if started in none-signal mode
      // and someone sends us the USR1 signal anyway
   }
   else
   {
      // Going to abort on all other signals
      vcos_log_error("Aborting program\n");
      exit(130);
   }

}

/**
 * Set the specified camera to all the specified settings
 * @param camera Pointer to camera component
 * @param params Pointer to parameter block containing parameters
 * @return 0 if successful, none-zero if unsuccessful.
 */
int raspicamcontrol_set_all_parameters(MMAL_COMPONENT_T *camera, const RASPICAM_CAMERA_PARAMETERS *params)
{
   int result;

   if (params->settings)
   {
		MMAL_PARAMETER_CHANGE_EVENT_REQUEST_T change_event_request =
		{
			{MMAL_PARAMETER_CHANGE_EVENT_REQUEST, sizeof(MMAL_PARAMETER_CHANGE_EVENT_REQUEST_T)},
			MMAL_PARAMETER_CAMERA_SETTINGS, 1
		};

		MMAL_STATUS_T status = mmal_port_parameter_set(camera->control, &change_event_request.hdr);
		if ( status != MMAL_SUCCESS )
		{
			vcos_log_error("No camera settings events");
		}

		result += status;
   }

   return result;
}

/** Default camera callback function
 * Handles the --settings
 * @param port
 * @param Callback data
 */
void default_camera_control_callback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer)
{
	fprintf(stderr, "Camera control callback  cmd=0x%08x", buffer->cmd);

	if (buffer->cmd == MMAL_EVENT_PARAMETER_CHANGED)
	{
		MMAL_EVENT_PARAMETER_CHANGED_T *param = (MMAL_EVENT_PARAMETER_CHANGED_T *)buffer->data;
		switch (param->hdr.id)
		{
			case MMAL_PARAMETER_CAMERA_SETTINGS:
			{
				MMAL_PARAMETER_CAMERA_SETTINGS_T *settings = (MMAL_PARAMETER_CAMERA_SETTINGS_T*)param;
				vcos_log_error("Exposure now %u, analog gain %u/%u, digital gain %u/%u",
							settings->exposure,
							settings->analog_gain.num, settings->analog_gain.den,
							settings->digital_gain.num, settings->digital_gain.den);
				vcos_log_error("AWB R=%u/%u, B=%u/%u",
							settings->awb_red_gain.num, settings->awb_red_gain.den,
							settings->awb_blue_gain.num, settings->awb_blue_gain.den);
			}
			break;
		}
	}
	else if (buffer->cmd == MMAL_EVENT_ERROR)
	{
		vcos_log_error("No data received from sensor. Check all connections, including the Sunny one on the camera board");
	}
	else
	{
		vcos_log_error("Received unexpected camera control callback event, 0x%08x", buffer->cmd);
	}

	mmal_buffer_header_release(buffer);
}

MMAL_PORT_T *camera_video_port = NULL;
int exit_code = EX_OK;
int ShutdownGlobal(MMAL_STATUS_T status, RASPIVIDYUV_STATE& state)
{
	mmal_status_to_int(status);

	// Disable all our ports that are not handled by connections
	check_disable_port(camera_video_port);

	if (state.camera_component)
		mmal_component_disable(state.camera_component);

	// Can now close our file. Note disabling ports may flush buffers which causes
	// problems if we have already closed the file!
	if (state.callback_data.file_handle && state.callback_data.file_handle != stdout)
		fclose(state.callback_data.file_handle);

	//raspipreview_destroy(&state.preview_parameters);
	destroy_camera_component(&state);
	
	if (status != MMAL_SUCCESS)
		raspicamcontrol_check_configuration(128);
	
	return exit_code;
}

static MMAL_STATUS_T create_camera_component(RASPIVIDYUV_STATE *state)
{
	// Create the component
	MMAL_COMPONENT_T *camera = 0;
	MMAL_STATUS_T status = mmal_component_create(MMAL_COMPONENT_DEFAULT_CAMERA, &camera);

	if (status != MMAL_SUCCESS)
	{
		vcos_log_error("Failed to create camera component");
		return status;
	}

	if (status != MMAL_SUCCESS)
	{
		vcos_log_error("Could not set stereo mode : error %d", status);
		return status;
	}

	MMAL_PARAMETER_INT32_T camera_num =
	{
	   {MMAL_PARAMETER_CAMERA_NUM, sizeof(camera_num)}, 
	   state->common_settings.cameraNum
	};

	status = mmal_port_parameter_set(camera->control, &camera_num.hdr);

	if (status != MMAL_SUCCESS)
	{
		vcos_log_error("Could not select camera : error %d", status);
		return status;
	}

	if (!camera->output_num)
	{
		status = MMAL_ENOSYS;
		vcos_log_error("Camera doesn't have output ports");
		return status;
	}

	status = 
		mmal_port_parameter_set_uint32(
			camera->control, 
			MMAL_PARAMETER_CAMERA_CUSTOM_SENSOR_CONFIG, 
			state->common_settings.sensor_mode);

	if (status != MMAL_SUCCESS)
	{
		vcos_log_error("Could not set sensor mode : error %d", status);
		return status;
	}

	MMAL_PORT_T* preview_port 	= camera->output[MMAL_CAMERA_PREVIEW_PORT];
	MMAL_PORT_T* video_port 	= camera->output[MMAL_CAMERA_VIDEO_PORT];
	MMAL_PORT_T* still_port 	= camera->output[MMAL_CAMERA_CAPTURE_PORT];

	// Enable the camera, and tell it its control callback function
	status = mmal_port_enable(camera->control, default_camera_control_callback);

	if (status != MMAL_SUCCESS)
	{
		vcos_log_error("Unable to enable control port : error %d", status);
		return status;
	}

	//  set up the camera configuration
	{
		MMAL_PARAMETER_CAMERA_CONFIG_T cam_config =
		{
			{ MMAL_PARAMETER_CAMERA_CONFIG, sizeof(cam_config) },
			.max_stills_w 			= state->common_settings.width,
			.max_stills_h 			= state->common_settings.height,
			.stills_yuv422 			= 0,
			.one_shot_stills		= 0,
			.max_preview_video_w 	= state->common_settings.width,
			.max_preview_video_h 	= state->common_settings.height,
			.num_preview_video_frames = 3,
			.stills_capture_circular_buffer_height = 0,
			.fast_preview_resume 	= 0,
			.use_stc_timestamp 		= MMAL_PARAM_TIMESTAMP_MODE_RESET_STC
		};
		mmal_port_parameter_set(camera->control, &cam_config.hdr);
   }

   // Now set up the port formats

	//////////////////////////////////////////////////
	//
	//		PREVIEW PORT:	
	// 		Set the encode format on the Preview port
	// 		HW limitations mean we need the preview to be the same size as the required recorded output
	//
	//////////////////////////////////////////////////

	MMAL_ES_FORMAT_T* format = preview_port->format;

	if(state->camera_parameters.shutter_speed > 6000000)
	{
		MMAL_PARAMETER_FPS_RANGE_T fps_range = 
		{
			{MMAL_PARAMETER_FPS_RANGE, sizeof(fps_range)},
			{ 5, 	1000 }, 
			{ 166, 	1000 }
		};
		mmal_port_parameter_set(preview_port, &fps_range.hdr);
	}
	else if(state->camera_parameters.shutter_speed > 1000000)
	{
		MMAL_PARAMETER_FPS_RANGE_T fps_range = 
		{
			{MMAL_PARAMETER_FPS_RANGE, sizeof(fps_range)},
			{ 166, 1000 }, 
			{ 999, 1000 }
		};
		mmal_port_parameter_set(preview_port, &fps_range.hdr);
	}

	//enable dynamic framerate if necessary
	if (state->camera_parameters.shutter_speed)
	{
		if (state->framerate > 1000000./state->camera_parameters.shutter_speed)
		{
			state->framerate=0;
		}
	}

	format->encoding 					= MMAL_ENCODING_OPAQUE;
	format->es->video.width 			= VCOS_ALIGN_UP(state->common_settings.width, 32);
	format->es->video.height 			= VCOS_ALIGN_UP(state->common_settings.height, 16);
	format->es->video.crop.x 			= 0;
	format->es->video.crop.y 			= 0;
	format->es->video.crop.width 		= state->common_settings.width;
	format->es->video.crop.height 		= state->common_settings.height;
	format->es->video.frame_rate.num 	= state->framerate;
	format->es->video.frame_rate.den 	= VIDEO_FRAME_RATE_DEN;

	status = mmal_port_format_commit(preview_port);

	if (status != MMAL_SUCCESS)
	{
		vcos_log_error("camera viewfinder format couldn't be set");
		return status;
	}

	//////////////////////////////////////////////////
	//
	//		VIDEO PORT:
	// 		Set the encode format on the video  port
	//
	//////////////////////////////////////////////////
	
	format = video_port->format;

	if(state->camera_parameters.shutter_speed > 6000000) // TODO: DRY violation with logic?
	{
		MMAL_PARAMETER_FPS_RANGE_T fps_range = 
		{
			{MMAL_PARAMETER_FPS_RANGE, sizeof(fps_range)},
			{ 5, 	1000 }, 
			{ 166, 	1000 }
		};
		mmal_port_parameter_set(video_port, &fps_range.hdr);
	}
	else if(state->camera_parameters.shutter_speed > 1000000)
	{
		MMAL_PARAMETER_FPS_RANGE_T fps_range = 
		{
			{MMAL_PARAMETER_FPS_RANGE, sizeof(fps_range)},
			{ 167, 	1000 }, 
			{ 999, 	1000 }
		};
		mmal_port_parameter_set(video_port, &fps_range.hdr);
	}

	format->encoding = MMAL_ENCODING_I420;
	format->encoding_variant = MMAL_ENCODING_I420;

	format->es->video.width 			= VCOS_ALIGN_UP(state->common_settings.width, 32);
	format->es->video.height 			= VCOS_ALIGN_UP(state->common_settings.height, 16);
	format->es->video.crop.x 			= 0;
	format->es->video.crop.y 			= 0;
	format->es->video.crop.width 		= state->common_settings.width;
	format->es->video.crop.height 		= state->common_settings.height;
	format->es->video.frame_rate.num 	= state->framerate;
	format->es->video.frame_rate.den 	= VIDEO_FRAME_RATE_DEN;

	status = mmal_port_format_commit(video_port);

	if (status != MMAL_SUCCESS)
	{
		vcos_log_error("camera video format couldn't be set");
		return status;
	}

	// Ensure there are enough buffers to avoid dropping frames
	if (video_port->buffer_num < VIDEO_OUTPUT_BUFFERS_NUM)
		video_port->buffer_num = VIDEO_OUTPUT_BUFFERS_NUM;

	status = mmal_port_parameter_set_boolean(video_port, MMAL_PARAMETER_ZERO_COPY, MMAL_TRUE);
	if (status != MMAL_SUCCESS)
	{
		vcos_log_error("Failed to select zero copy");
		return status;
	}
	//////////////////////////////////////////////////
	//
	//	STILL PORT:
	// 	Set the encode format on the still  port
	//
	//////////////////////////////////////////////////

	format = still_port->format;

	format->encoding = MMAL_ENCODING_OPAQUE;
	format->encoding_variant = MMAL_ENCODING_I420;

	format->es->video.width 		= VCOS_ALIGN_UP(state->common_settings.width, 32);
	format->es->video.height 		= VCOS_ALIGN_UP(state->common_settings.height, 16);
	format->es->video.crop.x 		= 0;
	format->es->video.crop.y 		= 0;
	format->es->video.crop.width 	= state->common_settings.width;
	format->es->video.crop.height 	= state->common_settings.height;
	format->es->video.frame_rate.num = 0;
	format->es->video.frame_rate.den = 1;

	status = mmal_port_format_commit(still_port);

	if (status != MMAL_SUCCESS)
	{
		vcos_log_error("camera still format couldn't be set");
		return status;
	}

	/* Ensure there are enough buffers to avoid dropping frames */
	if (still_port->buffer_num < VIDEO_OUTPUT_BUFFERS_NUM)
		still_port->buffer_num = VIDEO_OUTPUT_BUFFERS_NUM;

	//////////////////////////////////////////////////
	//
	// 	ENABLE COMPONENT
	//
	//////////////////////////////////////////////////
	status = mmal_component_enable(camera);

	if (status != MMAL_SUCCESS)
	{
		vcos_log_error("camera component couldn't be enabled");
		return status;
	}

	raspicamcontrol_set_all_parameters(camera, &state->camera_parameters);

	// Create pool of buffer headers for the output port to consume
	MMAL_POOL_T* pool = 
		mmal_port_pool_create(
			video_port, 
			video_port->buffer_num, 
			video_port->buffer_size);
	
	if (!pool)
	{
		vcos_log_error("Failed to create buffer header pool for camera still port %s", still_port->name);
		return status;
	}

	state->camera_pool = pool;
	state->camera_component = camera;

	return status;
}

int main(int argc, const char **argv)
{
	// Our main data storage vessel..
	check_camera_stack();
	
	// The Raspberry Pi requires that the bcm_host_init() function is called first before any GPU calls can be made.
	// https://elinux.org/Raspberry_Pi_VideoCore_APIs
	bcm_host_init();

	// Register our application with the logging system
	vcos_log_register("RaspiVid", VCOS_LOG_CATEGORY);

	signal(SIGINT, default_signal_handler);
	signal(SIGUSR1, SIG_IGN);		// Disable USR1 for the moment - may be reenabled if go in to signal capture mode

	RASPIVIDYUV_STATE state;
	memset(&state, 0, sizeof(RASPIVIDYUV_STATE));
	
	//raspicommonsettings_set_defaults(&state->common_settings);
	//void raspicommonsettings_set_defaults(RASPICOMMONSETTINGS_PARAMETERS *state)
	strncpy(state.common_settings.camera_name, "(Unknown)", MMAL_PARAMETER_CAMERA_INFO_MAX_STR_LEN);
	state.common_settings.cameraNum 	= 0;
	state.common_settings.sensor_mode 	= 0;
	state.common_settings.width 		= 1920;       // Default to 1080p
	state.common_settings.height 		= 1080;
	state.framerate 					= VIDEO_FRAME_RATE_NUM;
	
	//void raspicamcontrol_set_defaults(RASPICAM_CAMERA_PARAMETERS *params)
	RASPICAM_CAMERA_PARAMETERS* params = &state.camera_parameters;
	params->sharpness 					= 0;
	params->contrast 					= 0;
	params->brightness 					= 50;
	params->ISO 						= 0;   	// 0 = auto
	params->videoStabilisation 			= 0;
	params->exposureCompensation 		= 0;
	params->exposureMode 				= MMAL_PARAM_EXPOSUREMODE_AUTO;
	params->flickerAvoidMode 			= MMAL_PARAM_FLICKERAVOID_OFF;
	params->exposureMeterMode 			= MMAL_PARAM_EXPOSUREMETERINGMODE_AVERAGE;
	params->shutter_speed 				= 0;    // 0 = auto
	params->awb_gains_r 				= 0;	// Only have any function if AWB OFF is used.
	params->awb_gains_b 				= 0;
	params->drc_level 					= MMAL_PARAMETER_DRC_STRENGTH_OFF;
	

	// Setup for sensor specific parameters, only set W/H settings if zero on entry
	get_sensor_defaults(
		state.common_settings.cameraNum, 
		state.common_settings.camera_name,
		state.common_settings.width, 
		state.common_settings.height);
					   
	// OK, we have a nice set of parameters. Now set up our components
	// We have two components. Camera, Preview
	MMAL_STATUS_T status = create_camera_component(&state);
	if(status != MMAL_SUCCESS)
	{
		vcos_log_error("%s: Failed to create camera component", __func__);
		exit_code = EX_SOFTWARE;
	}
	else
	{
		camera_video_port   = state.camera_component->output[MMAL_CAMERA_VIDEO_PORT];
		if (status == MMAL_SUCCESS)
		{
			state.callback_data.file_handle = stdout;

			// Set up our userdata - this is passed though to the callback where we need the information.
			state.callback_data.pstate = &state;
			state.callback_data.abort = 0;
			camera_video_port->userdata = (struct MMAL_PORT_USERDATA_T *)&state.callback_data;

			// Only save stuff if we have a filename and it opened
			// Note we use the file handle copy in the callback, as the call back MIGHT change the file handle
			if (state.callback_data.file_handle)
			{
				// Enable the camera video port and tell it its callback function
				status = mmal_port_enable(camera_video_port, camera_buffer_callback);

				if (status != MMAL_SUCCESS)
				{
					vcos_log_error("Failed to setup camera output");
					return ShutdownGlobal(status, state);
				}

				// Send all the buffers to the camera video port
				int num = mmal_queue_length(state.camera_pool->queue);
				for (int q = 0; q < num; q++)
				{
					MMAL_BUFFER_HEADER_T *buffer = mmal_queue_get(state.camera_pool->queue);

					if (!buffer)
						vcos_log_error("Unable to get a required buffer %d from pool queue", q);

					if (mmal_port_send_buffer(camera_video_port, buffer)!= MMAL_SUCCESS)
						vcos_log_error("Unable to send a buffer to camera video port (%d)", q);
				}		
				bool running = true;
				while (running)
				{
					// Change state
					if (mmal_port_parameter_set_boolean(camera_video_port, MMAL_PARAMETER_CAPTURE, true) != MMAL_SUCCESS)
					{ 
						// How to handle? 
					}
					// We never return from this. Expect a ctrl-c to exit or abort.
					while (!state.callback_data.abort)
					{
						// Have a sleep so we don't hog the CPU.
						vcos_sleep(ABORT_INTERVAL);
					}
					running = false;
				}
			}
		}
		else
		{
			mmal_status_to_int(status);
			vcos_log_error("%s: Failed to connect camera to preview", __func__);
		}
	}
	return ShutdownGlobal(status, state);
}