#ifndef NC_SDK_SHARED_TYPES_H
#define NC_SDK_SHARED_TYPES_H

#ifdef WINDOWS
	#include <stdint.h>
#else
	#include <stdlib.h>
	#include "pstdint.h"
#endif

#define OPENGL_ENABLE 0

#define NC_AUTO_DETECT 0x0000ffff
#define NC_AUTO_CHANNEL NC_AUTO_DETECT
#define NC_AUTO_UNIT	0x6fffffff
#define NC_FULL_WIDTH -1
#define NC_FULL_HEIGHT -1
#define NC_USE_MAC_ADRESS 0x20000000

// Type of amplifier set by the selected readout mode
/*! \ingroup group1  
 * @brief 	Values identifying possible readout paths 
 *  		that may be provided by a readout mode.
 *  
 *  For use with the functions ncCamGetAmpliTypeAvail() and 
 *  ncCamGetFreqAvail().  
 *************************************************************/
enum Ampli { 
	NOTHING = 0, 	//!< Generic unsupported value.
	EM, 			//!< Photo-electrons pass through an Electron Multiplication register before readout. 
	CONV 			//!< Photo-electrons pass directly to the readout register.
};

// The types of camera models
enum CameraType { UNKNOWN_TYPE, EMN2, HNU };

// Readout mode flags
enum NcReadoutModeFlags {
	NC_ROM_FLAG_NONE = 0x0,
	NC_ROM_FLAG_TDI = 0x1
};

// Type of communication interface that can be used
//  to access a camera controller.
/*! \ingroup group1  
 * @brief 	Values identifying the type of communication interface 
 *  		that can be used to operate a Nüvü Camēras device.
 *
 * This typically corresponds to a particular type/manufacturer of framegrabber 
 *  and support requires that an appropriate plugin be installed. 
 ***********************************************************************/
enum CommType { 
	INVALID =		0x0000fffe, /*!< Fallback value for error states. */
	AUTO_COMM =		0x6fff0000, /*!< Detect any plugin; equal to (NC_AUTO_UNIT - NC_AUTO_CHANNEL). */
	VIRTUAL =		0x7fff0000, /*!< Support provided by the nc_grab_virtual plugin. 
								     Allows the use of this SDK without having a camera available on your system. */
	EDT_CL =		0x00000000, /*!< Support provided by the nc_grab_edt plugin. 
								     For EDT framegrabbers. */
	PT_GIGE =		0x00010000, /*!< Support provided by the nc_grab_pleora plugin. 
								     For Pleora GigE Vision framegrabbers and IP Engines providing connectivity over Ethernet. */
	MIL_CL =		0x00020000, /*!< Support provided by the nc_grab_mil plugin. 
								     For Matrox framegrabbers. */
	SAPERA_CL =		0x00030000, /*!< Support provided by the nc_grab_sapera plugin. 
								     For Teledyne Dalsa framegrabbers.*/
	UNKNOWN =		0x00040000, /*!< Default for a user-implemented plugin. */
	REMOTE_COMM =	0x00050000, /*!< Not yet implemented.  To allow remote access over Internet. */
	NC_SERIAL_IO =	0x00060000  /*!< Support provided by the nc_grab_serial plugin. 
								    No images may be returned but the SDK can be used to configure the camera 
									 while a user's own, independent framegrabber implementation receives data. */
};

// Serial protocols that may be available
//  with differing requirements for the formatting of serial commands.
/*! \ingroup group3  
 * @brief 	Values identifying the serial protocol
 *  		that can be used to communicate with a Nüvü Camēras device.
 * 
 * The serial protocol will impact the required format of serial commands
 *  if the user intends to send them to the camera controller themselves
 *  (see ncGrabFormatSerialCommand() for more details).
 *************************************************************/
enum NcSerialProtocol {
	NC_SERIAL_LIVE = 0, 	//!< Use the serial protocol in effect
	NC_SERIAL_MEGANTIC, 	//!< Original serial protocol 
	NC_SERIAL_PASADENA  	//!< CRC32-based error-checking
};

// Reasons why a communication interface (CommType) 
//  or framegrabber tap may be reported as unused
/*! \ingroup group1  
 * Values identifying possible reasons that a 
 *  communication interface (#CommType) 
 *  might not provide access to a Nüvü Camēras device 
 *  when this SDK scans your system for an available device. 
 *************************************************************/
enum NcPortUnusedReason {
	NC_PORT_UNUSED_NO_DEVICE = 0,	//!< The port was found to not be connected to a Nuvu device 
	NC_PORT_UNUSED_ALREADY_IN_USE,	//!< The device is already in use by another instance 
	NC_PORT_UNUSED_FILTERED,		//!< Port was not scanned (filtered, internal use only) 
	NC_PORT_CLMODE_UNAVAILABLE, 	//!< The CameraLink mode required by the device is not available in the current configuration 
	NC_PORT_CLMODE_SUPPORT, 		//!< The CameraLink mode required by the device is not supported by the plugin used to access it 
	NC_PORT_PLUGIN_INCOMPATIBLE, 	//!< The plugin used to access the device was incompatible with some of its requirements 
	NC_PORT_UNUSED_UNKNOWN			//!< The reason was unusual and has no unique identifier 
};

// We define the CameraLink mode as the maximum width in bits 
//  that may be sent in a single pixel clock.
enum CameraLinkMode {
	DEFAULT_MODE = 	 0, // 16-bit pixels, single tap
	BASE = 			24,
	MEDIUM = 		48,
	FULL = 			64,
	FULL_EX = 		72,
	EXTENDED = 		80
};

// Detector product numbers or models that may be available. 
enum DetectorType { 
	CCD60 = 0,  	// e2v
	CCD97,   		// e2v
	CCD201_20,  	// e2v
	CCD207_00,  	// e2v
	CCD207_10,  	// e2v
	CCD207_40,  	// e2v
	CCD220,   		// e2v
	S10200_02_01, 	// Hamamatsu
    S10202_16_01, 	// Hamamatsu
	UNKNOWN_CCD };

// Camera features / operational parameters that may be available.
/*! \ingroup group2  
 * @brief 	Values identifying features of the camera that may be available.
 * 
 * Availability is reported by the functions ncCamParamAvailable() or ncGrabParamAvailable() 
 *  and may ultimately depend upon the specific readout mode loaded, 
 *  the specific model or options included. 
 * The availability of many of these features is associated with the 
 *  use of particular API functions: 
 *  where appropriate, such functions are listed below with the enum values.
 *
 * @note 	Certain Features may be available in principle but their access
 * 			restricted according to other operational parameters: 
 *  		e.g. #CALIBRATED_EM_GAIN will only be available for readout modes 
 *  		having the parameter #Ampli with value #EM 
 *  		(see ncCamGetAmpliTypeAvail() and ncCamGetReadoutMode()), 
 *  		but in addition can only be used when the sensor temperature range is within
 *  		that given by ncCamGetCalibratedEmGainTempRange().  
 *************************************************************/
enum Features {
	UNKNOWN_FEATURE = 0, 		//!< Generic unsupported value.
	EXPOSURE, 					//!< Control of exposure time; use of ncCamSetExposureTime().
	WAITING_TIME, 				//!< Control of blanking/waiting time; use of ncCamSetWaitingTime().
	TRIGGER_MODE, 				//!< Also specify a particular #TriggerMode as the setting, or use directly one of the \link TRIGGER_MODE_ TRIGGER_MODE_* \endlink values below; use of ncCamSetTriggerMode().  TDI and \ref FKM readout can affect #TriggerMode availability.
	SHUTTER_MODE, 				//!< Also specify a particular #ShutterMode as the setting; use of ncCamSetShutterMode().  \ref FKM readout can affect #ShutterMode availability.
	SHUTTER_POLARITY, 			//!< Control of SHUTTER external signal polarity; use of ncCamSetShutterPolarity().
	EXTERNAL_SHUTTER, 			//!< Independent control of an external shutter via the SHUTTER external signal; use of ncCamSetExternalShutter().
	EXTERNAL_SHUTTER_MODE, 		//!< Independent control of the external shutter mode; use of ncCamSetExternalShutterMode().
	EXTERNAL_SHUTTER_DELAY, 	//!< Independent control of the external shutter signal delay relative to the start of exposure (FIRE); use of ncCamSetExternalShutterDelay().
	FIRE_POLARITY, 				//!< Control of FIRE external signal polarity; use of ncCamSetFirePolarity().
	MINIMUM_PULSE_WIDTH, 		//!< Control over the minimum pulse width of external signals output by the camera (ARM, SHUTTER, FIRE); use of ncCamSetOutputMinimumPulseWidth().
	ARM_POLARITY, 				//!< Control of ARM external signal polarity; use of ncCamSetArmPolarity().
	CALIBRATED_EM_GAIN, 		//!< Calibrated Electron Multiplication gain; use of ncCamSetCalibratedEmGain().  Also depends upon sensor temperature (see ncCamGetCalibratedEmGainTempRange()).
	RAW_EM_GAIN, 				//!< Direct control of the voltage bias controlling Electron Multiplication; use of ncCamSetRawEmGain(), however #CALIBRATED_EM_GAIN is preferred, if available.
	ANALOG_GAIN, 				//!< Control of analog gain (scaling of dynamic range); use of ncCamSetAnalogGain().
	ANALOG_OFFSET, 				//!< Control of analog offset (baseline level of raw images); use of ncCamSetAnalogOffset().
	TARGET_DETECTOR_TEMP, 		//!< Control of sensor temperature set-point; use of ncCamSetTargetDetectorTemp().
	DETECTOR_TEMP, 				//!< Access to current sensor temperature; use of ncCamGetDetectorTemp().
	CTRL_TIMESTAMP, 			//!< Control of returning timestamped images; use of ncCamSetTimestampMode().
	GPS_PRESENT, 				//!< Detection of an external GPS clock.
	GPS_LOCKED, 				//!< Access to GPS synchronisation of image timestamps; use of ncCamSetTimestampMode() with TimestampMode #GPS_TIMESTAMP.
	BINNING_X, 					//!< Also specify a particular horizontal binning setting (1, 2, 4, 8 or 16); use of ncCamSetBinningMode() with the appropriate setting.  Particular #ROI configurations may limit availability.
	BINNING_Y, 					//!< A specific vertical binning setting may be provided or 0 to query general availability; use of ncCamSetBinningMode().  Particular #ROI configurations may limit availability.
	ROI, 						//!< Control over deliberate restriction of the readout region of the sensor; use of ncCamSetMRoiSize() and/or ncCamSetMRoiPosition(), etc. with index 0.  
	CROP_MODE_X, 				//!< Selection of horizontally-compressed, accelerated readout; use of ncCamSetCropMode() with CropMode #CROP_MODE_ENABLE_X.
	CROP_MODE_Y, 				//!< Selection of vertically-compressed, accelerated readout; use of ncCamSetCropMode() with CropMode #CROP_MODE_ENABLE_Y.
	ACTIVE_REGION, 				//!< Default readout and ROI definitions are contained within the illuminated portion of the sensor.
	MULTIPLE_ROI, 				//!< Control over multiple Regions Of Interest simultaneously; use of ncCamSetMRoiSize() and/or ncCamSetMRoiPosition(), etc. with multiple indices.  
	TRIGGER_MODE_INTERNAL, 		//!<  @anchor TRIGGER_MODE_ Selection of internally triggered readout; use of ncCamSetTriggerMode() with #TriggerMode #INTERNAL.
	TRIGGER_MODE_EXTERNAL, 		//!< Selection of externally triggered readout; use of ncCamSetTriggerMode() with #TriggerMode #EXT_LOW_HIGH or #EXT_HIGH_LOW.
	TRIGGER_MODE_EXPOSURE, 		//!< Selection of externally triggered readout, with exposure controlled by trigger signal width; use of ncCamSetTriggerMode() with #TriggerMode #EXT_LOW_HIGH_EXP or #EXT_HIGH_LOW_EXP
	TRIGGER_MODE_CONTINUOUS, 	//!< Selection of externally triggered readout, with no blanking/waiting between exposures; use of ncCamSetTriggerMode() with #TriggerMode #CONT_LOW_HIGH or #CONT_HIGH_LOW
	SYSTEM_STATUS, 				//!< Access to system status polling callbacks; use of ncCamSetOnStatusUpdateCallback() and ncCamSetOnStatusAlertCallback().
	SHORT_SSVA_CMD, 			//!< Internal.
	BIN_CDS, 					//!< Internal.
	SEQUENCE_REGISTERS, 		//!< Internal.
	TDI_HEIGHT_CTRL, 			//!< TDI readout has configurable frame height (via #ROI).
	NC_PROC_SUPPORT_RAW, 		//!< Selection of no image-processing (i.e. raw images); use of ncCamSetProcType() or ncGrabSetProcType() with ProcType #NO_PROC.
	NC_PROC_SUPPORT_LM, 		//!< Selection of bias subtraction image-processing; use of ncCamSetProcType() or ncGrabSetProcType() with ProcType #LM.  Requires a compatible readout mode having overscan (see ncCamGetOverscanLines()).
	NC_PROC_SUPPORT_PC, 		//!< Selection of photon-counting image-processing; use of ncCamSetProcType() or ncGrabSetProcType() with ProcType #PC.  Requires a compatible readout mode having overscan (see ncCamGetOverscanLines()).
    FAST_KINETICS_MODE 			//!< Selection of Fast Kinetics Mode readout; use of ncCamSetFastKineticsMode().  Requires a compatible readout mode.
};

// For backwards compatibility
typedef enum Features Param;

// List of different sub-components for which 
//  temperatures may be available.
/*! \ingroup group2  
 * 	@brief 	Values identifying camera sub-components having a monitored temperature. 
 *
 *  For use with the function ncCamGetComponentTemp().  
 *************************************************************/
enum NcTemperatureType {
	NC_TEMP_CCD = 0, 					//!< Sensor temperature
	NC_TEMP_CONTROLLER, 				//!< Camera controller electronics board temperature
	NC_TEMP_POWER_SUPPLY, 				//!< Internal power conversion/supply board
	NC_TEMP_FPGA, 						//!< FPGA die temperature
	NC_TEMP_HEATINK, 					//!< Legacy typo (see #NC_TEMP_HEATSINK)
	NC_TEMP_HEATSINK = NC_TEMP_HEATINK 	//!< Sensor TEC heatsink (if applicable).  Alias for #NC_TEMP_HEATINK 
};

// Type of shutter mode available
/*! \ingroup group2  
 * 	@brief 	Values identifying possible shutter mode settings. 
 *
 *  For use with the functions ncCamParamAvailable() and 
 *  ncCamSetShutterMode().  
 *************************************************************/
enum ShutterMode { 
	SHUT_NOT_SET = 0, 		//!< Generic unsupported value.
	OPEN = 1, 				//!< Shutter open regardless of acquisition state. 
	CLOSE = 2, 				//!< Shutter closed regardless of acquisition state.
	AUTO = 3, 				//!< Shutter opens shortly before each exposure (FIRE); implies a default blanking/waiting time.
	BIAS_DEFAULT = CLOSE 	//!< Shutter will close to ensure dark images for bias acquisition. 
};

// Type of trigger mode available
/*! \ingroup group2  
 *  @brief 	Values identifying possible trigger mode settings.
 *
 *  For use with the functions ncCamParamAvailable() and 
 *  ncCamSetTriggerMode().  
 *************************************************************/
enum TriggerMode { 
	CONT_HIGH_LOW = -3, 	//!< Continuous acquisition (no blanking/waiting between triggered exposures); exposure start synchronised on high-to-low edge.
	EXT_HIGH_LOW_EXP = -2, 	//!< Exposure controlled by trigger signal width; exposure start synchronised on high-to-low edge, fall to blanking/waiting  between triggers.
	EXT_HIGH_LOW = -1, 		//!< Exposure controlled by set exposure; exposure start synchronised on high-to-low edge, fall to blanking/waiting  between triggers..
	INTERNAL = 0, 			//!< Exposure controlled by set exposure, blanking/waiting controlled by set waiting time; exposure start controlled internally. 
	EXT_LOW_HIGH = 1, 		//!< Exposure controlled by set exposure; exposure start synchronised on low-to-high edge.
	EXT_LOW_HIGH_EXP = 2, 	//!< Exposure controlled by trigger signal width; exposure start synchronised on low-to-high edge, fall to blanking/waiting  between triggers..
	CONT_LOW_HIGH = 3  		//!< Continuous acquisition (no blanking/waiting between triggered exposures); exposure start synchronised on low-to-high edge, fall to blanking/waiting  between triggers..
};

// Type of crop-mode available
/*! \ingroup group2  
 *  @brief 	Values identifying possible crop-mode settings.
 *
 *  For use with the functions ncCamParamAvailable() and 
 *  ncCamSetCropMode().  
 *************************************************************/
enum CropMode { 
	CROP_MODE_DISABLE = 0x00,  			//!< Normal ROI.
	CROP_MODE_ENABLE_X = 0x01,  		//!< Horizontally-compressed, accelerated readout.
	CROP_MODE_ENABLE_Y = 0x02,  		//!< Vertically-compressed, accelerated readout.
	CROP_MODE_ENABLE_XY = CROP_MODE_ENABLE_X | CROP_MODE_ENABLE_Y, 	//!< Accelerated readout, both horizontally- and vertically-compressed.
	CROP_MODE_ENABLE_ZL = CROP_MODE_ENABLE_X, //!< Zero Latency: no frame latency.  Alias for #CROP_MODE_ENABLE_X (cf. #CROP_MODE_ENABLE_Y, see ncCamGetFrameLatency()).
	CROP_MODE_ENABLE_SP = CROP_MODE_ENABLE_Y, //!< Spectroscopy: frame transfer shifts perpendicular to slit length.  Alias for #CROP_MODE_ENABLE_Y. 
	CROP_MODE_ENABLE_MX = CROP_MODE_ENABLE_XY //!< Maximum frame rate.  Alias for #CROP_MODE_ENABLE_XY.
};

enum ExtShutter { EXTERNAL_SHUTTER_DISABLE = 0, EXTERNAL_SHUTTER_ENABLE = 1 };

enum ExtPolarity { NEGATIVE = -1, POL_NOT_SET = 0, POSITIVE = 1 };

// Type of image format which will be used to save an image
enum ImageFormat { UNKNOWNN = -1, TIF = 0, FITS };

// Use only CFITSIO DATATYPE codes
enum ImageDataType { 
	NC_IMG = 0,		// uint 16 bit 
	NC_IMG_UINT,	// uint 32 bit
	NC_IMG_ULONG,	// uint 64 bit
	NC_IMG_FLOAT  	// 32 bit float
};

enum HeaderDataType { NC_INT = 0, NC_DOUBLE, NC_STRING };

enum ImageCompression { NO_COMPRESSION = 0, GZIP };

// This enum is for legacy compatibility
/*! \ingroup camAcq  
 * @brief 	Values identifying some available image processing mode settings.
 *  
 *  This enum is for legacy compatibility, 
 *  however the values are suitable
 *  for use with the functions ncCamSetProcType() and 
 *  ncGrabSetProcType().  
 *
 *************************************************************/
enum ProcType { 
	NO_PROC = 0x00, //!< No processing; raw images
	LM = 0x01, 		//!< Linear or analogue mode; bias subtraction of images to a flat consistent baseline (bias clamp)
	PC = 0x02  		//!< Photon-counting mode; bias subtraction and thresholding to eliminate ENF (requires high EM-gain and low light levels).
};

// Modes available for the timestamp appended to images by the camera controller.
/*! \ingroup group2  
 * @brief 	Values identifying possible camera controller timestamp mode settings.
 *
 *  For use with the functions ncCamParamAvailable() and 
 *  ncCamSetTimestampMode().  
 *************************************************************/
enum TimestampMode { 
	NO_TIMESTAMP = 0, 		//!< No timestamp returned by the camera controller
	INTERNAL_TIMESTAMP = 1, //!< The camera controller appends a timestamp synchronised to it's on-board clock
	GPS_TIMESTAMP = 2 		//!< The camera controller appends a timestamp synchronised to an external GPS clock.
};

// List of versioned system sub-components
enum VersionType { SOFTWARE = 0, FIRMWARE, FPGA, HARDWARE };

// List of special driver operating modes
/*! \ingroup camAcq
 * @brief 	Values identify available special operating modes.
 *
 *  For use with the ncCamSetSpecialMode() function.
 *
 *************************************************************/
enum NcSpecialMode
{
	NC_SPECIAL_MODE_NORMAL,			//!< Normal operation mode, images are received and processed as normal
	NC_SPECIAL_MODE_SERIAL_ONLY		//!< Serial communication only, custom restrictions are in effect, no images are processed
};

// Argument struture to pass to ncCamSetSpecialMode
// The correct union field to use repends on the NcSpecialMode enum value provided
// For NcSpecialMode enum values with no associated union field, no arguments are required
// An uninitialized instance or NULL can be provided
union NcSpecialModeArguments
{
	// For use with NC_SPECIAL_MODE_SERIAL_ONLY
	struct
	{
		enum NcSpecialMode ModeEnum;	// Must be set to `NC_SPECIAL_MODE_SERIAL_ONLY`

		// Defines the buffer size restrictions required by the receiving device
		// All values may be set to 1 if the images are not received by any device
		int SizeRestrictionPixel;
		int SizeRestrictionWidth;
		int SizeRestrictionHeight;
	} NcSerialOnlyArgs;
};

// Deprecated TIFF header tags
#define  AMPLI_TYPE_TAG							650
#define  HORIZ_FREQ_TAG							651
#define  VERT_FREQ_TAG							652
#define  EXPOSURE_TIME_TAG						653
#define  EFFECTIVE_EXP_TIME_TAG					682
#define  WAITING_TIME_TAG						654
#define  RAW_EM_GAIN_TAG						655
#define  CAL_EM_GAIN_TAG						680
#define  ANALOG_GAIN_TAG						656
#define  ANALOG_OFFSET_TAG						657
#define  TARGET_DETECTOR_TEMP_TAG				658
#define  BINNING_X_TAG							659
#define  BINNING_Y_TAG							660
#define  ROI_X1_TAG								661
#define  ROI_X2_TAG								662
#define  ROI_Y1_TAG								663
#define  ROI_Y2_TAG								664
#define  CROP_MODE_ENABLE_TAG					695
#define  CROP_MODE_PADDING_X_TAG				696
#define  CROP_MODE_PADDING_Y_TAG				697
#define  SHUTTER_MODE_TAG						665
#define  TRIGGER_MODE_TAG						666
#define  CLAMP_LEVEL_TAG						667
#define  PROC_TYPE_TAG							676
#define  NBR_PC_IMAGES_TAG						675
#define  SERIAL_NUMBER_TAG						668
#define  FIRM_VERSION_TAG						669
#define  HARDW_VERSION_TAG						670
#define  FPGA_VERSION_TAG						671
#define  SOFT_VERSION_TAG						672
#define  ADD_COMMENTS_TAG						674
#define  OVERSCAN_LINES_TAG						677
#define  OVERSAMPLE_X_TAG						678
#define  DETECTOR_TYPE_TAG						681
#define  FIRE_POLARITY_TAG						688
#define  SHUTTER_POLARITY_TAG					683
#define  ARM_POLARITY_TAG						689
#define  OUTPUT_PULSE_WIDTH_TAG					687
#define  EXT_SHUTTER_PRES_TAG					684
#define  EXT_SHUTTER_MODE_TAG					685
#define  EXT_SHUTTER_DELAY_TAG					686
#define  OBS_DATE_TAG							690
#define  OBS_TIME_SEC_FRACT_TAG					691
#define  OBS_FLAGS_TAG							692
#define  DATE_TIME_US_TAG						694
#define  HOST_TIME_TAG							693
#define  NBR_IMAGES_TRIG_TAG					679
#define  SEQUENCE_NAME_TAG						673


typedef const unsigned short NcImage;
typedef void(*NcCallbackFunc)(void*);

typedef struct _NcStatsCtx NcStatsCtx;

typedef struct _NcDevice* NcDevice;


#ifdef WINDOWS
	#ifdef NC_DRIVER_EXPORTS  	
		#define NC_FUNCTION __declspec(dllexport)
	#else
		#define NC_FUNCTION __declspec(dllimport)
	#endif
#else
	#include <errno.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <unistd.h>
	#include <pthread.h>
	#include <string.h>
	#define NC_FUNCTION // __attribute__ ((visibility ("default")))  // TODO: Try this definition to allow building with -fvisibility=hidden for a more efficient dynamic library; could mess up nc_status_log compatibility ... ?
#endif

// Common data type enumeration
enum NcSdkDataTypes 
{
	NcSdkDataTypeInt = 0,
	NcSdkDataTypeDouble,
	NcSdkDataTypeString,
};	// enum NcSdkDataTypes

#endif	// NC_SDK_SHARED_TYPES_H
