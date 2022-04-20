/*
Copyright (c) 2013, Broadcom Europe Ltd
Copyright (c) 2013, James Hughes
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <memory.h>
#include <ctype.h>

#include "interface/vcos/vcos.h"

#include "interface/vmcs_host/vc_vchi_gencmd.h"
#include "interface/mmal/mmal.h"
#include "interface/mmal/mmal_logging.h"
#include "interface/mmal/util/mmal_util.h"
#include "interface/mmal/util/mmal_util_params.h"
#include "interface/mmal/util/mmal_default_components.h"
#include "interface/mmal/util/mmal_connection.h"

#include "RaspiCamControl.h"
#include "RaspiCLI.h"
#include "RaspiHelpers.h"

/// Structure to cross reference exposure strings against the MMAL parameter equivalent
static XREF_T  exposure_map[] =
{
   {"off",           MMAL_PARAM_EXPOSUREMODE_OFF},
   {"auto",          MMAL_PARAM_EXPOSUREMODE_AUTO},
   {"night",         MMAL_PARAM_EXPOSUREMODE_NIGHT},
   {"nightpreview",  MMAL_PARAM_EXPOSUREMODE_NIGHTPREVIEW},
   {"backlight",     MMAL_PARAM_EXPOSUREMODE_BACKLIGHT},
   {"spotlight",     MMAL_PARAM_EXPOSUREMODE_SPOTLIGHT},
   {"sports",        MMAL_PARAM_EXPOSUREMODE_SPORTS},
   {"snow",          MMAL_PARAM_EXPOSUREMODE_SNOW},
   {"beach",         MMAL_PARAM_EXPOSUREMODE_BEACH},
   {"verylong",      MMAL_PARAM_EXPOSUREMODE_VERYLONG},
   {"fixedfps",      MMAL_PARAM_EXPOSUREMODE_FIXEDFPS},
   {"antishake",     MMAL_PARAM_EXPOSUREMODE_ANTISHAKE},
   {"fireworks",     MMAL_PARAM_EXPOSUREMODE_FIREWORKS}
};

static const int exposure_map_size = sizeof(exposure_map) / sizeof(exposure_map[0]);

/// Structure to cross reference flicker avoid strings against the MMAL parameter equivalent

static XREF_T  flicker_avoid_map[] =
{
   {"off",           MMAL_PARAM_FLICKERAVOID_OFF},
   {"auto",          MMAL_PARAM_FLICKERAVOID_AUTO},
   {"50hz",          MMAL_PARAM_FLICKERAVOID_50HZ},
   {"60hz",          MMAL_PARAM_FLICKERAVOID_60HZ}
};

static const int flicker_avoid_map_size = sizeof(flicker_avoid_map) / sizeof(flicker_avoid_map[0]);

/// Structure to cross reference awb strings against the MMAL parameter equivalent
static XREF_T awb_map[] =
{
   {"off",           MMAL_PARAM_AWBMODE_OFF},
   {"auto",          MMAL_PARAM_AWBMODE_AUTO},
   {"sun",           MMAL_PARAM_AWBMODE_SUNLIGHT},
   {"cloud",         MMAL_PARAM_AWBMODE_CLOUDY},
   {"shade",         MMAL_PARAM_AWBMODE_SHADE},
   {"tungsten",      MMAL_PARAM_AWBMODE_TUNGSTEN},
   {"fluorescent",   MMAL_PARAM_AWBMODE_FLUORESCENT},
   {"incandescent",  MMAL_PARAM_AWBMODE_INCANDESCENT},
   {"flash",         MMAL_PARAM_AWBMODE_FLASH},
   {"horizon",       MMAL_PARAM_AWBMODE_HORIZON},
   {"greyworld",     MMAL_PARAM_AWBMODE_GREYWORLD}
};

static const int awb_map_size = sizeof(awb_map) / sizeof(awb_map[0]);

static XREF_T metering_mode_map[] =
{
   {"average",       MMAL_PARAM_EXPOSUREMETERINGMODE_AVERAGE},
   {"spot",          MMAL_PARAM_EXPOSUREMETERINGMODE_SPOT},
   {"backlit",       MMAL_PARAM_EXPOSUREMETERINGMODE_BACKLIT},
   {"matrix",        MMAL_PARAM_EXPOSUREMETERINGMODE_MATRIX}
};

static const int metering_mode_map_size = sizeof(metering_mode_map)/sizeof(metering_mode_map[0]);

static XREF_T drc_mode_map[] =
{
   {"off",           MMAL_PARAMETER_DRC_STRENGTH_OFF},
   {"low",           MMAL_PARAMETER_DRC_STRENGTH_LOW},
   {"med",           MMAL_PARAMETER_DRC_STRENGTH_MEDIUM},
   {"high",          MMAL_PARAMETER_DRC_STRENGTH_HIGH}
};

static const int drc_mode_map_size = sizeof(drc_mode_map)/sizeof(drc_mode_map[0]);

static XREF_T stereo_mode_map[] =
{
   {"off",           MMAL_STEREOSCOPIC_MODE_NONE},
   {"sbs",           MMAL_STEREOSCOPIC_MODE_SIDE_BY_SIDE},
   {"tb",            MMAL_STEREOSCOPIC_MODE_TOP_BOTTOM},
};

static const int stereo_mode_map_size = sizeof(stereo_mode_map)/sizeof(stereo_mode_map[0]);

enum
{
   CommandSharpness,
   CommandContrast,
   CommandBrightness,
   CommandISO,
   CommandVideoStab,
   CommandEVComp,
   CommandExposure,
   CommandAWB,
   CommandMeterMode,
   CommandShutterSpeed,
   CommandAwbGains,
   CommandDRCLevel,
   CommandStatsPass,
   CommandStereoMode,
   CommandStereoDecimate,
   CommandStereoSwap,
   CommandFlicker,
   CommandAnalogGain,
   CommandDigitalGain,
   CommandSettings,
   CommandFocusWindow
};

static COMMAND_LIST  cmdline_commands[] =
{
   {CommandSharpness,   "-sharpness", "sh", "Set image sharpness (-100 to 100)",  1},
   {CommandContrast,    "-contrast",  "co", "Set image contrast (-100 to 100)",  1},
   {CommandBrightness,  "-brightness","br", "Set image brightness (0 to 100)",  1},
   {CommandISO,         "-ISO",       "ISO","Set capture ISO",  1},
   {CommandVideoStab,   "-vstab",     "vs", "Turn on video stabilisation", 0},
   {CommandEVComp,      "-ev",        "ev", "Set EV compensation - steps of 1/6 stop",  1},
   {CommandExposure,    "-exposure",  "ex", "Set exposure mode (see Notes)", 1},
   {CommandFlicker,     "-flicker",   "fli","Set flicker avoid mode (see Notes)", 1},
   {CommandAWB,         "-awb",       "awb","Set AWB mode (see Notes)", 1},
   {CommandMeterMode,   "-metering",  "mm", "Set metering mode (see Notes)", 1},
   {CommandShutterSpeed,"-shutter",   "ss", "Set shutter speed in microseconds", 1},
   {CommandAwbGains,    "-awbgains",  "awbg", "Set AWB gains - AWB mode must be off", 1},
   {CommandDRCLevel,    "-drc",       "drc", "Set DRC Level (see Notes)", 1},
   {CommandStatsPass,   "-stats",     "st", "Force recomputation of statistics on stills capture pass"},
   {CommandStereoMode,  "-stereo",    "3d", "Select stereoscopic mode", 1},
   {CommandStereoDecimate,"-decimate","dec", "Half width/height of stereo image"},
   {CommandStereoSwap,  "-3dswap",    "3dswap", "Swap camera order for stereoscopic"},
   {CommandAnalogGain,  "-analoggain", "ag", "Set the analog gain (floating point)", 1},
   {CommandDigitalGain, "-digitalgain", "dg", "Set the digital gain (floating point)", 1},
   {CommandSettings,    "-settings",   "set","Retrieve camera settings and write to stdout", 0},
   {CommandFocusWindow, "-focus",      "fw","Draw a window with the focus FoM value on the image.", 0},
};

static int cmdline_commands_size = sizeof(cmdline_commands) / sizeof(cmdline_commands[0]);


#define parameter_reset -99999

#define zoom_full_16P16 ((unsigned int)(65536 * 0.15))
#define zoom_increment_16P16 (65536UL / 10)

/**
 * Update the passed in parameter according to the rest of the parameters
 * passed in.
 *
 *
 * @return 0 if reached end of cycle for this parameter, !0 otherwise
 */
static int update_cycle_parameter(int *option, int min, int max, int increment)
{
   vcos_assert(option);
   if (!option)
      return 0;

   if (*option == parameter_reset)
      *option = min - increment;

   *option += increment;

   if (*option > max)
   {
      *option = parameter_reset;
      return 0;
   }
   else
      return 1;
}

/**
 * Convert string to the MMAL parameter for exposure mode
 * @param str Incoming string to match
 * @return MMAL parameter matching the string, or the AUTO option if no match found
 */
static MMAL_PARAM_EXPOSUREMODE_T exposure_mode_from_string(const char *str)
{
   int i = raspicli_map_xref(str, exposure_map, exposure_map_size);

   if( i != -1)
      return (MMAL_PARAM_EXPOSUREMODE_T)i;

   vcos_log_error("Unknown exposure mode: %s", str);
   return MMAL_PARAM_EXPOSUREMODE_AUTO;
}

/**
 * Convert string to the MMAL parameter for flicker avoid mode
 * @param str Incoming string to match
 * @return MMAL parameter matching the string, or the AUTO option if no match found
 */
static MMAL_PARAM_FLICKERAVOID_T flicker_avoid_mode_from_string(const char *str)
{
   int i = raspicli_map_xref(str, flicker_avoid_map, flicker_avoid_map_size);

   if( i != -1)
      return (MMAL_PARAM_FLICKERAVOID_T)i;

   vcos_log_error("Unknown flicker avoid mode: %s", str);
   return MMAL_PARAM_FLICKERAVOID_OFF;
}

/**
 * Convert string to the MMAL parameter for AWB mode
 * @param str Incoming string to match
 * @return MMAL parameter matching the string, or the AUTO option if no match found
 */
static MMAL_PARAM_AWBMODE_T awb_mode_from_string(const char *str)
{
   int i = raspicli_map_xref(str, awb_map, awb_map_size);

   if( i != -1)
      return (MMAL_PARAM_AWBMODE_T)i;

   vcos_log_error("Unknown awb mode: %s", str);
   return MMAL_PARAM_AWBMODE_AUTO;
}

/**
 * Convert string to the MMAL parameter for exposure metering mode
 * @param str Incoming string to match
 * @return MMAL parameter matching the string, or the AUTO option if no match found
 */
static MMAL_PARAM_EXPOSUREMETERINGMODE_T metering_mode_from_string(const char *str)
{
   int i = raspicli_map_xref(str, metering_mode_map, metering_mode_map_size);

   if( i != -1)
      return (MMAL_PARAM_EXPOSUREMETERINGMODE_T)i;

   vcos_log_error("Unknown metering mode: %s", str);
   return MMAL_PARAM_EXPOSUREMETERINGMODE_AVERAGE;
}

/**
 * Convert string to the MMAL parameter for DRC level
 * @param str Incoming string to match
 * @return MMAL parameter matching the string, or the AUTO option if no match found
 */
static MMAL_PARAMETER_DRC_STRENGTH_T drc_mode_from_string(const char *str)
{
   int i = raspicli_map_xref(str, drc_mode_map, drc_mode_map_size);

   if( i != -1)
      return (MMAL_PARAMETER_DRC_STRENGTH_T)i;

   vcos_log_error("Unknown DRC level: %s", str);
   return MMAL_PARAMETER_DRC_STRENGTH_OFF;
}

/**
 * Convert string to the MMAL parameter for exposure metering mode
 * @param str Incoming string to match
 * @return MMAL parameter matching the string, or the AUTO option if no match found
 */
static MMAL_STEREOSCOPIC_MODE_T stereo_mode_from_string(const char *str)
{
   int i = raspicli_map_xref(str, stereo_mode_map, stereo_mode_map_size);

   if( i != -1)
      return (MMAL_STEREOSCOPIC_MODE_T)i;

   vcos_log_error("Unknown metering mode: %s", str);
   return MMAL_STEREOSCOPIC_MODE_NONE;
}

/**
 * Parse a possible command pair - command and parameter
 * @param arg1 Command
 * @param arg2 Parameter (could be NULL)
 * @return How many parameters were used, 0,1,2
 */
int raspicamcontrol_parse_cmdline(RASPICAM_CAMERA_PARAMETERS *params, const char *arg1, const char *arg2)
{
   int command_id, used = 0, num_parameters;

   if (!arg1)
      return 0;

   command_id = raspicli_get_command_id(cmdline_commands, cmdline_commands_size, arg1, &num_parameters);

   // If invalid command, or we are missing a parameter, drop out
   if (command_id==-1 || (command_id != -1 && num_parameters > 0 && arg2 == NULL))
      return 0;

   switch (command_id)
   {
   case CommandSharpness : // sharpness - needs single number parameter
      sscanf(arg2, "%d", &params->sharpness);
      used = 2;
      break;

   case CommandContrast : // contrast - needs single number parameter
      sscanf(arg2, "%d", &params->contrast);
      used = 2;
      break;

   case CommandBrightness : // brightness - needs single number parameter
      sscanf(arg2, "%d", &params->brightness);
      used = 2;
      break;

   case CommandISO : // ISO - needs single number parameter
      sscanf(arg2, "%d", &params->ISO);
      used = 2;
      break;

   case CommandVideoStab : // video stabilisation - if here, its on
      params->videoStabilisation = 1;
      used = 1;
      break;

   case CommandEVComp : // EV - needs single number parameter
      sscanf(arg2, "%d", &params->exposureCompensation);
      used = 2;
      break;

   case CommandExposure : // exposure mode - needs string
      params->exposureMode = exposure_mode_from_string(arg2);
      used = 2;
      break;

   case CommandFlicker : // flicker avoid mode - needs string
      params->flickerAvoidMode = flicker_avoid_mode_from_string(arg2);
      used = 2;
      break;

   case CommandAWB : // AWB mode - needs single number parameter
      params->awbMode = awb_mode_from_string(arg2);
      used = 2;
      break;
	  
   case CommandMeterMode:
      params->exposureMeterMode = metering_mode_from_string(arg2);
      used = 2;
      break;

   case CommandShutterSpeed : // Shutter speed needs single number parameter
   {
      sscanf(arg2, "%d", &params->shutter_speed);
      used = 2;
      break;
   }

   case CommandAwbGains :
   {
      double r,b;
      int args;

      args = sscanf(arg2, "%lf,%lf", &r,&b);

      if (args != 2 || r > 8.0 || b > 8.0)
      {
         return 0;
      }

      params->awb_gains_r = r;
      params->awb_gains_b = b;

      used = 2;
      break;
   }

   case CommandDRCLevel:
   {
      params->drc_level = drc_mode_from_string(arg2);
      used = 2;
      break;
   }

   case CommandStatsPass:
   {
      params->stats_pass = MMAL_TRUE;
      used = 1;
      break;
   }

   case CommandStereoMode:
   {
      params->stereo_mode.mode = stereo_mode_from_string(arg2);
      used = 2;
      break;
   }

   case CommandStereoDecimate:
   {
      params->stereo_mode.decimate = MMAL_TRUE;
      used = 1;
      break;
   }

   case CommandStereoSwap:
   {
      params->stereo_mode.swap_eyes = MMAL_TRUE;
      used = 1;
      break;
   }

   case CommandAnalogGain:
   {
      double gain;
      int args;

      args = sscanf(arg2, "%lf", &gain);

      if (args != 1 || gain > 16.0)
      {
         return 0;
      }

      params->analog_gain = gain;

      used = 2;
      break;
   }
   case CommandDigitalGain:
   {
      double gain;
      int args;

      args = sscanf(arg2, "%lf", &gain);

      if (args != 1 || gain > 64.0)
      {
         return 0;
      }

      params->digital_gain = gain;

      used = 2;
      break;
   }

   case CommandSettings:
   {
      params->settings = 1;
      used = 1;
      break;
   }

   case CommandFocusWindow:
   {
      params->focus_window = 1;
      used = 1;
      break;
   }

   }

   return used;
}

/**
 * Display help for command line options
 */
void raspicamcontrol_display_help()
{
   int i;

   fprintf(stdout, "\nImage parameter commands\n\n");

   raspicli_display_help(cmdline_commands, cmdline_commands_size);

   fprintf(stdout, "\n\nNotes\n\nExposure mode options :\n%s", exposure_map[0].mode );

   for (i=1; i<exposure_map_size; i++)
   {
      fprintf(stdout, ",%s", exposure_map[i].mode);
   }

   fprintf(stdout, "\n\nFlicker avoid mode options :\n%s", flicker_avoid_map[0].mode );

   for (i=1; i<flicker_avoid_map_size; i++)
   {
      fprintf(stdout, ",%s", flicker_avoid_map[i].mode);
   }

   fprintf(stdout, "\n\nAWB mode options :\n%s", awb_map[0].mode );

   for (i=1; i<awb_map_size; i++)
   {
      fprintf(stdout, ",%s", awb_map[i].mode);
   }

   fprintf(stdout, "\n\nMetering Mode options :\n%s", metering_mode_map[0].mode );

   for (i=1; i<metering_mode_map_size; i++)
   {
      fprintf(stdout, ",%s", metering_mode_map[i].mode);
   }

   fprintf(stdout, "\n\nDynamic Range Compression (DRC) options :\n%s", drc_mode_map[0].mode );

   for (i=1; i<drc_mode_map_size; i++)
   {
      fprintf(stdout, ",%s", drc_mode_map[i].mode);
   }

   fprintf(stdout, "\n");
}


/**
 * Give the supplied parameter block a set of default values
 * @params Pointer to parameter block
 */
void raspicamcontrol_set_defaults(RASPICAM_CAMERA_PARAMETERS *params)
{
   vcos_assert(params);

   params->sharpness = 0;
   params->contrast = 0;
   params->brightness = 50;
   params->ISO = 0;                    // 0 = auto
   params->videoStabilisation = 0;
   params->exposureCompensation = 0;
   params->exposureMode = MMAL_PARAM_EXPOSUREMODE_AUTO;
   params->flickerAvoidMode = MMAL_PARAM_FLICKERAVOID_OFF;
   params->exposureMeterMode = MMAL_PARAM_EXPOSUREMETERINGMODE_AVERAGE;
   params->awbMode = MMAL_PARAM_AWBMODE_AUTO;
   params->shutter_speed = 0;          // 0 = auto
   params->awb_gains_r = 0;      // Only have any function if AWB OFF is used.
   params->awb_gains_b = 0;
   params->drc_level = MMAL_PARAMETER_DRC_STRENGTH_OFF;
   params->stats_pass = MMAL_FALSE;
   params->stereo_mode.mode = MMAL_STEREOSCOPIC_MODE_NONE;
   params->stereo_mode.decimate = MMAL_FALSE;
   params->stereo_mode.swap_eyes = MMAL_FALSE;
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

   result += raspicamcontrol_set_sharpness(camera, params->sharpness);
   result += raspicamcontrol_set_contrast(camera, params->contrast);
   result += raspicamcontrol_set_brightness(camera, params->brightness);
   result += raspicamcontrol_set_ISO(camera, params->ISO);
   result += raspicamcontrol_set_exposure_compensation(camera, params->exposureCompensation);
   result += raspicamcontrol_set_exposure_mode(camera, params->exposureMode);
   result += raspicamcontrol_set_flicker_avoid_mode(camera, params->flickerAvoidMode);
   result += raspicamcontrol_set_metering_mode(camera, params->exposureMeterMode);
   result += raspicamcontrol_set_awb_mode(camera, params->awbMode);
   result += raspicamcontrol_set_awb_gains(camera, params->awb_gains_r, params->awb_gains_b);
   result += raspicamcontrol_set_shutter_speed(camera, params->shutter_speed);
   result += raspicamcontrol_set_DRC(camera, params->drc_level);
   result += raspicamcontrol_set_stats_pass(camera, params->stats_pass);
   result += raspicamcontrol_set_gains(camera, params->analog_gain, params->digital_gain);
   result += raspicamcontrol_set_focus_window(camera, params->focus_window);

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

/**
 * Set the sharpness of the image
 * @param camera Pointer to camera component
 * @param sharpness Sharpness adjustment -100 to 100
 */
int raspicamcontrol_set_sharpness(MMAL_COMPONENT_T *camera, int sharpness)
{
   int ret = 0;

   if (!camera)
      return 1;

   if (sharpness >= -100 && sharpness <= 100)
   {
      MMAL_RATIONAL_T value = {sharpness, 100};
      ret = mmal_status_to_int(mmal_port_parameter_set_rational(camera->control, MMAL_PARAMETER_SHARPNESS, value));
   }
   else
   {
      vcos_log_error("Invalid sharpness value");
      ret = 1;
   }

   return ret;
}

/**
 * Set the contrast adjustment for the image
 * @param camera Pointer to camera component
 * @param contrast Contrast adjustment -100 to  100
 * @return
 */
int raspicamcontrol_set_contrast(MMAL_COMPONENT_T *camera, int contrast)
{
   int ret = 0;

   if (!camera)
      return 1;

   if (contrast >= -100 && contrast <= 100)
   {
      MMAL_RATIONAL_T value = {contrast, 100};
      ret = mmal_status_to_int(mmal_port_parameter_set_rational(camera->control, MMAL_PARAMETER_CONTRAST, value));
   }
   else
   {
      vcos_log_error("Invalid contrast value");
      ret = 1;
   }

   return ret;
}

/**
 * Adjust the brightness level for images
 * @param camera Pointer to camera component
 * @param brightness Value to adjust, 0 to 100
 * @return 0 if successful, non-zero if any parameters out of range
 */
int raspicamcontrol_set_brightness(MMAL_COMPONENT_T *camera, int brightness)
{
   int ret = 0;

   if (!camera)
      return 1;

   if (brightness >= 0 && brightness <= 100)
   {
      MMAL_RATIONAL_T value = {brightness, 100};
      ret = mmal_status_to_int(mmal_port_parameter_set_rational(camera->control, MMAL_PARAMETER_BRIGHTNESS, value));
   }
   else
   {
      vcos_log_error("Invalid brightness value");
      ret = 1;
   }

   return ret;
}

/**
 * Adjust the ISO used for images
 * @param camera Pointer to camera component
 * @param ISO Value to set TODO :
 * @return 0 if successful, non-zero if any parameters out of range
 */
int raspicamcontrol_set_ISO(MMAL_COMPONENT_T *camera, int ISO)
{
   if (!camera)
      return 1;

   return mmal_status_to_int(mmal_port_parameter_set_uint32(camera->control, MMAL_PARAMETER_ISO, ISO));
}

/**
 * Adjust the metering mode for images
 * @param camera Pointer to camera component
 * @param saturation Value from following
 *   - MMAL_PARAM_EXPOSUREMETERINGMODE_AVERAGE,
 *   - MMAL_PARAM_EXPOSUREMETERINGMODE_SPOT,
 *   - MMAL_PARAM_EXPOSUREMETERINGMODE_BACKLIT,
 *   - MMAL_PARAM_EXPOSUREMETERINGMODE_MATRIX
 * @return 0 if successful, non-zero if any parameters out of range
 */
int raspicamcontrol_set_metering_mode(MMAL_COMPONENT_T *camera, MMAL_PARAM_EXPOSUREMETERINGMODE_T m_mode )
{
   MMAL_PARAMETER_EXPOSUREMETERINGMODE_T meter_mode = {{MMAL_PARAMETER_EXP_METERING_MODE,sizeof(meter_mode)},
      m_mode
   };
   if (!camera)
      return 1;

   return mmal_status_to_int(mmal_port_parameter_set(camera->control, &meter_mode.hdr));
}

/**
 * Adjust the exposure compensation for images (EV)
 * @param camera Pointer to camera component
 * @param exp_comp Value to adjust, -10 to +10
 * @return 0 if successful, non-zero if any parameters out of range
 */
int raspicamcontrol_set_exposure_compensation(MMAL_COMPONENT_T *camera, int exp_comp)
{
   if (!camera)
      return 1;

   return mmal_status_to_int(mmal_port_parameter_set_int32(camera->control, MMAL_PARAMETER_EXPOSURE_COMP, exp_comp));
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
int raspicamcontrol_set_exposure_mode(MMAL_COMPONENT_T *camera, MMAL_PARAM_EXPOSUREMODE_T mode)
{
   MMAL_PARAMETER_EXPOSUREMODE_T exp_mode = {{MMAL_PARAMETER_EXPOSURE_MODE,sizeof(exp_mode)}, mode};

   if (!camera)
      return 1;

   return mmal_status_to_int(mmal_port_parameter_set(camera->control, &exp_mode.hdr));
}


/**
 * Set flicker avoid mode for images
 * @param camera Pointer to camera component
 * @param mode Exposure mode to set from
 *   - MMAL_PARAM_FLICKERAVOID_OFF,
 *   - MMAL_PARAM_FLICKERAVOID_AUTO,
 *   - MMAL_PARAM_FLICKERAVOID_50HZ,
 *   - MMAL_PARAM_FLICKERAVOID_60HZ,
 *
 * @return 0 if successful, non-zero if any parameters out of range
 */
int raspicamcontrol_set_flicker_avoid_mode(MMAL_COMPONENT_T *camera, MMAL_PARAM_FLICKERAVOID_T mode)
{
   MMAL_PARAMETER_FLICKERAVOID_T fl_mode = {{MMAL_PARAMETER_FLICKER_AVOID,sizeof(fl_mode)}, mode};

   if (!camera)
      return 1;

   return mmal_status_to_int(mmal_port_parameter_set(camera->control, &fl_mode.hdr));
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
int raspicamcontrol_set_awb_mode(MMAL_COMPONENT_T *camera, MMAL_PARAM_AWBMODE_T awb_mode)
{
   MMAL_PARAMETER_AWBMODE_T param = {{MMAL_PARAMETER_AWB_MODE,sizeof(param)}, awb_mode};

   if (!camera)
      return 1;

   return mmal_status_to_int(mmal_port_parameter_set(camera->control, &param.hdr));
}

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
   return mmal_status_to_int(mmal_port_parameter_set(camera->control, &param.hdr));
}

/**
 * Zoom in and Zoom out by changing ROI
 * @param camera Pointer to camera component
 * @param zoom_command zoom command enum
 * @return 0 if successful, non-zero otherwise
 */
int raspicamcontrol_zoom_in_zoom_out(MMAL_COMPONENT_T *camera, ZOOM_COMMAND_T zoom_command, PARAM_FLOAT_RECT_T *roi)
{
   MMAL_PARAMETER_INPUT_CROP_T crop;
   crop.hdr.id = MMAL_PARAMETER_INPUT_CROP;
   crop.hdr.size = sizeof(crop);

   if (mmal_port_parameter_get(camera->control, &crop.hdr) != MMAL_SUCCESS)
   {
      vcos_log_error("mmal_port_parameter_get(camera->control, &crop.hdr) failed, skip it");
      return 0;
   }

   if (zoom_command == ZOOM_IN)
   {
      if (crop.rect.width <= (zoom_full_16P16 + zoom_increment_16P16))
      {
         crop.rect.width = zoom_full_16P16;
         crop.rect.height = zoom_full_16P16;
      }
      else
      {
         crop.rect.width -= zoom_increment_16P16;
         crop.rect.height -= zoom_increment_16P16;
      }
   }
   else if (zoom_command == ZOOM_OUT)
   {
      unsigned int increased_size = crop.rect.width + zoom_increment_16P16;
      if (increased_size < crop.rect.width) //overflow
      {
         crop.rect.width = 65536;
         crop.rect.height = 65536;
      }
      else
      {
         crop.rect.width = increased_size;
         crop.rect.height = increased_size;
      }
   }

   if (zoom_command == ZOOM_RESET)
   {
      crop.rect.x = 0;
      crop.rect.y = 0;
      crop.rect.width = 65536;
      crop.rect.height = 65536;
   }
   else
   {
      unsigned int centered_top_coordinate = (65536 - crop.rect.width) / 2;
      crop.rect.x = centered_top_coordinate;
      crop.rect.y = centered_top_coordinate;
   }

   int ret = mmal_status_to_int(mmal_port_parameter_set(camera->control, &crop.hdr));

   if (ret == 0)
   {
      roi->x = roi->y = (double)crop.rect.x/65536;
      roi->w = roi->h = (double)crop.rect.width/65536;
   }
   else
   {
      vcos_log_error("Failed to set crop values, x/y: %u, w/h: %u", crop.rect.x, crop.rect.width);
      ret = 1;
   }

   return ret;
}

/**
 * Adjust the exposure time used for images
 * @param camera Pointer to camera component
 * @param shutter speed in microseconds
 * @return 0 if successful, non-zero if any parameters out of range
 */
int raspicamcontrol_set_shutter_speed(MMAL_COMPONENT_T *camera, int speed)
{
   if (!camera)
      return 1;

   return mmal_status_to_int(mmal_port_parameter_set_uint32(camera->control, MMAL_PARAMETER_SHUTTER_SPEED, speed));
}

/**
 * Adjust the Dynamic range compression level
 * @param camera Pointer to camera component
 * @param strength Strength of DRC to apply
 *        MMAL_PARAMETER_DRC_STRENGTH_OFF
 *        MMAL_PARAMETER_DRC_STRENGTH_LOW
 *        MMAL_PARAMETER_DRC_STRENGTH_MEDIUM
 *        MMAL_PARAMETER_DRC_STRENGTH_HIGH
 *
 * @return 0 if successful, non-zero if any parameters out of range
 */
int raspicamcontrol_set_DRC(MMAL_COMPONENT_T *camera, MMAL_PARAMETER_DRC_STRENGTH_T strength)
{
   MMAL_PARAMETER_DRC_T drc = {{MMAL_PARAMETER_DYNAMIC_RANGE_COMPRESSION, sizeof(MMAL_PARAMETER_DRC_T)}, strength};

   if (!camera)
      return 1;

   return mmal_status_to_int(mmal_port_parameter_set(camera->control, &drc.hdr));
}

int raspicamcontrol_set_stats_pass(MMAL_COMPONENT_T *camera, int stats_pass)
{
   if (!camera)
      return 1;

   return mmal_status_to_int(mmal_port_parameter_set_boolean(camera->control, MMAL_PARAMETER_CAPTURE_STATS_PASS, stats_pass));
}

int raspicamcontrol_set_focus_window(MMAL_COMPONENT_T *camera, int focus_window)
{
   if (!camera)
      return 1;

   return mmal_status_to_int(mmal_port_parameter_set_boolean(camera->control, MMAL_PARAMETER_DRAW_BOX_FACES_AND_FOCUS, focus_window));
}

int raspicamcontrol_set_stereo_mode(MMAL_PORT_T *port, MMAL_PARAMETER_STEREOSCOPIC_MODE_T *stereo_mode)
{
   MMAL_PARAMETER_STEREOSCOPIC_MODE_T stereo = { {MMAL_PARAMETER_STEREOSCOPIC_MODE, sizeof(stereo)},
      MMAL_STEREOSCOPIC_MODE_NONE, MMAL_FALSE, MMAL_FALSE
   };
   if (stereo_mode->mode != MMAL_STEREOSCOPIC_MODE_NONE)
   {
      stereo.mode = stereo_mode->mode;
      stereo.decimate = stereo_mode->decimate;
      stereo.swap_eyes = stereo_mode->swap_eyes;
   }
   return mmal_status_to_int(mmal_port_parameter_set(port, &stereo.hdr));
}

int raspicamcontrol_set_gains(MMAL_COMPONENT_T *camera, float analog, float digital)
{
   MMAL_RATIONAL_T rational = {0,65536};
   MMAL_STATUS_T status;

   if (!camera)
      return 1;

   rational.num = (unsigned int)(analog * 65536);
   status = mmal_port_parameter_set_rational(camera->control, MMAL_PARAMETER_ANALOG_GAIN, rational);
   if (status != MMAL_SUCCESS)
      return mmal_status_to_int(status);

   rational.num = (unsigned int)(digital * 65536);
   status = mmal_port_parameter_set_rational(camera->control, MMAL_PARAMETER_DIGITAL_GAIN, rational);
   return mmal_status_to_int(status);
}

/**
 * Asked GPU how much memory it has allocated
 *
 * @return amount of memory in MB
 */
static int raspicamcontrol_get_mem_gpu(void)
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

/**
 * Check to see if camera is supported, and we have allocated enough memory
 * Ask GPU about its camera abilities
 * @param supported None-zero if software supports the camera
 * @param detected  None-zero if a camera has been detected
 */
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
