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

// The installation of these header files on the RaspberryPi may involve 
// checking out the userland repo, and building the repo in order
// to install linkable libraries and to copy the .h files into the
// expected locations.
// https://github.com/raspberrypi/userland
#include "bcm_host.h"
#include "interface/vcos/vcos.h"
#include "interface/mmal/mmal.h"
#include "interface/mmal/mmal_logging.h"
#include "interface/mmal/mmal_buffer.h"
#include "interface/mmal/util/mmal_util.h"
#include "interface/mmal/util/mmal_util_params.h"
#include "interface/mmal/util/mmal_default_components.h"
#include "interface/mmal/util/mmal_connection.h"

#include <opencv2/imgcodecs.hpp>
#include "CamImpl_MMAL.h"

/*
#define MMAL_VERBOSE true

#if MMAL_VERBOSE
	#define VERBOSELOG(msg) \
		std::cout << "VLOG: " << (msg) << std::endl;
#else
	#define VERBOSELOG(msg)
#endif
*/

// This code is based off the RaspiVidYUV application, originally
// programmed in C, using a different coding convention. While some
// of it is left alone, the code is also picked-at and converted 
// as needed if it needs to be touched during the conversion to
// CamImpl_MMAL.
//
// https://github.com/raspberrypi/userland/blob/master/host_applications/linux/apps/raspicam/RaspiVidYUV.c
//
// For documentation on MMAL, see
//	http://www.jvcref.com/files/PI/documentation/html/index.html and
//  https://picamera.readthedocs.io/en/release-1.13/api_mmalobj.html
//
// The second link is specifically for the Python library but has
// very comprehensive and transferable information.

int CamImpl_MMAL::instsInit = 0;

/// <summary>
/// The various camera ports. We're only interested in Video, but may
/// need to "entertain" properly setting up the preview and still capture
/// ports - something that needs further follow-up to understand
/// if necessary.
/// </summary>
enum MMALCamPort
{
	Preview = 0,
	Video,
	Capture		// Still snapshot port
};

const int minVideoOutputBuffers = 3;


#define TARGET_CAMRES_WIDTH 1920		// Default capture width
#define TARGET_CAMRES_HEIGHT 1080		// Default capture height
//
#define VIDEO_FRAME_RATE_NUM 30
#define VIDEO_FRAME_RATE_DEN 1

/// <summary>
/// Coppied from the RaspiVidYUV program. Left in, in case we want
/// to reuse and recycle some of the brightness adjustment logic.
/// </summary>
class RaspicamParams
{
public:
	// Right now these are set in SetDefaults(), but we may just
	// want to make members be member-initialized and get rid of
	// SetDefaults().
	//
	int sharpness;						/// -100 to 100
	int contrast;						/// -100 to 100
	int brightness;						///  0 to 100
	int ISO;							///  TODO : what range?
	int videoStabilisation;				/// 0 or 1 (false or true)
	int exposureCompensation;			/// -10 to +10 ?
	MMAL_PARAM_EXPOSUREMODE_T 			exposureMode;
	MMAL_PARAM_EXPOSUREMETERINGMODE_T 	exposureMeterMode;
	MMAL_PARAM_FLICKERAVOID_T 			flickerAvoidMode;
	int shutter_speed;					/// 0 = auto, otherwise the shutter speed in ms
	float awb_gains_r;					/// AWB red gain
	float awb_gains_b;					/// AWB blue gain
	MMAL_PARAMETER_DRC_STRENGTH_T drc_level;  // Strength of Dynamic Range compression to apply
	
	float analog_gain;					// Analog gain
	float digital_gain;					// Digital gain
	
	int settings;
   
public:
	void SetDefaults()
	{
		this->sharpness 				= 0;
		this->contrast 					= 0;
		this->brightness 				= 50;
		this->ISO 						= 0;   	// 0 = auto
		this->videoStabilisation 		= 0;
		this->exposureCompensation 		= 0;
		this->exposureMode 				= MMAL_PARAM_EXPOSUREMODE_AUTO;
		this->flickerAvoidMode 			= MMAL_PARAM_FLICKERAVOID_OFF;
		this->exposureMeterMode 		= MMAL_PARAM_EXPOSUREMETERINGMODE_AVERAGE;
		this->shutter_speed 			= 0;    // 0 = auto
		this->awb_gains_r 				= 0;	// Only have any function if AWB OFF is used.
		this->awb_gains_b 				= 0;
		this->drc_level 				= MMAL_PARAMETER_DRC_STRENGTH_OFF;
	}
};

/// <summary>
/// Transfered from RaspiVidYUV with some member containers spilled out
/// and the naming convention changed a bit for C++ and code convention
/// confort.
/// </summary>
class RPiYUVState
{
public:

	// Name of the camera sensor
	char camera_name[MMAL_PARAMETER_CAMERA_INFO_MAX_STR_LEN]; 

	/// Requested width of image
	int width;

	/// requested height of image
	int height;

	/// Camera number
	int cameraNum;

	/// Sensor mode. 0=auto. Check docs/forum for modes selected by other values.
	int sensor_mode;

   /// Requested frame rate (fps)
   int framerate;

   /// Camera setup parameters
   RaspicamParams camParams;

   /// Pointer to the camera component
   MMAL_COMPONENT_T* camera_component;

   /// Pointer to the pool of buffers used by camera video port
   MMAL_POOL_T* camera_pool;

public:
   void SetDefaults()
   {
	   memset(this, 0, sizeof(RPiYUVState));

	   strncpy(this->camera_name, "(Unknown)", MMAL_PARAMETER_CAMERA_INFO_MAX_STR_LEN);
	   this->cameraNum 		= 0;
	   this->sensor_mode	= 0;
	   this->width 			= TARGET_CAMRES_WIDTH;		// Default to 1080p
	   this->height 		= TARGET_CAMRES_HEIGHT;
	   this->framerate 		= VIDEO_FRAME_RATE_NUM;

	   this->camParams.SetDefaults();
   }

   void DisableCameraComponent()
   {
	   if (this->camera_component)
		   mmal_component_disable(this->camera_component);
   }

   void DestroyCameraComponent()
   {
	   if (this->camera_component)
	   {
		   mmal_component_destroy(this->camera_component);
		   this->camera_component = nullptr;
	   }
   }
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
bool check_camera_stack(const std::string& devPath)
{
	int fd = open(devPath.c_str(), O_RDWR, 0);
	if (fd < 0)
		return false;

	struct v4l2_capability caps;
	int ret = ioctl(fd, VIDIOC_QUERYCAP, &caps);
	close(fd);

	if (ret < 0 || strcmp((char *)caps.driver, "unicam"))
		return true;

	std::cerr << "ERROR: the system should be configured for the legacy camera stack" << std::endl;
	return false;
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
	int supported = 0;
	int detected = 0;
	raspicamcontrol_get_camera(&supported, &detected);

	if (!supported)
		std::cerr << "Camera is not enabled in this build. Try running \"sudo raspi-config\" and ensure that \"camera\" has been enabled" << std::endl;
	else if (gpu_mem < min_gpu_mem)
		std::cerr << "Only " << gpu_mem << " of gpu_mem is configured. Try running \"sudo raspi-config\" and ensure that \"memory_split\" has a value of " << min_gpu_mem << " or greater" << std::endl;
	else if (!detected)
		std::cerr << "Camera is not detected. Please check carefully the camera module is installed correctly" << std::endl;
	else
		std::cerr << "Failed to run camera app. Please check for firmware updates" << std::endl;
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
			std::cerr << "Cannot read camera info, keeping the defaults for OV5647" << std::endl;
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
	   std::cerr << "Failed to create camera_info component" << std::endl;
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
void CamImpl_MMAL::_CameraBufferCallback(MMAL_PORT_T* port, MMAL_BUFFER_HEADER_T* buffer)
{
	MMAL_BUFFER_HEADER_T *new_buffer;

	// We pass our file handle and other stuff in via the userdata field.

	CamImpl_MMAL* camImpl = (CamImpl_MMAL*)port->userdata;

	if (camImpl)
	{
		int bytes_to_write = 
			vcos_min(
				buffer->length, 
				port->format->es->video.width * port->format->es->video.height);

		if (bytes_to_write)
		{
			mmal_buffer_header_mem_lock(buffer);
			{
				cv::Ptr<cv::Mat> matImg = 
					new cv::Mat(
						port->format->es->video.height,
						port->format->es->video.width,
						CV_8UC1);

				// The data in buffer->data is locked hardware memory so for the sake
				// of sanity we're not going to hang onto it. This means we're going
				// to make a copy of it.
				memcpy(buffer->data, &matImg->data[0], bytes_to_write);
			}
			mmal_buffer_header_mem_unlock(buffer);
		}
	}
	else
	{
		std::cerr << "Received a camera buffer callback with no state" << std::endl;
	}

	// release buffer back to the pool
	mmal_buffer_header_release(buffer);

   // and send one back to the port (if still open)
   if (port->is_enabled)
   {
		new_buffer = mmal_queue_get(camImpl->state->camera_pool->queue);

		MMAL_STATUS_T status;
		if (new_buffer)
			status = mmal_port_send_buffer(port, new_buffer);

		if (!new_buffer || status != MMAL_SUCCESS)
			std::cerr << "Unable to return a buffer to the camera port" << std::endl;
   }
}

// Log a status code as human readable text, and 
bool HandleMMALStatus(MMAL_STATUS_T status)
{
	if (status == MMAL_SUCCESS)
		return false;
	
	switch (status)
	{
	case MMAL_ENOMEM :
		std::cerr << "Out of memory" << std::endl;
		break;
	case MMAL_ENOSPC :
		std::cerr << "Out of resources (other than memory)" << std::endl;
		break;
	case MMAL_EINVAL:
		std::cerr << "Argument is invalid" << std::endl;
		break;
	case MMAL_ENOSYS :
		std::cerr << "Function not implemented" << std::endl;
		break;
	case MMAL_ENOENT :
		std::cerr << "No such file or directory" << std::endl;
		break;
	case MMAL_ENXIO :
		std::cerr << "No such device or address" << std::endl;
		break;
	case MMAL_EIO :
		std::cerr << "I/O error" << std::endl;
		break;
	case MMAL_ESPIPE :
		std::cerr << "Illegal seek" << std::endl;
		break;
	case MMAL_ECORRUPT :
		std::cerr << "Data is corrupt \attention FIXME: not POSIX" << std::endl;
		break;
	case MMAL_ENOTREADY :
		std::cerr << "Component is not ready \attention FIXME: not POSIX" << std::endl;
		break;
	case MMAL_ECONFIG :
		std::cerr << "Component is not configured \attention FIXME: not POSIX" << std::endl;
		break;
	case MMAL_EISCONN :
		std::cerr << "Port is already connected " << std::endl;
		break;
	case MMAL_ENOTCONN :
		std::cerr << "Port is disconnected" << std::endl;
		break;
	case MMAL_EAGAIN :
		std::cerr << "Resource temporarily unavailable. Try again later" << std::endl;
		break;
	case MMAL_EFAULT :
		std::cerr << "Bad address" << std::endl;
		break;
	default :
		std::cerr << "Unknown status error" << std::endl;
		break;
	}

	return true;
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
	  std::cerr << "Aborting program" << std::endl;
	  exit(130);
   }

}

/**
 * Set the specified camera to all the specified settings
 * @param camera Pointer to camera component
 * @param params Pointer to parameter block containing parameters
 * @return 0 if successful, none-zero if unsuccessful.
 */
int raspicamcontrol_set_all_parameters(MMAL_COMPONENT_T *camera, const RaspicamParams* params)
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
			std::cerr << "No camera settings events" << std::endl;

		result += status;
   }

   return result;
}

/** Default camera callback function
 * Handles the --settings
 * @param port
 * @param Callback data
 */
void CamImpl_MMAL::_CameraControlCallback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer)
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

				std::cerr << "Exposure now "	<< settings->exposure << 
							 ", analog gain "	<< settings->analog_gain.num	<< settings->analog_gain.den << 
							 ", digital gain "	<< settings->digital_gain.num	<< settings->digital_gain.den << std::endl;

				std::cerr << "AWB R=" << settings->awb_red_gain.num  << settings->awb_red_gain.den << 
							 ", B= "  << settings->awb_blue_gain.num << settings->awb_blue_gain.den << std::endl;
			}
			break;
		}
	}
	else if (buffer->cmd == MMAL_EVENT_ERROR)
	{
		std::cerr << "No data received from sensor. Check all connections, including the Sunny one on the camera board" << std::endl;
	}
	else
	{
		std::cerr << "Received unexpected camera control callback event, " << buffer->cmd << std::endl;
	}

	mmal_buffer_header_release(buffer);
}

bool CamImpl_MMAL::_ShutdownGlobal()
{
	return this->_ShutdownGlobal(MMAL_SUCCESS);
}

bool CamImpl_MMAL::_ShutdownGlobal(MMAL_STATUS_T status)
{
	HandleMMALStatus(status);

	// Disable all our ports that are not handled by connections
	check_disable_port(this->camVideoPort);

	state->DisableCameraComponent();

	//raspipreview_destroy(&state.preview_parameters);
	state->DestroyCameraComponent();

	if (status != MMAL_SUCCESS)
		raspicamcontrol_check_configuration(128);

	return true;
}

static MMAL_STATUS_T create_camera_component(RPiYUVState* state)
{
	// Create the component
	MMAL_COMPONENT_T* camera = nullptr;
	MMAL_STATUS_T status = mmal_component_create(MMAL_COMPONENT_DEFAULT_CAMERA, &camera);

	if (status != MMAL_SUCCESS)
	{
		std::cerr << "Failed to create camera component" << std::endl;
		return status;
	}

	if (status != MMAL_SUCCESS)
	{
		std::cerr << "Could not set stereo mode : error " << status << std::endl;
		return status;
	}

	MMAL_PARAMETER_INT32_T camera_num =
	{
	   {MMAL_PARAMETER_CAMERA_NUM, sizeof(camera_num)}, 
	   state->cameraNum
	};

	status = mmal_port_parameter_set(camera->control, &camera_num.hdr);

	if (status != MMAL_SUCCESS)
	{
		std::cerr << "Could not select camera : error " << status << std::endl;
		return status;
	}

	if (!camera->output_num)
	{
		std::cerr << "Camera doesn't have output ports" << status << std::endl;
		return MMAL_ENOSYS;
	}

	status = 
		mmal_port_parameter_set_uint32(
			camera->control, 
			MMAL_PARAMETER_CAMERA_CUSTOM_SENSOR_CONFIG, 
			state->sensor_mode);

	if (status != MMAL_SUCCESS)
	{
		std::cerr << "Could not set sensor mode : error " << status << std::endl;
		return status;
	}

	// TODO: Currently not sure if I can just initialize the video_port,
	// or if ALL 3 ports need to be set up correctly for the PiCam to
	// not blow up.
	MMAL_PORT_T* preview_port 	= camera->output[MMALCamPort::Preview];
	MMAL_PORT_T* video_port 	= camera->output[MMALCamPort::Video];
	MMAL_PORT_T* still_port 	= camera->output[MMALCamPort::Capture];

	// Enable the camera, and tell it its control callback function
	status = mmal_port_enable(camera->control, CamImpl_MMAL::_CameraControlCallback);

	if (status != MMAL_SUCCESS)
	{
		std::cerr << "Unable to enable control port : error " << status << std::endl;
		return status;
	}

	//  set up the camera configuration
	{
		MMAL_PARAMETER_CAMERA_CONFIG_T cam_config =
		{
			{ MMAL_PARAMETER_CAMERA_CONFIG, sizeof(cam_config) },
			.max_stills_w 			= state->width,
			.max_stills_h 			= state->height,
			.stills_yuv422 			= 0,
			.one_shot_stills		= 0,
			.max_preview_video_w 	= state->width,
			.max_preview_video_h 	= state->height,
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

	if(state->camParams.shutter_speed > 6000000)
	{
		MMAL_PARAMETER_FPS_RANGE_T fps_range = 
		{
			{MMAL_PARAMETER_FPS_RANGE, sizeof(fps_range)},
			{ 5, 	1000 }, 
			{ 166, 	1000 }
		};
		mmal_port_parameter_set(preview_port, &fps_range.hdr);
	}
	else if(state->camParams.shutter_speed > 1000000)
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
	if (state->camParams.shutter_speed)
	{
		if (state->framerate > 1000000./state->camParams.shutter_speed)
		{
			state->framerate=0;
		}
	}

	format->encoding 					= MMAL_ENCODING_OPAQUE;
	format->es->video.width 			= VCOS_ALIGN_UP(state->width, 32);
	format->es->video.height 			= VCOS_ALIGN_UP(state->height, 16);
	format->es->video.crop.x 			= 0;
	format->es->video.crop.y 			= 0;
	format->es->video.crop.width 		= state->width;
	format->es->video.crop.height 		= state->height;
	format->es->video.frame_rate.num 	= state->framerate;
	format->es->video.frame_rate.den 	= VIDEO_FRAME_RATE_DEN;

	status = mmal_port_format_commit(preview_port);

	if (status != MMAL_SUCCESS)
	{
		std::cerr << "camera viewfinder format couldn't be set" << std::endl;
		return status;
	}

	//////////////////////////////////////////////////
	//
	//		VIDEO PORT:
	// 		Set the encode format on the video  port
	//
	//////////////////////////////////////////////////
	
	format = video_port->format;

	if(state->camParams.shutter_speed > 6000000) // TODO: DRY violation with logic?
	{
		MMAL_PARAMETER_FPS_RANGE_T fps_range = 
		{
			{MMAL_PARAMETER_FPS_RANGE, sizeof(fps_range)},
			{ 5, 	1000 }, 
			{ 166, 	1000 }
		};
		mmal_port_parameter_set(video_port, &fps_range.hdr);
	}
	else if(state->camParams.shutter_speed > 1000000)
	{
		MMAL_PARAMETER_FPS_RANGE_T fps_range = 
		{
			{MMAL_PARAMETER_FPS_RANGE, sizeof(fps_range)},
			{ 167, 	1000 }, 
			{ 999, 	1000 }
		};
		mmal_port_parameter_set(video_port, &fps_range.hdr);
	}

	format->encoding					= MMAL_ENCODING_I420;
	format->encoding_variant			= MMAL_ENCODING_I420;

	format->es->video.width 			= VCOS_ALIGN_UP(state->width, 32);
	format->es->video.height 			= VCOS_ALIGN_UP(state->height, 16);
	format->es->video.crop.x 			= 0;
	format->es->video.crop.y 			= 0;
	format->es->video.crop.width 		= state->width;
	format->es->video.crop.height 		= state->height;
	format->es->video.frame_rate.num 	= state->framerate;
	format->es->video.frame_rate.den 	= VIDEO_FRAME_RATE_DEN;

	status = mmal_port_format_commit(video_port);

	if (status != MMAL_SUCCESS)
	{
		std::cerr << "camera video format couldn't be set" << std::endl;
		return status;
	}

	// Ensure there are enough buffers to avoid dropping frames
	if (video_port->buffer_num < minVideoOutputBuffers)
		video_port->buffer_num = minVideoOutputBuffers;

	status = mmal_port_parameter_set_boolean(video_port, MMAL_PARAMETER_ZERO_COPY, MMAL_TRUE);
	if (status != MMAL_SUCCESS)
	{
		std::cerr << "Failed to select zero copy" << std::endl;
		return status;
	}
	//////////////////////////////////////////////////
	//
	//	STILL PORT:
	// 	Set the encode format on the still  port
	//
	//////////////////////////////////////////////////

	format = still_port->format;

	format->encoding					= MMAL_ENCODING_OPAQUE;
	format->encoding_variant			= MMAL_ENCODING_I420;

	format->es->video.width 			= VCOS_ALIGN_UP(state->width, 32);
	format->es->video.height 			= VCOS_ALIGN_UP(state->height, 16);
	format->es->video.crop.x 			= 0;
	format->es->video.crop.y 			= 0;
	format->es->video.crop.width 		= state->width;
	format->es->video.crop.height 		= state->height;
	format->es->video.frame_rate.num	= 0;
	format->es->video.frame_rate.den	= 1;

	status = mmal_port_format_commit(still_port);

	if (status != MMAL_SUCCESS)
	{
		std::cerr << "camera still format couldn't be set" << std::endl;
		return status;
	}

	/* Ensure there are enough buffers to avoid dropping frames */
	if (still_port->buffer_num < minVideoOutputBuffers)
		still_port->buffer_num = minVideoOutputBuffers;

	//////////////////////////////////////////////////
	//
	// 	ENABLE COMPONENT
	//
	//////////////////////////////////////////////////
	status = mmal_component_enable(camera);

	if (status != MMAL_SUCCESS)
	{
		std::cerr << "camera component couldn't be enabled" << std::endl;
		return status;
	}

	raspicamcontrol_set_all_parameters(camera, &state->camParams);

	// Create pool of buffer headers for the output port to consume
	MMAL_POOL_T* pool = 
		mmal_port_pool_create(
			video_port, 
			video_port->buffer_num, 
			video_port->buffer_size);
	
	if (!pool)
	{
		std::cerr << "Failed to create buffer header pool for camera still port " << still_port->name << std::endl;
		return status;
	}

	state->camera_pool		= pool;
	state->camera_component = camera;

	return status;
}

void CamImpl_MMAL::InitPrereqs()
{
	++instsInit;
	if(instsInit != 1)
		return;
	
	// The Raspberry Pi requires that the bcm_host_init() function is called first before any GPU calls can be made.
	// https://elinux.org/Raspberry_Pi_VideoCore_APIs
	bcm_host_init();

	// Register our application with the logging system
	vcos_log_register("HMDOpView", VCOS_LOG_CATEGORY);

	signal(SIGINT, default_signal_handler);
	signal(SIGUSR1, SIG_IGN);		// Disable USR1 for the moment - may be reenabled if go in to signal capture mode
}

bool CamImpl_MMAL::InitializeImpl()
{
	// While we could call InitPrereqs() here, we'll reserve calling that
	// for when we're absolutely sure it's being used, in ActivateImpl().
	return true;
}

bool CamImpl_MMAL::ShutdownImpl()
{
	--instsInit;

	if (this->state)
	{
		this->state->DestroyCameraComponent();
		this->_ShutdownGlobal();

		delete this->state;
		this->state = nullptr;
	}
	return true;
}

bool CamImpl_MMAL::ActivateImpl()
{
	// Our main data storage vessel..
	check_camera_stack(this->devPath);

	InitPrereqs();
	this->state->SetDefaults();

	// Setup for sensor specific parameters, only set W/H settings if zero on entry
	get_sensor_defaults(
		this->state->cameraNum, 
		this->state->camera_name,
		this->state->width, 
		this->state->height);
					   
	MMAL_STATUS_T status = create_camera_component(state);
	if(status != MMAL_SUCCESS)
	{
		std::cerr << __func__ << ": Failed to create camera component" << std::endl;
		return false;
	}

	this->camVideoPort = this->state->camera_component->output[MMALCamPort::Video];

	if (status != MMAL_SUCCESS)
	{
		HandleMMALStatus(status);
		std::cerr << __func__ << ": Failed to connect camera to preview" << std::endl;
		return false;
	}

	// Set up our userdata - this is passed though to the callback where we need the information.
	this->polling = true;
	this->camVideoPort->userdata = (struct MMAL_PORT_USERDATA_T *)this;
	
	// Enable the camera video port and tell it its callback function
	status = mmal_port_enable(this->camVideoPort, CamImpl_MMAL::_CameraBufferCallback);

	if (status != MMAL_SUCCESS)
	{
		std::cerr << "Failed to setup camera output" << std::endl;
		return this->_ShutdownGlobal(status);
	}

	// Send all the buffers to the camera video port
	int num = mmal_queue_length(this->state->camera_pool->queue);
	for (int q = 0; q < num; q++)
	{
		MMAL_BUFFER_HEADER_T *buffer = mmal_queue_get(this->state->camera_pool->queue);

		if (!buffer)
			std::cerr << "Unable to get a required buffer " << q << " from pool queue" << std::endl;

		if (mmal_port_send_buffer(this->camVideoPort, buffer)!= MMAL_SUCCESS)
			std::cerr << "Unable to send a buffer to camera video port (" << q << ")" << std::endl;
	}	
}

bool CamImpl_MMAL::DeactivateImpl()
{
	this->polling = false;

	if(this->state != nullptr)
	{
		delete this->state;
		this->state = nullptr;
	}
}

cv::Ptr<cv::Mat> CamImpl_MMAL::PollFrameImpl()
{
	cv::Ptr<cv::Mat> ret;
	// Safely get the last polled frame, without risk of
	// mucking up the MMAL PiCam callback.
	std::lock_guard<std::mutex> guard(this->mutPolled);
	{
		ret = this->lastPolled;
		this->lastPolled = cv::Ptr<cv::Mat>();
	}
	return ret;
}

CamImpl_MMAL::CamImpl_MMAL(const std::string& devPath)
{
	this->devPath = devPath;
}
	
VideoPollType CamImpl_MMAL::PollType()
{
	return VideoPollType::MMAL;
}

bool CamImpl_MMAL::IsValid()
{
	// TODO: There's probably more we need to do
	return this->state != nullptr;
}
	
bool CamImpl_MMAL::PullOptions(const cvgCamFeedLocs& opts)
{
	// This will be the same option that CamImpl_OCV_HWPath uses,
	// because it's essentially the same HW and same system.
	this->devPath = opts.devicePath;
	return true;
}
