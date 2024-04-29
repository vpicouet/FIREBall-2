#ifndef NC_ERROR_CODE_H
#define NC_ERROR_CODE_H

#define NC_SUCCESS							0
#define NC_GENERIC_ERROR					1

	//11 to 40 are general errors
#define NC_ERROR_MEM_ALLOC                  11		//Memory allocation error, please make sure your system have sufficient RAM to operate your application
#define NC_ERROR_MUTEX_LOCK					12
#define NC_ERROR_MUTEX_RELEASE              14		//An internal mutex can't be released, please review the concurrent threads of your application
#define NC_ERROR_CREATE_THREAD              15		//An internal thread can't be properly created
#define NC_ERROR_RELEASE_THREAD             16
#define NC_ERROR_WAIT_SIGNAL				17
#define NC_ERROR_BAD_CMD_LINE_PARAMS		18

#define NC_ERROR_DATA_TYPE                  25		//The data type passed for this function is invalid
#define NC_ERROR_IMAGE_HEADER_FORMAT        26		//The image format doesn't support additional headers
#define NC_ERROR_CAMERA_FOUND				27		//No camera has been found using the automatic parameters passed
#define NC_ERROR_NO_GRABBER					28

	// 41 to 50 are errors related to standard library exceptions
#define NC_ERROR_UNKNOWN					41
#define NC_ERROR_STD_ERROR					42		// std::exception
#define NC_ERROR_LOGIC_ERROR				43		// std::logic_error exception
#define NC_ERROR_RUNTIME_ERROR				44		// std::runtime_error exception
#define NC_ERROR_BAD_CAST					45		// std::bad_cast exception
#define NC_ERROR_BAD_FUNC_POINTER			46		// std::bad_function_call
#define NC_ERROR_BAD_TYPE_ID				47		// std::bad_typeid
#define NC_ERROR_BAD_WEAK_PTR				48		// std::bad_weak_ptr
#define NC_ERROR_BAD_EXCEPTION				49		// std::bad_exception
#define NC_ERROR_STD_SYS_ERROR				50		// std::system_error

	//51 to 75 are related to communication error between the camera and the host system
#define NC_ERROR_COM_CMD_RET                51		//The camera returned an error to the command sent
#define NC_ERROR_COM_CMD_ANSW               52		//The camera never finished answering the command sent
#define NC_ERROR_COM_CSUM_REC               53		//The checksum calculated for the answer received doesn't match the one sent by the camera
#define NC_ERROR_COM_CSUM_SENT              54		//The camera returned an error to the checksum sent
#define NC_ERROR_COM_CSUM_ANSW              55		//The camera never finished answering the checksum sent
#define NC_ERROR_COM_ABORT_STATE            56		//The camera status command didn't came back in the idle state as it should after an abort
#define NC_ERROR_COM_UNEXPECTED				57		//Serial communication with the controller diverged from the established protocol

	//76 to 98 are related to parsing errors in the information sent from the camera
#define NC_ERROR_CAM_PARSE_INIT             76		//One of the passed parameters hasn't been properly initialized
#define NC_ERROR_CAM_PARSE_FOUND            77		//The information expected hasn't been found
#define NC_ERROR_CAM_PARSE_DEFINE           78		//The internal define function hasn't been able to run as expected

#define NC_ERROR_NO_MENS_LAND               99		//You're in an unexpected case, if you can reproduce this issue, contact Nuvu Cameras

	//101 to 200 are related to the camera layer errors
#define NC_ERROR_CAM_STRUCT_PTR             101		//The structure pointer (NcCam) passed to the function is invalid
#define NC_ERROR_CAM_PARAM_INIT             102		//One of the required parameters hasn't been properly initialized
#define NC_ERROR_CAM_PARAM_OUT              103		//One of the parameter values is invalid because it is out of bounds
#define NC_ERROR_CAM_ASCII_REC              104		//Even though the serial answer received seems fine, a typo error from the controller seems to be present
#define NC_ERROR_CAM_FIRMWARE_VERSION       105		//The firmware version of the controller isn't compatible with the feature you're trying to use
#define NC_ERROR_CAM_FIRMWARE_DETECTION     106		//The firmware version can't be read properly
#define NC_ERROR_CAM_NO_FEATURE             107		//The feature you're looking to use isn't supported by this camera
#define NC_ERROR_CAM_NO_READOUT	/* Deprecated*/        108		//The readout mode loaded isn't a valid one
#define NC_ERROR_CAM_EM_READOUT             109		//This function can only be executed while using an EM readout mode, which isn't the case
#define NC_ERROR_CAM_SAVE_OPEN              110		//The file you're trying to create to save the image can't be put in place
#define NC_ERROR_CAM_SAVE_FORMAT            111		//The save format specified isn't valid or supported for this data type
#define NC_ERROR_CAM_SAVE_OVERWRITE         112		//There's already an image file with this name, please select a new name or modify the overwrite flag
#define NC_ERROR_CAM_SAVE_DEPTH             113		//The data type specified can't be saved with the appropriate depth
#define NC_ERROR_CAM_INTERNAL_REG           114		//Some internal camera registers hasn't properly be initialized
#define NC_ERROR_CAM_PARAM_OPEN             115		//The camera parameter file specified can't be opened
#define NC_ERROR_CAM_PARAM_READ             116		//The camera parameter file can't be read properly, please make sure it's not write protected and you have the rights to access this file
#define NC_ERROR_CAM_PARAM_POS              117		//The camera parameter file size can't be retrieved, make sure it's not empty
#define NC_ERROR_CAM_PARAM_VALUE            118		//The buffer size in which the camera parameter file information are being copied isn't the appropriate one
#define NC_ERROR_CAM_PARAM_READOUT          119		//There's no readout mode matching the one you used when saving your parameters
#define NC_ERROR_CAM_PARAM_MISS             120		//Some required camera parameters can't be found in the camera parameters file
#define NC_ERROR_CAM_PARAM_OVERWRITE        121		//There's already a camera parameter file existing with the same name, please select a new name or modify the overwrite flag
#define NC_ERROR_CAM_EMPTY_ANSWER           122
#define NC_ERROR_CAM_FEATURE_DISABLED       123		//The current mode of operation means that this function can't be used (e.g. Crop mode prohibits MoveRoi)
#define NC_ERROR_CAM_ROI_OVERLAP			124		//Can't apply the current ROIs because two or more ROIs overlap
#define NC_ERROR_CAM_TOO_MANY_ROIS			125		//Trying to add an ROI, but the maximum number of supported ROIs has been reached
#define NC_ERROR_CAM_BAD_ROI_INDEX			126		//Trying to access an ROI, but the index that was used is invalid
#define NC_ERROR_CAM_ROI_OUT_OF_BOUNDS		127		//An ROI's definition exceeds the available dimensions of the sensor
#define NC_ERROR_CAM_SAVE_WRITE_DATA		128		//An error occured while writing pixel values to an image file
#define NC_ERROR_CAM_SAVE_WRITE_META		129		//An error occured while writing metadata to an image file
#define NC_ERROR_CAM_DEPRECATED				130		//This function is deprecated and this use of it is not supported
#define NC_ERROR_CAM_NONSTOP				131		//The acquisition in progress is required to not halt but the change requested would require it
#define NC_ERROR_CAM_NO_READOUT_MODE		132		//No readout sequence has been loaded yet
#define NC_ERROR_CAM_READOUT_MODE_INVALID	133		//The selected readout mode does not conform to the requirements of NcCam
#define NC_ERROR_CAM_ROI_INVLAID			134		//The requested ROI cannot be returned
#define NC_ERROR_CAM_CROPMODE_ROI_INVLAID	135		//The requested ROI cannot be returned with this crop-mode
#define NC_ERROR_CAM_NOT_PREPARED			136		//The user has asked the acquisition to begin but has not prepared the framegrabber
#define NC_ERROR_CAM_NO_ROIS                137		//Trying to apply ROIs, but none are defined; there must always be one or more ROIs defined.
#define NC_ERROR_CAM_NO_READOUT_AVAILABLE	138		//No readout sequence has been programmed onto this camera controller
#define NC_ERROR_CAM_BAD_BUFFER_PTR         139     //A user provided buffer pointer was not allocated with an SDK function
#define NC_ERROR_CAM_BUFFER_SIZE_ERROR      140     //A user provided buffer's size does not match the image size, the configuration may have changed since it was allocated

	//201 to 300 are related to the grab layer errors
#define NC_ERROR_GRAB_STRUCT_PTR            201		//The structure pointer (NcGrab) passed to the function is invalid
#define NC_ERROR_GRAB_PARAM_INIT            202		//One of the required parameter hasn't been properly initialized
#define NC_ERROR_GRAB_PARAM_OUT             203		//One of the parameter value is out of bound and can't be used
#define NC_ERROR_GRAB_REG_ACCESS            204		//The nc registry keys can't be accessed.  Please make sure that the installation of this software does not report an error and that the permissions of your current session let you access the registry of your system.
#define NC_ERROR_GRAB_REG_MISS              205		//Some of the mandatory nc registry keys haven't been setup properly.  Please contact Nuvu Cameras Technical support to remedy this issue.
#define NC_ERROR_GRAB_COMM_INTER            206		//Unknown communication interface passed to the Open function thru the Unit parameter
#define NC_ERROR_GRAB_CONFIG_OPEN           207		//The Nuvu Cameras configuration file can't be opened, please make sure it's in the Nuvu Cameras path
#define NC_ERROR_GRAB_CONFIG_READ           208		//The Nuvu Cameras configuration file can't be read properly, please make sure it's not right protected and you have the rights to access this file
#define NC_ERROR_GRAB_CONFIG_POS            209		//The Nuvu Cameras configuration file size can't be retrieved, make sure it's not empty
#define NC_ERROR_GRAB_CONFIG_VALUE          210		//The buffer size in which the Nuvu Cameras configuration file information are being copied isn't the appropriate one
#define NC_ERROR_GRAB_LOG_OPEN              211		//The log file file can't be opened, please make sure "Log" folder exist in the Nuvu Cameras path
#define NC_ERROR_GRAB_SAVE_FORMAT           212		//The save format specified isn't valid or supported
#define NC_ERROR_GRAB_COMP_FORMAT           213		//The compression format specified isn't valid or supported
#define NC_ERROR_GRAB_TIMEOUT               214		//The image didn't arrived prior to the timeout value set
#define NC_ERROR_GRAB_NO_IMAGE              215		//There's no image already available, so a null pointer is being returned
#define NC_ERROR_GRAB_NOT_STOP              216		//Acquisitions are still in progress even though they should be stopped
#define NC_ERROR_GRAB_CLOSE_ACQUISITION		217
#define NC_ERROR_GRAB_SKIP_IMAGE			218
#define NC_ERROR_GRAB_BIAS_SIZE				219
#define NC_ERROR_GRAB_DEBUG_LEVEL			220
#define NC_ERROR_GRAB_PROC_TIMEOUT			221
#define NC_ERROR_GRAB_BIAS_CANCEL  			222 
#define NC_ERROR_GRAB_SUPPORT_MAC           223
#define NC_ERROR_GRAB_FIRMWARE_VERSION      224
#define NC_ERROR_GRAB_NO_FEATURE            225		//The feature you're looking to use isn't supported by this controller
#define NC_ERROR_GRAB_NO_GPS                226
#define NC_ERROR_GRAB_TIMESTAMP_SKIP        227 	//The image width is too small to accomodate the timestamp metadata with the current timestamp skip setting 
#define NC_ERROR_GRAB_GPS_UNLOCK            228
#define NC_ERROR_GRAB_NO_TIMESTAMP          229 	//A timestamp was requested from an image that did not have any associated timestamp information
#define NC_ERROR_GRAB_UNMATCH_IMAGE         230
#define NC_ERROR_GRAB_BUFFER_TOO_SMALL		231
#define NC_ERROR_ALREADY_OPENED				232
#define NC_ERROR_GRAB_CONFIRM_FAILED		233
#define NC_ERROR_GRAB_GPS_INVALID           234 	// Timestamp information received from the GPS unit is not valid: it must provide the NEMA GPGGA, GPRMC and GPZDA sentences, once per second.
#define NC_ERROR_GRAB_WIDTH_RESTRICTION		235		// The specified image width does not respect a line-length restriction, e.g. requiring a multiple of the number of readout taps
#define NC_ERROR_GRAB_TIMESTAMP_SYNCT_FAIL	236		// Failed to synchronize the controller timestamp with the system timestamp
#define NC_ERROR_GRAB_IMAGE_INVALIDATED		237		// The image was successfully acquired but was invalidated during processing due to an unexpected error
#define NC_ERROR_GRAB_CLINK_MODE_SUPPORT	238		// The controller requires a cameralink mode that the framegrabber hardware or plugin does not support
#define NC_ERROR_GRAB_PIXEL_DEPTH_SUPPORT	239		// The framegrabber plugin used does not support the specified pixel depth
#define NC_ERROR_GRAB_SIZE_RESTRICTION		240		// The specified image size does not respect the frame grabber's restrictions
#define NC_ERROR_GRAB_METADATA_UNAVAILABLE  241 	//The timestamp metadata cannot be returned in this readout configuration
#define NC_ERROR_GRAB_CLINK_MODE_UNAVAILABLE 242	// The controller requires a cameralink mode that the specified unit and channel cannot provide: the cables may need to be swapped
#define NC_ERROR_GRAB_PIXEL_DEPTH_UNAVAILABLE 243	// The controller requires a pixel depth that the framegrabber plugin cannot support for the current CameraLink mode 
#define NC_ERROR_GRAB_NOT_ACQ               244		// Acquisitions are not in progress even though they should be.
#define NC_ERROR_GRAB_CLINK_TAPS_SUPPORT    245		// The framegrabber plugin used does not support the specified number of readout taps
#define NC_ERROR_GRAB_REGION_OUT_OF_BOUNDS	246		// The current region's definition exceeds the available dimensions of the returned frame

	//301 to 400 are related to the processing layer errors
#define NC_ERROR_PROC_STRUCT_PTR            301		//The structure pointer (NcProcCtx) passed to the function is invalid
#define NC_ERROR_PROC_PARAM_INI             302		//One of the required parameter hasn't been properly initialized
#define NC_ERROR_PROC_PARAM_OUT             303		//One of the parameter value is out of bound and can't be used
#define NC_ERROR_PROC_NO_TYPE               304		//The processing type isn't valid or supported
#define NC_ERROR_PROC_NO_BIAS               305		//There's no image stored to calculate the Bias
#define NC_ERROR_PROC_SAVE_OPEN             306		//The bias file can't be correctly open
#define NC_ERROR_PROC_BIAS_LOAD             307		//One the bias parameter can't be loaded correctly
#define NC_ERROR_PROC_BIAS_SAVE             308		//There's no Bias image available to save
#define NC_ERROR_PROC_SAVE_OVERWRITE        309		//There's already a bias file with this name, please select a new name or modify the overwrite flag
#define NC_ERROR_PROC_LOAD_SN               310
#define NC_ERROR_PROC_LOAD_READOUT          311
#define NC_ERROR_PROC_LOAD_AMPLI            312
#define NC_ERROR_PROC_LOAD_VERT             313
#define NC_ERROR_PROC_LOAD_HORIZ            314
#define NC_ERROR_PROC_IMAGE_CONTEXT_SIZE    315     //The number of images the context can contain is 3 or 10
#define NC_ERROR_PROC_RETRIEVE_OUT          316     //The number of the image to be retrieved is larger than the maximum number of images that the context stores
#define NC_ERROR_PROC_RETRIEVE_LARGE        317     //The number of the image to be retrieved is larger than the number of images in the context
#define NC_ERROR_PROC_SIZE_COSMICRAY        318     //The number of images in the context is not large enough for removing cosmic rays. Please add more images
#define NC_ERROR_PROC_NO_CONTEXT_SIZE       319     //It is not possible to add / get images to / from the context since it is set to not store images
#define NC_ERROR_PROC_NO_RECENT_IMAGE       320     //No more recent images to retrieve
#define NC_ERROR_PROC_SIZE_PC               321     //The number of images in the context is not large enough to obtain a PC image. Please add more images
#define NC_ERROR_PROC_BAD_MODE				322		//The current processing mode is invalid for the requested operation
#define NC_ERROR_PROC_NOT_SUPPORTED			323		//The function is no longer supported
#define NC_ERROR_PROC_PIPELINE_ERROR		324		//The processing pipeline could not be constructed

	//401 to 500 are related to the EDT driver errors
#define NC_ERROR_EDT_DEP_STRUCT             401		//The internal dependent structure that EDT generates to initialize the frame grabber hasn't been created properly
#define NC_ERROR_EDT_INIT                   402		//The initialization of the frame grabber failed
#define NC_ERROR_EDT_READ_CONFIG            403		//The EDT initialization file hasn't been read properly
#define NC_ERROR_EDT_OPEN_STRUCT            404		//The EDT driver hasn't been able to properly open/allocate its structure
#define NC_ERROR_EDT_CLOSE_STRUCT           405		//The EDT driver hasn't been able to properly close/free its structure
#define NC_ERROR_EDT_MULTIBUF               406		//The EDT driver hasn't been able to allocate the desired number of loop buffer
#define NC_ERROR_EDT_SET_BAUD               407		//The EDT driver can't set this baudrate.
#define NC_ERROR_EDT_BIN_CMD                408		//The binary command can't be sent using the EDT driver
#define NC_ERROR_EDT_SET_TIMEOUT            409		//The timeout can't be set using the the EDT driver
#define NC_ERROR_EDT_STOP_BUFFERS           410		//The EDT drivers can't have the buffer to stop receiving images
#define NC_ERROR_EDT_PARAM_SIZE             411		//The EDT driver hasn't been able to set the width or the heigth passed in parameter
#define NC_ERROR_EDT_SET_SIZE               412		//The EDT driver hasn't been able to modifiy the buffer size receiving the image
#define NC_ERROR_EDT_SET_ROI                413		//The EDT driver hasn't been able to modify the buffer size so it size is now matching the buffer receive
#define NC_ERROR_EDT_CANCEL_EVENT           414		//The EDT driver can't cancel the grab event linked to the structure passed in parameter
#define NC_ERROR_EDT_SET_EVENT              415		//The EDT driver isn't able to properly set the grab event
#define NC_ERROR_EDT_DEVICE_FOUND			416   //No device linked through EDT have been found, make sure the cable are correctly plus and try to reboot the device

	//501 to 600 are related to the virtual mode errors
#define NC_ERROR_VIRTUAL_NO_CAMERA          501		//There's no virtual camera specified in the Nuvu Cameras configuration file
#define NC_ERROR_VIRTUAL_DETECTOR_TYPE      502		//There's no detector type specified in the Nuvu Cameras configuration file
#define NC_ERROR_VIRTUAL_DETECTOR_WIDTH     503		//There's no detector width specified in the Nuvu Cameras configuration file
#define NC_ERROR_VIRTUAL_DETECTOR_HEIGHT    504		//There's no detector height specified in the Nuvu Cameras configuration file
#define NC_ERROR_VIRTUAL_TEMP_MIN           505		//There's no minimal temperature specified in the Nuvu Cameras configuration file
#define NC_ERROR_VIRTUAL_TEMP_MAX           506		//There's no maximal temperature specified in the Nuvu Cameras configuration file
#define NC_ERROR_VIRTUAL_ACTIVE_WIDTH		507
#define NC_ERROR_VIRTUAL_ACTIVE_HEIGHT		508
#define NC_ERROR_VIRTUAL_NO_IMAGES			509

	//601 to 700 are related to PLEORA errors
#define NC_ERROR_PLEORA_SIZE_MISMATCH       601
#define NC_ERROR_PLEORA_TIMEOUT_MISMATCH    602
#define NC_ERROR_PLEORA_FIND_SYSEM          603		//The Pleora driver hasn't been able to proceed with the detection of the GigE devices
#define NC_ERROR_PLEORA_CONNECTION_FAILED   604		//The connection to the requested GigE device failed
#define NC_ERROR_PLEORA_DEVICE_FOUND        605		//No GigE device have been found, make sure the cable are correctly plus and try to reboot the device
#define NC_ERROR_PLEORA_STREAM_OPEN         606		//The Pleora GigE stream can't be opened with the requested device
#define NC_ERROR_PLEORA_STREAM_DESTINATION  607		//The Pleora GigE stream destination can't be set
#define NC_ERROR_PLEORA_PIPELINE_BUFFERS    608		//The Pleora acquisitions buffers can't be allocated by the pleora driver
#define NC_ERROR_PLEORA_SET_TIMEOUT         609		//The modification request, to have the timeout modified, failed
#define NC_ERROR_PLEORA_SET_DEPTH           610		//The modification request, to have the pixel depth modified, failed
#define NC_ERROR_PLEORA_DVAL_ENABLE         611		//The modification request, to have the DVAL signal enable, failed
#define NC_ERROR_PLEORA_SET_WIDTH           612		//The modification request, to have the image width modified, failed
#define NC_ERROR_PLEORA_SET_HEIGHT          613		//The modification request, to have the image height modified, failed
#define NC_ERROR_PLEORA_SET_BAUDRATE        614		//The modification request, to have the serial baudrate modified, failed
#define NC_ERROR_PLEORA_STOP_BITS           615		//The modification request, to have the number of stop bits set to one, failed
#define NC_ERROR_PLEORA_TEST_IMAGE          616		//The modification request, to have the test image disabled, failed
#define NC_ERROR_PLEORA_TICK_FREQUENCY      617		//The modification request, to get the tick frequency per second, failed
#define NC_ERROR_PLEORA_SET_PARITY          618		//The modification request, to have the parity set to none, failed
#define NC_ERROR_PLEORA_RESET_TIMSTAMP      619		//The modification request, to reset the timestamp, failed
#define NC_ERROR_PLEORA_SET_LOOPBACK        620		//The modification request, to disable the Uart loopback, failed
#define NC_ERROR_PLEORA_ACQUISITION_START   621		//The command, to have the acquisition started, failed
#define NC_ERROR_PLEORA_ACQUISITION_STOP    622		//The command, to have the acquisotion stopped, failed
#define NC_ERROR_PLEORA_PORT_OPEN           623		//The Pleora GigE port can't be opened with the requested device
#define NC_ERROR_PLEORA_SERIAL_SIZE         624		//The Pleora GigE driver can't set the serial buffer size
#define NC_ERROR_PLEORA_PIPELINE_START      625		//The Pleora GigE driver can't have the image pipeline to start
#define NC_ERROR_PLEORA_SERIAL_WRITE        626		//The Pleora GigE driver can't write the requested command to the serial port
#define NC_ERROR_PLEORA_SET_HEARTBEAT		627
#define NC_ERROR_PLEORA_SET_DEVICE			628
#define NC_ERROR_PLEORA_SET_PORT			629
#define NC_ERROR_PLEORA_NEGOCIATE_PACKET    630
#define NC_ERROR_PLEORA_NOT_SUPPORTED		631
#define NC_ERROR_PLEORA_REQUEST_TIMEOUT		632		//The modification request, to set the timeout on an image request, failed

	//701 to 800 are related to device enumeration and service errors
#define NC_ERROR_SERVICE_WINSOCK_START		701   //The winsock library hasn't started properly
#define NC_ERROR_SERVICE_WINSOCK_VERSION    702   //The winsock version used in the dll isn't the one expected
#define NC_ERROR_SERVICE_CREATE_SOCKET      703   //The application hasn't been able to create a socket to connect to the service
#define NC_ERROR_SERVICE_CONNECT_SERVICE    704   //The application hasn't been able to connect itself to the service
#define NC_ERROR_SERVICE_SEND_COMMAND       705   //The application hasn't been able to send a command to the service
#define NC_ERROR_SERVICE_RECEIVE_COMMAND    706   //The application hasn't been able to receive the answer from the service
#define NC_ERROR_SERVICE_DEVICE_LOCK        707   //The device you're trying to open is already locked by another application
#define NC_ERROR_SERVICE_LOCK_ACCESS_DENIED	708   //The device lock you're trying to access belongs to another process

    //801 to 900 are related to statistical errors
#define NC_ERROR_STATS_INVALID_INDEX        801
#define NC_ERROR_STATS_REGION_OUT           802      //The statsCtx selected is outside the boundaries of the image
#define NC_ERROR_STATS_IMAGE_SIZE           803 
#define NC_ERROR_STATS_STRUCT_PTR           804

	//901 to 1000 are related to MIL errors
#define NC_ERROR_MIL_DEVICE_FOUND           901
#define NC_ERROR_MIL_NOT_SUPPORTED          902
#define NC_ERROR_MIL_SET_SIZE               903
#define NC_ERROR_MIL_BUFFER_ALLOCATION      904		// Use the MILConfig tool to reserve additional non-paged memory
#define NC_ERROR_MIL_UART_UAVAILABLE        905
#define NC_ERROR_APPLICATION_ALLOCATION     906
#define NC_ERROR_MIL_SMALL_WIDTH			907
#define NC_ERROR_MIL_PORT_UNAVAILABLE       908

	//1001 to 1110 are related to SAPERA errors
#define NC_ERRROR_SAPERA_SERIAL_FUNCTION	1001
#define NC_ERROR_SAPERA_SERIAL_READ         1002
#define NC_ERROR_SAPERA_SERIAL_WRITE        1003
#define NC_ERROR_SAPERA_SERIAL_UNREAD       1004
#define NC_ERROR_SAPERA_DEVICE_FOUND        1005
#define NC_ERROR_SAPERA_INVALID_BAUDRATE    1006
#define NC_ERROR_SAPERA_SET_BAUDRATE        1007
#define NC_ERROR_SAPERA_CREATE_ACQUISITION  1008
#define NC_ERROR_SAPERA_SERIAL_PORT			1009
#define NC_ERROR_SAPERA_SERIAL_INIT         1010
#define NC_ERROR_SAPERA_SERIAL_MATCH        1011
#define NC_ERROR_SAPERA_SERIAL_CLOSE        1012
#define NC_ERROR_SAPERA_ACQ_DESTROY         1013
#define NC_ERROR_SAPERA_NBR_SERIAL          1014
#define NC_ERROR_SAPERA_ACQ_CREATE          1015
#define NC_ERROR_SAPERA_BUFFERS_CREATE      1016
#define NC_ERROR_SAPERA_XFER_CREATE         1017
#define NC_ERROR_SAPERA_DESTROY_XFER        1018
#define NC_ERROR_SAPERA_DESTROY_BUFFERS     1019
#define NC_ERROR_SAPERA_DESTROY_ACQ         1020
#define NC_ERROR_SAPERA_BUFFERS_WIDTH       1021
#define NC_ERROR_SAPERA_BUFFERS_HEIGHT      1022
#define NC_ERROR_SAPERA_ACQ_WIDTH           1023
#define NC_ERROR_SAPERA_CROP_WIDTH          1024
#define NC_ERROR_SAPERA_ACQ_HEIGHT          1025
#define NC_ERROR_SAPERA_CROP_HEIGHT         1026
#define NC_ERROR_SAPERA_GRAB_START          1027
#define NC_ERROR_SAPERA_XFER_FREEZE         1028
#define NC_ERROR_SAPERA_XFER_WAIT           1029
#define NC_ERROR_SAPERA_XFER_ABORT          1030
#define NC_ERROR_SAPERA_WIDTH_OUT           1031
#define NC_ERROR_SAPERA_HEIGHT_OUT          1032
#define NC_ERROR_SAPERA_XFER_INIT			1033
#define NC_ERROR_SAPERA_SERIAL_READ_TIMEOUT	1034
#define NC_ERROR_SAPERA_RESSOURCE_CREATE	1035
#define NC_ERROR_SAPERA_ASSERT_FAILURE		1036

	//1111 to 1200 are related to serial communication plugin errors
#define NC_ERROR_SERIAL_PLUGIN_INVALID_UNIT	1111

	//1201 to 1300 are related to logging plugin errors
#define NC_ERROR_LOGGING_FAILED_TO_START	1201


	// 2001 to 2100 refer to system configuration errors
#define NC_ERROR_VERSION_FAILED				2001	// Failed to parse a version string
#define NC_ERROR_STRING_CAST_FAILED			2002	// Failed to obtain a value from a string
#define NC_ERROR_ENV_VAR_ACCESS_FAILED		2003
#define NC_ERROR_SET_WORKING_DIR_FAILED		2004	// Failed to change the present working directory

	// 3001 to 3100 refer to processing pipeline framework errors
#define NC_ERROR_PIPELINE_ERROR				3001

	// 4001 to 4100 refer to inter-process errors
#define NC_ERROR_GLOBAL_MUTEX_CREATE_FAILED		4001	// An error occured while creating a global mutex file, check the lock files directory write permissions
#define NC_ERROR_GLOBAL_MUTEX_LOCK_FAILED		4002
#define NC_ERROR_SHARED_MEMORY_CREATE_FAILED	4003
#define NC_ERROR_SHARED_MEMORY_TOO_SMALL		4004
#define NC_ERROR_SHARED_MEMORY_ACCESS_FAILED	4005
#define NC_ERROR_PROCESS_FAILED_TO_FIND_BIN		4006
#define NC_ERROR_PROCESS_FAILED_TO_START_BIN	4007
#define NC_ERROR_PROCESS_FAILED_TO_OPEN_PIPE	4008
#define NC_ERROR_PROCESS_PIPE_IO_ERROR			4009

	// 4101 to 4200 refer to integrity validation errors
#define NC_ERROR_INTEGRITY_ERROR			4101

	// 5001 to 5100 refer to file system errors
#define NC_ERROR_FAILED_TO_MAKE_DIR			5001
#define NC_ERROR_FAILED_TO_OPEN_FILE		5002
#define NC_ERROR_FAILED_TO_DELETE_FILE		5003
#define NC_ERROR_FILE_IO_FAILED				5004
#define NC_ERROR_BAD_FILE_STATE				5005

	// 6001 to 6100 refer to plugin or board exclusion errors
#define NC_ERROR_FAILED_TO_LOAD_PLUGIN		6001
#define NC_ERROR_FAILED_TO_LOAD_SYMBOL		6002
#define NC_ERROR_SYMBOL_NOT_LOADED			6003
#define NC_ERROR_PLUGIN_DIRECTORY_MISSING	6004
#define NC_ERROR_PLUGIN_METADATA_MISSING	6005
#define NC_ERROR_BOARD_NAME_TOO_LONG		6006
#define NC_ERROR_TAP_TAG_TOO_LONG			6007
#define NC_ERROR_TAP_ALREADY_LOCKED			6008
#define NC_ERROR_PLUGIN_THREW_EXCEPTION		6009
#define NC_ERROR_PLUGIN_TOO_NEW				6010

	// 7001 to 7100 refer to internal library errors
#define NC_ERROR_CORE_OVERFLOW				7001
#define NC_ERROR_CORE_UNDERFLOW				7002
#define NC_ERROR_TIME_CAST_FAILED			7003
#define NC_ERROR_OUT_OF_BOUNDS				7004

	// 8001 to 9000 refer to plugin configuration errors
#define NC_ERROR_PLUGIN_LIST_STRUCT_PTR		8001	//The plugin list pointer passed to the function is invalid
#define NC_ERROR_PLUGIN_STRUCT_PTR			8002	//The plugin pointer passed to the function is invalid

#endif	// NC_ERROR_CODE_H

