#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdbool.h>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <sysexits.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <iostream>
#include "../IManagedCam.h"


// The installation of these header files on the RaspberryPi may involve 
// checking out the userland repo, and building the repo in order
// to install linkable libraries and to copy the .h files into the
// expected locations.
// https://github.com/raspberrypi/userland
#include "bcm_host.h"


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
/// Transfered from RaspiVidYUV with some member containers spilled out
/// and the naming convention changed a bit for C++ and code convention
/// confort.
/// 
/// At a certain point, it may be sensible to refactor this class to be
/// absorbed into CamImpl_MMAL and remove RPiYUVState. But this would
/// require testing, and make the codebase quite a bit harder to compare
/// against the original raspivid(yuv) source code that this came from.
/// (wleu 06/27/2022)
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


inline void check_disable_port(MMAL_PORT_T* port)
{
	if (port && port->is_enabled)
		mmal_port_disable(port);
}

/*
 * If we are configured to use /dev/video0 as unicam (e.g. for libcamera) then
 * these legacy camera apps can't work. Fail immediately with an obvious message.
 */
// Borrowed from RaspberryPi userland repo, RaspiCamControl.c
bool check_camera_stack()
{
	// NOTES (wleu 06/27/2022)
	// For a single camera, dev/video0 is expected to be the CSI camera, if
	// connected. But this is more to see if ANY CSI cameras are connected and
	// if the RPi is properly configured for using the legacy camera stack.
	//
	// When involving multiple CSI cameras, the path being opened doesn't have
	// much to do with the SPECIFIC camera used, because the MMAL API doesn't use
	// the device path to specify which camera is being referenced (if uses integer IDs).
	int fd = open("dev/video0", O_RDWR, 0);
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

// Borrowed from RaspberryPi userland repo, RaspiCamControl.c
int raspicamcontrol_get_mem_gpu(void)
{
   char response[80] = "";
   int gpu_mem = 0;

   if (vc_gencmd(response, sizeof response, "get_mem gpu") == 0)
	  vc_gencmd_number_property(response, "gpu", &gpu_mem);

   return gpu_mem;
}

/**
* Set the flips state of the image
* @param camera Pointer to camera component
* @param hflip If true, horizontally flip the image
* @param vflip If true, vertically flip the image
*
* @return 0 if successful, non-zero if any parameters out of range
*/
// Borrowed from RaspberryPi userland repo, RaspiCamControl.c
bool raspicamcontrol_set_flips(MMAL_COMPONENT_T *camera, int hflip, int vflip)
{
	MMAL_PARAMETER_MIRROR_T mirror = {{MMAL_PARAMETER_MIRROR, sizeof(MMAL_PARAMETER_MIRROR_T)}, MMAL_PARAM_MIRROR_NONE};

	if (hflip && vflip)
		mirror.value = MMAL_PARAM_MIRROR_BOTH;
	else if (hflip)
		mirror.value = MMAL_PARAM_MIRROR_HORIZONTAL;
	else if (vflip)
		mirror.value = MMAL_PARAM_MIRROR_VERTICAL;

	mmal_port_parameter_set(camera->output[0], &mirror.hdr);
	mmal_port_parameter_set(camera->output[1], &mirror.hdr);
	return HandleMMALStatus(mmal_port_parameter_set(camera->output[2], &mirror.hdr));
}

/**
* Set exposure mode for images
* @param camera Pointer to camera component
* @param mode Exposure mode to set from
*   - MMAL_PARAM_EXPOSUREMODE_OFF,
*   - MMAL_PARAM_EXPOSUREMODE_AUTO,
*   - MMAL_PARAM_EXPOSUREMODE_NIGHT,
*   - MMAL_PARAM_EXPOSUREMODE_NIGHTPREVIEW,
*   - MMAL_PARAM_EXPOSUREMODE_BACKLIGHT,
*   - MMAL_PARAM_EXPOSUREMODE_SPOTLIGHT,
*   - MMAL_PARAM_EXPOSUREMODE_SPORTS,
*   - MMAL_PARAM_EXPOSUREMODE_SNOW,
*   - MMAL_PARAM_EXPOSUREMODE_BEACH,
*   - MMAL_PARAM_EXPOSUREMODE_VERYLONG,
*   - MMAL_PARAM_EXPOSUREMODE_FIXEDFPS,
*   - MMAL_PARAM_EXPOSUREMODE_ANTISHAKE,
*   - MMAL_PARAM_EXPOSUREMODE_FIREWORKS,
*
* @return 0 if successful, non-zero if any parameters out of range
*/
// Borrowed from RaspberryPi userland repo, RaspiCamControl.c
bool raspicamcontrol_set_exposure_mode(MMAL_COMPONENT_T *camera, MMAL_PARAM_EXPOSUREMODE_T mode)
{
	MMAL_PARAMETER_EXPOSUREMODE_T exp_mode = {{MMAL_PARAMETER_EXPOSURE_MODE,sizeof(exp_mode)}, mode};

	if (!camera)
		return 1;

	return HandleMMALStatus(mmal_port_parameter_set(camera->control, &exp_mode.hdr));
}

/**
* Set the aWB (auto white balance) mode for images
* @param camera Pointer to camera component
* @param awb_mode Value to set from
*   - MMAL_PARAM_AWBMODE_OFF,
*   - MMAL_PARAM_AWBMODE_AUTO,
*   - MMAL_PARAM_AWBMODE_SUNLIGHT,
*   - MMAL_PARAM_AWBMODE_CLOUDY,
*   - MMAL_PARAM_AWBMODE_SHADE,
*   - MMAL_PARAM_AWBMODE_TUNGSTEN,
*   - MMAL_PARAM_AWBMODE_FLUORESCENT,
*   - MMAL_PARAM_AWBMODE_INCANDESCENT,
*   - MMAL_PARAM_AWBMODE_FLASH,
*   - MMAL_PARAM_AWBMODE_HORIZON,
* @return 0 if successful, non-zero if any parameters out of range
*/
// Borrowed from RaspberryPi userland repo, RaspiCamControl.c
int raspicamcontrol_set_awb_mode(MMAL_COMPONENT_T *camera, MMAL_PARAM_AWBMODE_T awb_mode)
{
	MMAL_PARAMETER_AWBMODE_T param = {{MMAL_PARAMETER_AWB_MODE,sizeof(param)}, awb_mode};

	if (!camera)
		return 1;

	return HandleMMALStatus(mmal_port_parameter_set(camera->control, &param.hdr));
}

// Borrowed from RaspberryPi userland repo, RaspiCamControl.c
int raspicamcontrol_set_awb_gains(MMAL_COMPONENT_T *camera, float r_gain, float b_gain)
{
	MMAL_PARAMETER_AWB_GAINS_T param = {{MMAL_PARAMETER_CUSTOM_AWB_GAINS,sizeof(param)}, {0,0}, {0,0}};

	if (!camera)
		return 1;

	if (!r_gain || !b_gain)
		return 0;

	param.r_gain.num = (unsigned int)(r_gain * 65536);
	param.b_gain.num = (unsigned int)(b_gain * 65536);
	param.r_gain.den = param.b_gain.den = 65536;
	return HandleMMALStatus(mmal_port_parameter_set(camera->control, &param.hdr));
}

// Borrowed from RaspberryPi userland repo, RaspiCamControl.c
bool raspicamcontrol_set_frame_rate(MMAL_COMPONENT_T *camera, const MMAL_RATIONAL_T& rate)
{
	MMAL_PARAMETER_FRAME_RATE_T frame_rate = {{MMAL_PARAMETER_FRAME_RATE,sizeof(frame_rate)}, rate};

	if (!camera)
		return 1;

	return HandleMMALStatus(mmal_port_parameter_set(camera->control, &frame_rate.hdr));
}

/**
* Adjust the exposure time used for images
* @param camera Pointer to camera component
* @param shutter speed in microseconds
* @return 0 if successful, non-zero if any parameters out of range
*/
// Borrowed from RaspberryPi userland repo, RaspiCamControl.c
bool raspicamcontrol_set_shutter_speed(MMAL_COMPONENT_T *camera, int speed)
{
	if (!camera)
		return 1;

	return HandleMMALStatus(mmal_port_parameter_set_uint32(camera->control, MMAL_PARAMETER_SHUTTER_SPEED, speed));
}

/**
* Adjust the ISO used for images
* @param camera Pointer to camera component
* @param ISO Value to set TODO :
* @return 0 if successful, non-zero if any parameters out of range
*/
// Borrowed from RaspberryPi userland repo, RaspiCamControl.c
bool raspicamcontrol_set_ISO(MMAL_COMPONENT_T *camera, int ISO)
{
	if (!camera)
		return 1;

	return HandleMMALStatus(mmal_port_parameter_set_uint32(camera->control, MMAL_PARAMETER_ISO, ISO));
}

// Borrowed from RaspberryPi userland repo, RaspiCamControl.c
int raspicamcontrol_set_gains(MMAL_COMPONENT_T *camera, float analog, float digital)
{
	MMAL_RATIONAL_T rational = {0,65536};
	MMAL_STATUS_T status;

	if (!camera)
		return 1;

	rational.num = (unsigned int)(analog * 65536);
	status = mmal_port_parameter_set_rational(camera->control, MMAL_PARAMETER_ANALOG_GAIN, rational);
	if (status != MMAL_SUCCESS)
		return HandleMMALStatus(status);

	rational.num = (unsigned int)(digital * 65536);
	status = mmal_port_parameter_set_rational(camera->control, MMAL_PARAMETER_DIGITAL_GAIN, rational);
	return HandleMMALStatus(status);
}

/**
 * Ask GPU about its camera abilities
 * @param supported None-zero if software supports the camera
 * @param detected  None-zero if a camera has been detected
 */
 // Borrowed from RaspberryPi userland repo
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

// Borrowed from RaspberryPi userland repo
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

	  std::cout << "Detected " << param.num_cameras << " MMAL cameras." << std::endl;

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
 // Borrowed from RaspberryPi userland repo
void CamImpl_MMAL::_CameraBufferCallback(MMAL_PORT_T* port, MMAL_BUFFER_HEADER_T* buffer)
{
	MMAL_BUFFER_HEADER_T *new_buffer;

	// We pass our file handle and other stuff in via the userdata field.

	CamImpl_MMAL* camImpl = (CamImpl_MMAL*)port->userdata;

	if (camImpl)
	{
		// The size is expected to be 2088960, which is from the
		// resolution
		// 1920 x (1080 + 8)
		//
		// For more on the +8, see
		// https://forums.raspberrypi.com/viewtopic.php?t=203276
		// For some reason it doesn't seem to always rear its ugly head.
		//
		// The actual height to use when sending to OpenCV.
		int useHeight = port->format->es->video.height;
		// It may be the case that the target resolution isn't supported, in which case
		// it could be drastically different, but if it looks like we have a few extra
		// rows because our requested size was rounded up, detect the round-up and discard
		// the few extra lines
		int offTargV = useHeight - camImpl->prefHeight;
		if(offTargV > 1 && offTargV <= 32)
			useHeight = camImpl->prefHeight;

		int bytes_to_write = 
			vcos_min(
				buffer->length, 
				port->format->es->video.width * useHeight);

		if (bytes_to_write)
		{
			mmal_buffer_header_mem_lock(buffer);
			{
				cv::Ptr<cv::Mat> matImg = 
					new cv::Mat(
						useHeight,
						port->format->es->video.width,
						CV_8UC1);

				// The data in buffer->data is locked hardware memory so for the sake
				// of sanity we're not going to hang onto it. This means we're going
				// to make a copy of it.
				memcpy( 
					&matImg->data[0], 
					buffer->data, 
					bytes_to_write);

				std::lock_guard<std::mutex> guardPollSwap(camImpl->mutPolled);
				{
					// Only keep the lock as long as needed. It's only needed
					// for as long as the swap lasts; but we need to commit to
					// not touching matImg afterwards since camImpl now owns it.
					camImpl->lastPolled = matImg;
				}
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

float RationalToFloat(const MMAL_RATIONAL_T& r)
{
	return (float)((double)r.num/(double)r.den);
}

/** Default camera callback function
 * Handles the --settings
 * @param port
 * @param Callback data
 */
void CamImpl_MMAL::_CameraControlCallback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer)
{
	CamImpl_MMAL* camImpl = (CamImpl_MMAL*)port->userdata;
	if (buffer->cmd == MMAL_EVENT_PARAMETER_CHANGED)
	{
		MMAL_EVENT_PARAMETER_CHANGED_T *param = (MMAL_EVENT_PARAMETER_CHANGED_T *)buffer->data;
		switch (param->hdr.id)
		{
			case MMAL_PARAMETER_CAMERA_SETTINGS:
			{
				MMAL_PARAMETER_CAMERA_SETTINGS_T* settings = (MMAL_PARAMETER_CAMERA_SETTINGS_T*)param;
				std::lock_guard<std::mutex> guard(camImpl->cachedSettingsMutex);

				camImpl->cachedExposure		= settings->exposure;
				camImpl->cachedAnalogGain	= RationalToFloat(settings->analog_gain);
				camImpl->cachedDigitalGain	= RationalToFloat(settings->digital_gain);
				camImpl->cachedRedGain		= RationalToFloat(settings->awb_red_gain);
				camImpl->cachedBlueGain		= RationalToFloat(settings->awb_blue_gain);
				camImpl->cachedFocusPos		= settings->focus_position;

				if(camImpl->spamGains)
				{ 
					// This is put into a single stringstream and sent to std::cout all at once, or else
					// there's the possibility of the output being interlaced from multiple cameras because
					// of threading.

					std::stringstream sstrmOut;
					sstrmOut << "Camera Idx: " << camImpl->devCamID << std::endl;
					sstrmOut << "Control change for exposure     - " << camImpl->cachedExposure.value()		<< std::endl;
					sstrmOut << "Control change for analog gain  - " << camImpl->cachedAnalogGain.value()	<< std::endl;
					sstrmOut << "Control change for digital gain - " << camImpl->cachedDigitalGain.value()	<< std::endl;
					sstrmOut << "Control change for red gain     - " << camImpl->cachedRedGain.value()		<< std::endl;
					sstrmOut << "Control change for blue gain    - " << camImpl->cachedBlueGain.value()		<< std::endl;
					sstrmOut << "Control change for focus pos    - " << camImpl->cachedFocusPos.value()		<< std::endl;
					//
					std::cout << sstrmOut.str() << std::endl;
				}
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

bool ShutdownGlobal(MMAL_STATUS_T status, RPiYUVState* state, MMAL_PORT_T* camPort)
{
	HandleMMALStatus(status);

	// Disable all our ports that are not handled by connections
	check_disable_port(camPort);

	state->DisableCameraComponent();

	//raspipreview_destroy(&state.preview_parameters);
	state->DestroyCameraComponent();

	if (status != MMAL_SUCCESS)
		raspicamcontrol_check_configuration(128);

	return true;
}

bool CamImpl_MMAL::_ShutdownGlobal()
{
	return ShutdownGlobal(MMAL_SUCCESS, this->state, this->camVideoPort);
}

static MMAL_STATUS_T create_camera_component(RPiYUVState* state, int videoExposureTime)
{
	std::cout << "Creating camera component" << std::endl;
	std::cout << "\t" << "Target exposure microseconds: " << videoExposureTime << std::endl;

	// Create the component
	MMAL_COMPONENT_T* camera = nullptr;
	MMAL_STATUS_T status = mmal_component_create(MMAL_COMPONENT_DEFAULT_CAMERA, &camera);

	if (status != MMAL_SUCCESS)
	{
		std::cerr << "Failed to create camera component" << std::endl;
		exit(1);
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

	std::cout << "MMAL Preview port with addr "		<< preview_port << std::endl;
	std::cout << "MMAL Video port with addr	"		<< video_port << std::endl;
	std::cout << "MMAL Still port with addr	"		<< still_port << std::endl;
	std::cout << "MMAL Control port with addr	"	<< camera->control << std::endl;

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

	format->encoding 					= MMAL_ENCODING_OPAQUE;
	format->es->video.width 			= VCOS_ALIGN_UP(state->width, 32);
	format->es->video.height 			= VCOS_ALIGN_UP(state->height, 16);
	format->es->video.crop.x 			= 0;
	format->es->video.crop.y 			= 0;
	format->es->video.crop.width 		= state->width;
	format->es->video.crop.height 		= state->height;

	// When setting the video port, in order for this to be handled correctly,
	// the preview port ALSO needs to honor videoExposureTime.
	if(videoExposureTime <= 34000)
	{
		format->es->video.frame_rate.num 	= VIDEO_FRAME_RATE_NUM;
		format->es->video.frame_rate.den 	= VIDEO_FRAME_RATE_DEN;
	}
	else
	{
		format->es->video.frame_rate.num 	= videoExposureTime;
		format->es->video.frame_rate.den 	= 1000000; // The num should be in microseconds
	}

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

	format->encoding					= MMAL_ENCODING_I420;
	format->encoding_variant			= MMAL_ENCODING_I420;

	format->es->video.width 			= VCOS_ALIGN_UP(state->width, 32);
	format->es->video.height 			= VCOS_ALIGN_UP(state->height, 16);
	format->es->video.crop.x 			= 0;
	format->es->video.crop.y 			= 0;
	format->es->video.crop.width 		= state->width;
	format->es->video.crop.height 		= state->height;	

	// If the exposure time is greater than ~1/30th of a second, we need to lower
	// the refresh rate to allow additional photons to collect.
	if(videoExposureTime <= 34000)
	{
		format->es->video.frame_rate.num 	= VIDEO_FRAME_RATE_NUM;
		format->es->video.frame_rate.den 	= VIDEO_FRAME_RATE_DEN;
	}
	else
	{
		format->es->video.frame_rate.num 	= videoExposureTime;
		format->es->video.frame_rate.den 	= 1000000; // The num should be in microseconds
	}

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

	MMAL_PARAMETER_CHANGE_EVENT_REQUEST_T change_event_request =
	{
		{MMAL_PARAMETER_CHANGE_EVENT_REQUEST, sizeof(MMAL_PARAMETER_CHANGE_EVENT_REQUEST_T)},
		MMAL_PARAMETER_CAMERA_SETTINGS, 1
	};

	status = mmal_port_parameter_set(camera->control, &change_event_request.hdr);
	if ( status != MMAL_SUCCESS )
	{ 
		std::cerr << "No camera settings events" << std::endl;
		return status;
	}

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
	static std::mutex initMut;
	std::cout << "BEGIN CamImpl_MMAL::InitPrereqs()" << std::endl;

	// It's not known if this init process is thread safe, so we're only
	// allowing once peice of code to be here at a time.
	//
	// The risk is CamImpl_MMAL::ActivateImpl(), which invokes this
	// function, could be called from multiple cameras being initialized 
	// at once.
	{

		std::lock_guard<std::mutex> initGuard(initMut);
		std::cout << "\tENTER CamImpl_MMAL::InitPrereqs() GUARD" << std::endl;

		++instsInit;
		if(instsInit != 1)
			return;

		std::cout << "\tDoing on-time MMAL inits" << std::endl;
	
		// The Raspberry Pi requires that the bcm_host_init() function is called first before any GPU calls can be made.
		// https://elinux.org/Raspberry_Pi_VideoCore_APIs
		bcm_host_init();

		// Register our application with the logging system
		vcos_log_register("HMDOpView", VCOS_LOG_CATEGORY);

		signal(SIGINT, default_signal_handler);
		// Disable USR1 for the moment - may be reenabled if go in to signal capture mode
		signal(SIGUSR1, SIG_IGN);

		std::cout << "\tEXIT CamImpl_MMAL::InitPrereqs() GUARD" << std::endl;
	}
	std::cout << "EXIT CamImpl_MMAL::InitPrereqs()" << std::endl;
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
	std::cout << "Activating MMAL Impl" << std::endl;

	InitPrereqs();

	// TODO: Assert non-null
	this->state = new RPiYUVState();
	this->state->SetDefaults();
	this->state->cameraNum = this->devCamID;

	if(this->prefWidth != 0)
		this->state->width = this->prefWidth;

	if(this->prefHeight != 0)
		this->state->height = this->prefHeight;

	// Setup for sensor specific parameters, only set W/H settings if zero on entry

	std::cout << "Getting sensor defaults" << std::endl;

	get_sensor_defaults(
		this->state->cameraNum, 
		this->state->camera_name, 
		this->state->width, 
		this->state->height);

	std::cout << "Queried camera " << this->devCamID << " defaults with Width : " << this->state->width << " - Height : " << this->state->height << std::endl;
	std::cout << "BEGIN Creating camera component : " << this->devCamID << std::endl;

	std::cout << 
		"Get sensor defaults - camera num: "	<< this->state->cameraNum << std::endl <<
		"Get sensor defaults - camera name: "	<< this->state->camera_name << std::endl << 
		"Get sensor defaults - camera width: "	<< this->state->width << std::endl <<
		"Get sensor defaults - camera height: "	<< this->state->height << std::endl;

	std::cout << "Creating camera componenet" << std::endl;
	std::cout << "\t" << this->videoExposureTime << std::endl;

	MMAL_STATUS_T status = create_camera_component(state, this->videoExposureTime ); 

	if(this->whitebalanceGain.has_value())
	{ 
		raspicamcontrol_set_awb_mode(
			this->state->camera_component, 
			MMAL_PARAM_AWBMODE_OFF);
		//
		raspicamcontrol_set_awb_gains(
			this->state->camera_component, 
			this->whitebalanceGain.value().redGain, 
			this->whitebalanceGain.value().blueGain);
	
		std::cout << "Setting explicit white balance for camera : " << this->state->cameraNum << std::endl;
		std::cout << "\tRed - " << this->whitebalanceGain.value().redGain << std::endl;
		std::cout << "\tBlue - " << this->whitebalanceGain.value().blueGain << std::endl;
	}
	else
	{
		std::cout << "Leaving automatic white balance for camera : " << this->state->cameraNum << std::endl;
	}
	
	if(this->cameraGain.has_value())
	{ 
		raspicamcontrol_set_gains(
			this->state->camera_component, 
			this->cameraGain.value().analogGain, 
			this->cameraGain.value().digitalGain);
	
		std::cout << "Setting explicit analog/digital gain for camera : " << this->state->cameraNum << std::endl;
		std::cout << "\tAnalog - " << this->cameraGain.value().analogGain << std::endl;
		std::cout << "\tDigital - " << this->cameraGain.value().digitalGain << std::endl;
	}
	else
	{
		std::cout << "Leaving automatic gain control for camera : " << this->state->cameraNum << std::endl;
	}
	
	if(this->spamGains)
	{
		std::cout << "Camera feed is set to output control values as they change." << std::endl;
	}

	//
	//
	//////////////////////////////////////////////////

	std::cout << "END Creating camera component : " <<  this->devCamID << std::endl;


	if(status != MMAL_SUCCESS)
	{
		std::cerr << __func__ << ": Failed to create camera component" << std::endl;
		return false;
	}

	// Enable the camera, and tell it its control callback function
	this->state->camera_component->control->userdata = (struct MMAL_PORT_USERDATA_T *)this;
	status = mmal_port_enable(this->state->camera_component->control, CamImpl_MMAL::_CameraControlCallback);

	this->camVideoPort = this->state->camera_component->output[MMALCamPort::Video];

	raspicamcontrol_set_flips(
		this->state->camera_component, 
		this->flipHoriz ? 1 : 0, 
		this->flipVert ? 1 : 0);

	if(this->videoExposureTime != 0)
	{
		std::cout << "Video shutter exposure value at " << this->videoExposureTime << std::endl;

		// It's unsure if this should be turned off. Both on and off will change the brightness when
		// the shutter speed is modified, but in different ways (exposure off will be brighter).
		//
		//raspicamcontrol_set_exposure_mode(this->state->camera_component, MMAL_PARAM_EXPOSUREMODE_OFF);

		raspicamcontrol_set_shutter_speed(this->state->camera_component, this->videoExposureTime);
		MMAL_RATIONAL_T framerate = {this->videoExposureTime, 1000000};
		raspicamcontrol_set_frame_rate(this->state->camera_component, framerate);
	}


	if (status != MMAL_SUCCESS)
	{
		HandleMMALStatus(status);
		std::cerr << __func__ << ": Failed to connect camera to preview" << std::endl;
		return false;
	}

	// Set up our userdata - this is passed though to the callback where we need the information.
	this->polling = true;
	this->camVideoPort->userdata = (struct MMAL_PORT_USERDATA_T *)this;
	std::cout << "Setting CamViewPort " << this->camVideoPort << " user data to " << this << std::endl;
	
	// Enable the camera video port and tell it its callback function
	status = mmal_port_enable(this->camVideoPort, CamImpl_MMAL::_CameraBufferCallback);

	if (status != MMAL_SUCCESS)
	{
		std::cerr << "Failed to setup camera output" << std::endl;
		exit(1);
		return ShutdownGlobal(status, this->state, this->camVideoPort);
	}

	// Send all the buffers to the camera video port
	int num = mmal_queue_length(this->state->camera_pool->queue);
	for (int q = 0; q < num; q++)
	{
		MMAL_BUFFER_HEADER_T *buffer = 
			mmal_queue_get(this->state->camera_pool->queue);

		if (!buffer)
			std::cerr << "Unable to get a required buffer " << q << " from pool queue" << std::endl;

		if (mmal_port_send_buffer(this->camVideoPort, buffer)!= MMAL_SUCCESS)
			std::cerr << "Unable to send a buffer to camera video port (" << q << ")" << std::endl;
	}	

	// Turn on streaming to the callback
	mmal_port_parameter_set_boolean(
		this->camVideoPort, 
		MMAL_PARAMETER_CAPTURE, 
		true);

	std::cout << "Successful MMAL activation : " << this->devCamID << std::endl;

	return true;
}

bool CamImpl_MMAL::DeactivateImpl()
{
	this->polling = false;

	if(this->state != nullptr)
	{
		delete this->state;
		this->state = nullptr;
	}
	return true;
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

CamImpl_MMAL::CamImpl_MMAL(int devCamID)
{
	this->devCamID = devCamID;
}
	
VideoPollType CamImpl_MMAL::PollType()
{
	return VideoPollType::MMAL;
}

bool CamImpl_MMAL::IsValid()
{
	return 
		this->state != nullptr && 
		this->camVideoPort != nullptr;
}
	
bool CamImpl_MMAL::PullOptions(const cvgCamFeedLocs& opts)
{
	this->ICamImpl::PullOptions(opts);
	this->devCamID			= opts.camMMALIdx;
	this->videoExposureTime = opts.videoExposureTime;

	this->spamGains			= opts.spamGains;
	this->cameraGain		= opts.cameraGain;
	this->whitebalanceGain	= opts.whitebalanceGain;
	return true;
}

void CamImpl_MMAL::DelegatedInjectIntoDicom(DcmDataset* dicomData)
{
	// TODO: Poll camera name and insert
	InsertAcquisitionContextInfo(dicomData, "camera_sensor",		this->state->camera_name);
	{
		std::lock_guard<std::mutex> guard(this->cachedSettingsMutex);

		if(this->cachedExposure.has_value())
		{
			// The value is in microseconds, but we've decided on storing in milliseconds, hence the divide by 1000
			InsertAcquisitionContextInfo(dicomData, "exposure", std::to_string(this->cachedExposure.value() / 1000.0f));
		}

		if(this->cachedAnalogGain.has_value())
			InsertAcquisitionContextInfo(dicomData, "analog_gain", std::to_string(this->cachedAnalogGain.value()));

		if(this->cachedDigitalGain.has_value())
			InsertAcquisitionContextInfo(dicomData, "digital_gain", std::to_string(this->cachedDigitalGain.value()));

		if(this->cachedRedGain.has_value())
			InsertAcquisitionContextInfo(dicomData, "red_gain", std::to_string(this->cachedRedGain.value()));

		if(this->cachedBlueGain.has_value())
			InsertAcquisitionContextInfo(dicomData, "blue_gain", std::to_string(this->cachedBlueGain.value()));

		if(this->cachedFocusPos.has_value())
			InsertAcquisitionContextInfo(dicomData, "focus_pos", std::to_string(this->cachedFocusPos.value()));
	}
}