#ifndef NC_API_H
#define NC_API_H

#ifdef __cplusplus
extern "C" {
#endif

NC_FUNCTION int ncWriteFileHeader(NcImageSaved *currentFile, enum HeaderDataType dataType, const char *name, const void *value, const char *comment);
NC_FUNCTION int ncReadFileHeader(NcImageSaved *currentFile, enum HeaderDataType dataType, const char *name, const void *value);

NC_FUNCTION int ncImageGetFileFormat(NcImageSaved *image, enum ImageFormat * format);

//==========================
// Device listing functions.
NC_FUNCTION int ncControllerListOpen(NcCtrlList * ctrlList);
NC_FUNCTION int ncControllerListOpenBasic(NcCtrlList * ctrlList);
NC_FUNCTION int ncControllerListFree(NcCtrlList ctrlList);
NC_FUNCTION int ncControllerListGetSize(const NcCtrlList ctrlList, int * listSize);
NC_FUNCTION int ncControllerListGetSerial(const NcCtrlList ctrlList, int index, char* serial, int serialSize);
NC_FUNCTION int ncControllerListGetModel(const NcCtrlList ctrlList, int index, char* model, int modelSize);
NC_FUNCTION int ncControllerListGetPortUnit(const NcCtrlList ctrlList, int index, int * unit);
NC_FUNCTION int ncControllerListGetPortChannel(const NcCtrlList ctrlList, int index, int * channel);
NC_FUNCTION int ncControllerListGetPortInterface(const NcCtrlList ctrlList, int index, char* acqInterface, int acqInterfaceSize);
NC_FUNCTION int ncControllerListGetUniqueID(const NcCtrlList ctrlList, int index, char* uniqueID, int uniqueIDSize);
NC_FUNCTION int ncControllerListGetFullSizeSize(const NcCtrlList ctrlList, int index, int* detectorSizeX, int* detectorSizeY);
NC_FUNCTION int ncControllerListGetDetectorSize(const NcCtrlList ctrlList, int index, int* detectorSizeX, int* detectorSizeY);
NC_FUNCTION int ncControllerListGetDetectorType(const NcCtrlList ctrlList, int index, char* detectorType, int detectorTypeSize);
NC_FUNCTION int ncControllerListGetLargestCLinkMode(const NcCtrlList ctrlList, int index, enum CameraLinkMode* clinkMode, int * isSupported, int * isAvailable);

NC_FUNCTION int ncControllerListGetFreePortCount(const NcCtrlList ctrlList, int * portCount);
NC_FUNCTION int ncControllerListGetFreePortUnit(const NcCtrlList ctrlList, int index, int * unit);
NC_FUNCTION int ncControllerListGetFreePortChannel(const NcCtrlList ctrlList, int index, int * channel);
NC_FUNCTION int ncControllerListGetFreePortInterface(const NcCtrlList ctrlList, int index, char* acqInterface, int acqInterfaceSize);
NC_FUNCTION int ncControllerListGetFreePortUniqueID(const NcCtrlList ctrlList, int index, char* uniqueID, int uniqueIDSize);
NC_FUNCTION int ncControllerListGetFreePortReason(const NcCtrlList ctrlList, int index, enum NcPortUnusedReason* reason);

NC_FUNCTION int ncControllerListGetPluginCount(const NcCtrlList ctrlList, int * listSize);
NC_FUNCTION int ncControllerListGetPluginName(const NcCtrlList ctrlList, int index, char* pluginName, int pluginNameSize);


//==========================
// Grab context functions:
//  camera configuration handled by the user via serial commands.
NC_FUNCTION int ncGrabSetOpenMacAdress(char* macAddress);
NC_FUNCTION int ncGrabOpen(int unit, int channel, int nbrBuffer, NcGrab* grab);
NC_FUNCTION int ncGrabOpenFromList(const NcCtrlList ctrlList, int index, int nbrBuffer, NcGrab* grab);
/*Deprecated*/ NC_FUNCTION int ncGrabOpenUnlock(int unit, int channel, int nbrBuffer, NcGrab* grab);
NC_FUNCTION int ncGrabClose(NcGrab grab);

NC_FUNCTION int ncGrabSetBufferCount(NcGrab grab, int nbrBuffer);
NC_FUNCTION int ncGrabGetBufferCount(NcGrab grab, int *nbrBuffer);

NC_FUNCTION int ncGrabSetHeartbeat(NcGrab grab, int timeMs);
NC_FUNCTION int ncGrabGetHeartbeat(NcGrab grab, int *timeMs);

NC_FUNCTION int ncGrabStart(NcGrab grab, int nbrImages);
NC_FUNCTION int ncGrabAbort(NcGrab grab);
NC_FUNCTION int ncGrabFinish(NcGrab grab);
NC_FUNCTION int ncGrabSetFinishEvent(NcGrab grab, void(*callback)(NcGrab, void*), void* data);
NC_FUNCTION int ncGrabIsAcquiring(NcGrab grab, int* isAcquiring);

NC_FUNCTION int ncGrabRead(NcGrab grab, NcImage** imageAcqu);
/*Deprecated*/NC_FUNCTION int ncGrabReadTimed(NcGrab grab, NcImage** imageAcqu, double *receiveTime);
NC_FUNCTION int ncGrabReadChronological(NcGrab grab, NcImage** imageAcqu, int* nbrImagesSkipped);
NC_FUNCTION int ncGrabReadChronologicalNonBlocking(NcGrab grab, NcImage** imageAcqu, int* nbrImagesSkipped);

NC_FUNCTION int ncGrabValidateReadoutImageSize(NcGrab grab, const void * imagePtr, int* imageSize);

NC_FUNCTION int ncGrabWaitUntilImageWritable(NcGrab grab, unsigned short ** writableImage);
NC_FUNCTION int ncGrabWaitForSaveQueueFlushed(NcGrab grab);
NC_FUNCTION int ncGrabGetSaveQueueLength(NcGrab grab, int* length);

NC_FUNCTION int ncGrabOpenImageParams(ImageParams *imageParams);
NC_FUNCTION int ncGrabGetImageParams(NcGrab grab, const void* imageNc, ImageParams imageParams);
NC_FUNCTION int ncGrabCloseImageParams(ImageParams imageParams);

NC_FUNCTION int ncGrabFlushReadQueues(NcGrab grab);
NC_FUNCTION int ncGrabGetOverrun(NcGrab grab, int* overrunOccurred);
NC_FUNCTION int ncGrabGetSaveErrors(NcGrab grab, int* saveErrors);
NC_FUNCTION int ncGrabGetNbrDroppedImages(NcGrab grab, int* nbrDroppedImages);
NC_FUNCTION int ncGrabGetNbrTimeout(NcGrab grab, int* nbrTimeout);

NC_FUNCTION int ncGrabGetFramerate(NcGrab grab, double* fps);

NC_FUNCTION int ncGrabSetTimeout(NcGrab grab, int timeMs);
NC_FUNCTION int ncGrabGetTimeout(NcGrab grab, int* timeTimeout);

NC_FUNCTION int ncGrabEnableRawBufferingToDisk(NcGrab grab, int acquisitionSize, const char * directory);
NC_FUNCTION int ncGrabEnableRawBufferingToMem(NcGrab grab, int acquisitionSize);
NC_FUNCTION int ncGrabGetRawBuffersState(NcGrab grab, int * freeBuffers, int * totalBuffers);

NC_FUNCTION int ncGrabValidateSize(NcGrab grab, int width, int height, int* widthPadding, int* heightPadding);
NC_FUNCTION int ncGrabSetSize(NcGrab grab, int width, int height);
NC_FUNCTION int ncGrabGetSize(NcGrab grab, int* width, int* height);
/*Deprecated*/ NC_FUNCTION int ncGrabGetHorizontalRestriction(NcGrab grab, int* restriction);
NC_FUNCTION int ncGrabGetImageSizeRestriction(NcGrab grab, int* horizontal, int* vertical, int* pixelCount);

NC_FUNCTION int ncGrabSaveImage(NcGrab grab, NcImage* imageNc, const char* saveName, enum ImageFormat saveFormat, int overwriteFlag);
NC_FUNCTION int ncGrabSaveImageEx(NcGrab grab, const void* imageNc, const char* saveName, enum ImageFormat saveFormat, enum ImageDataType dataFormat, int overwriteFlag);
NC_FUNCTION int ncGrabSaveImageCommentedEx(NcGrab grab, const void* imageNc, const char* saveName, enum ImageFormat saveFormat, enum ImageDataType dataFormat, const char* addComments, int overwriteFlag);
NC_FUNCTION int ncGrabStartSaveAcquisition(NcGrab grab, const char *saveName, enum ImageFormat saveFormat, int imagesPerCube, int nbrOfCubes, int overwriteFlag);
NC_FUNCTION int ncGrabStopSaveAcquisition(NcGrab grab);

NC_FUNCTION int ncGrabSaveImageSetHeaderCallback(NcGrab grab, void (*fct)(NcGrab grab, NcImageSaved *imageFile, void *data), void *data);
NC_FUNCTION int ncGrabSaveImageWriteCallback(NcGrab grab, void (*fct)(NcGrab grab, int imageNo, void *data), void *data);
NC_FUNCTION int ncGrabSaveImageCloseCallback(NcGrab grab, void (*fct)(NcGrab grab, int fileNo, void *data), void *data);
NC_FUNCTION int ncGrabSaveImageSequenceEndCallback(NcGrab grab, void (*fct)(NcGrab grab, void *data, int received, int saved), void *data);
NC_FUNCTION int ncGrabSaveImageSetCompressionType(NcGrab grab, enum ImageCompression compress);
NC_FUNCTION int ncGrabSaveImageGetCompressionType(NcGrab grab, enum ImageCompression *compress);

NC_FUNCTION int ncGrabSaveParam(NcGrab grab, const char *saveName, int overwriteFlag);
NC_FUNCTION int ncGrabLoadParam(NcGrab grab, const char *saveName);
NC_FUNCTION int ncGrabSaveParamSetHeaderCallback(NcGrab grab, void (*fct)(NcProc ctx, NcImageSaved *imageFile, void *data), void *data);
NC_FUNCTION int ncGrabLoadParamSetHeaderCallback(NcGrab grab, void (*fct)(NcProc ctx, NcImageSaved *imageFile, void *data), void *data);

NC_FUNCTION int ncGrabSetTimestampMode(NcGrab grab, enum TimestampMode timestampMode);
NC_FUNCTION int ncGrabGetTimestampMode(NcGrab grab, int ctrlRequest, enum TimestampMode *timestampMode, int *gpsSignalValid);
NC_FUNCTION int ncGrabSetTimestampInternal(NcGrab grab, struct tm *dateTime, int nbrUs);
NC_FUNCTION int ncGrabGetCtrlTimestamp(NcGrab grab, NcImage* imageAcqu, struct tm *ctrlDatetime, double *ctrlSecondFraction, int *status);
NC_FUNCTION int ncGrabGetHostSystemTimestamp (NcGrab grab, NcImage* imageAcqu, double *hostSystemTimestamp);

NC_FUNCTION int ncGrabGetImageCtrlTimestamp(ImageParams imageParams, struct tm *ctrlDatetime, double *ctrlSecondFraction, int *status);
NC_FUNCTION int ncGrabGetImageHostTimestamp(ImageParams imageParams, double *hostSystemTimestamp);
NC_FUNCTION int ncGrabGetImageTimestampMode(ImageParams imageParams, int *hasCtrlTimestamp);

NC_FUNCTION int ncGrabParamAvailable(NcGrab grab, enum Features param, int setting);

NC_FUNCTION int ncGrabResetTimer(NcGrab grab, double timeOffset);
NC_FUNCTION int ncGrabSetEvent(NcGrab grab, NcCallbackFunc funcName, void* ncData);
NC_FUNCTION int ncGrabCancelEvent(NcGrab grab);


// Communication settings 
NC_FUNCTION int ncGrabSetSerialTimeout(NcGrab grab, int serialTimeout);
NC_FUNCTION int ncGrabGetSerialTimeout(NcGrab grab, int *serialTimeout);
NC_FUNCTION int ncGrabSetBaudrate (NcGrab grab, int baudrateSpeed);

// Communication functions 
NC_FUNCTION int ncGrabSendSerialCommand(NcGrab grab, const char * command, char * reply, unsigned int * reply_length);
NC_FUNCTION int ncGrabFormatSerialCommand(NcGrab grab, enum NcSerialProtocol format, const char * command, unsigned int * length, char * binary_comm);
NC_FUNCTION int ncGrabSendSerialBinaryComm (NcGrab grab, const char *command, int length);
NC_FUNCTION int ncGrabWaitSerialCmd(NcGrab grab, int length, int* numByte);
NC_FUNCTION int ncGrabRecSerial (NcGrab grab, char *recBuffer, int length, int* numByte);
NC_FUNCTION int ncGrabGetSerialUnreadBytes(NcGrab grab, int* numByte);

/*Deprecated*/ NC_FUNCTION int _ncGrabNbrImagesAcquired(NcGrab grab, int *nbrImages);
NC_FUNCTION int  ncGrabNbrImagesAcquired(NcGrab grab, int *nbrImages);

NC_FUNCTION int ncGrabGetVersion(NcGrab grab, enum VersionType versionType, char * version, int bufferSize);

NC_FUNCTION int ncGrabCreateBias(NcGrab grab, int nbrImages);
NC_FUNCTION int ncGrabCancelBiasCreation(NcGrab grab);
NC_FUNCTION int ncGrabSaveBiasImage(NcGrab grab, const char *saveName, int overwriteFlag);
NC_FUNCTION int ncGrabSetBiasClampLevel(NcGrab grab, int clamp);
NC_FUNCTION int ncGrabGetBiasClampLevel(NcGrab grab, int * clamp);
NC_FUNCTION int ncGrabSetProcType(NcGrab grab, int type, int nbrImagesPc);
NC_FUNCTION int ncGrabGetProcType(NcGrab grab, int * type, int * nbrImagesPc);
NC_FUNCTION int ncGrabCreateBiasNewImageCallback(NcGrab grab, void (*fct)(NcGrab grab, int imageNo, void *data), void *data);

NC_FUNCTION int ncGrabStatsAddRegion(NcGrab grab, int regionWidth, int regionHeight, int *regionIndex);
NC_FUNCTION int ncGrabStatsRemoveRegion(NcGrab grab, int regionIndex);
NC_FUNCTION int ncGrabStatsResizeRegion(NcGrab grab, int regionIndex, int regionWidth, int regionHeight);
NC_FUNCTION int ncGrabStatsGetCrossSection(NcGrab grab, int regionIndex, NcImage *image, int xCoord, int yCoord, double statsCtxRegion[5], double **histo, double **crossSectionHorizontal, double **crossSectionVertical);
NC_FUNCTION int ncGrabStatsGetGaussFit(NcGrab grab, int regionIndex, NcImage *image, int xCoord, int yCoord, double *maxAmplitude, double gaussSumHorizontal[3], double gaussSumVertical[3], int useActualCrossSection);

NC_FUNCTION int ncGrabGetControllerCameraLinkMode(NcGrab grab, enum CameraLinkMode * CLmode, unsigned int * CLpath);
NC_FUNCTION int ncGrabSetControllerCameraLinkMode(NcGrab grab, enum CameraLinkMode CLmode);
NC_FUNCTION int ncGrabGetControllerReadoutTaps(NcGrab grab, int * taps);
NC_FUNCTION int ncGrabSetControllerReadoutTaps(NcGrab grab, int taps);
//Pending NC_FUNCTION int ncGrabGetControllerSensorTaps(NcGrab grab, int * taps);
//Pending NC_FUNCTION int ncGrabSetControllerSensorTaps(NcGrab grab, int taps);
NC_FUNCTION int ncGrabGetControllerPixelDepth(NcGrab grab, int * bits);
NC_FUNCTION int ncGrabSetControllerPixelDepth(NcGrab grab, int bits);
NC_FUNCTION int ncGrabGetFrameGrabberPixelDepth(NcGrab grab, int * bits);
NC_FUNCTION int ncGrabSetReadoutTapLayout(NcGrab grab, const int * taps, int tapCount);

NC_FUNCTION int ncGrabSilenceFunctionLogging(NcGrab grab, const char * func_name, int silenced);


//==========================
// Camera context functions:
//  camera configuration handled transparently.
NC_FUNCTION int ncCamSetOpenMacAdress(char* macAddress);
NC_FUNCTION int ncCamOpen(int unit, int channel, int nbrBuffer, NcCam* cam);
NC_FUNCTION int ncCamOpenFromList(const NcCtrlList ctrlList, int index, int nbrBuffer, NcCam* cam);
/*Deprecated*/ NC_FUNCTION int ncCamOpenUnlock(int unit, int channel, int nbrBuffer, NcCam* cam);
NC_FUNCTION int ncCamPrepareShutdown(NcCam cam, int blockingMode, int(*callback)(double, double));
NC_FUNCTION int ncCamClose(NcCam cam);
/*Deprecated*/ NC_FUNCTION int ncCamReadyToClose(NcCam cam, void (*fct)(NcCam cam, void *data), void *data);

NC_FUNCTION int ncCamSetBufferCount(NcCam cam, int nbrBuffer, int dynamic);
NC_FUNCTION int ncCamGetBufferCount(NcCam cam, int *nbrBuffer, int * dynamic);
NC_FUNCTION int ncCamGetDynamicBufferCount(NcCam cam, int *nbrBuffer);
NC_FUNCTION int ncCamGetDefaultBufferCount(NcCam cam, int *nbrBuffer);

NC_FUNCTION int ncCamSetHeartbeat(NcCam cam, int timeMs);
NC_FUNCTION int ncCamGetHeartbeat(NcCam cam, int *timeMs);

NC_FUNCTION int ncCamStart(NcCam cam, int nbrImages);
NC_FUNCTION int ncCamPrepareAcquisition(NcCam cam, int nbrImages);
NC_FUNCTION int ncCamBeginAcquisition(NcCam cam);
NC_FUNCTION int ncCamAbort(NcCam cam);
NC_FUNCTION int ncCamFinish(NcCam cam);
NC_FUNCTION int ncCamSetFinishEvent(NcCam cam, void(*callback)(NcCam, void*), void* data);
NC_FUNCTION int ncCamIsAcquiring(NcCam cam, int* isAcquiring);

NC_FUNCTION int ncSaveImage(int width, int height, const ImageParams imageParams, const void* imageNc, enum ImageDataType dataType, const char* saveName, enum ImageFormat saveFormat, enum ImageCompression compress, const char* addComments, int overwriteFlag);
NC_FUNCTION int ncCamOpenImageParams(ImageParams *imageParams);
NC_FUNCTION int ncCamGetImageParams(NcCam cam, const void* imageNc, ImageParams imageParams);
NC_FUNCTION int ncCamCloseImageParams(ImageParams imageParams);

NC_FUNCTION int ncCamRead(NcCam cam, NcImage** imageAcqu);
NC_FUNCTION int ncCamReadUInt32(NcCam cam, uint32_t *image);
NC_FUNCTION int ncCamReadFloat(NcCam cam, float *image);

/*Deprecated*/	NC_FUNCTION int ncCamReadTimed(NcCam cam, NcImage** imageAcqu, double *receiveTime);
/*Deprecated*/	NC_FUNCTION int ncCamReadUInt32Timed(NcCam cam, uint32_t* imageAcqu, double *receiveTime);
/*Deprecated*/	NC_FUNCTION int ncCamReadFloatTimed(NcCam cam, float* imageAcqu, double *receiveTime);

NC_FUNCTION int ncCamReadChronological(NcCam cam, NcImage** imageAcqu, int* nbrImagesSkipped);
NC_FUNCTION int ncCamReadUInt32Chronological(NcCam cam, uint32_t* imageAcqu, int* nbrImagesSkipped);
NC_FUNCTION int ncCamReadFloatChronological(NcCam cam, float* imageAcqu, int* nbrImagesSkipped);

NC_FUNCTION int ncCamReadChronologicalNonBlocking(NcCam cam, NcImage **imageAcqu, int* nbrImagesSkipped);
NC_FUNCTION int ncCamReadUInt32ChronologicalNonBlocking(NcCam cam, uint32_t* imageAcqu, int* nbrImagesSkipped);
NC_FUNCTION int ncCamReadFloatChronologicalNonBlocking(NcCam cam, float* imageAcqu, int* nbrImagesSkipped);

NC_FUNCTION int ncCamValidateReadoutImageSize(NcCam cam, const void * imagePtr, int* imageSize);

NC_FUNCTION int ncCamWaitUntilImageWritable(NcCam cam, unsigned short ** writableImage);
NC_FUNCTION int ncCamWaitForSaveQueueFlushed(NcCam cam);
NC_FUNCTION int ncCamGetSaveQueueLength(NcCam cam, int* length);

NC_FUNCTION int ncCamAllocUInt32Image(NcCam cam, uint32_t **image);
NC_FUNCTION int ncCamFreeUInt32Image(uint32_t **image);

NC_FUNCTION int ncCamAllocFloatImage(NcCam cam, float **image);
NC_FUNCTION int ncCamFreeFloatImage(float **image);

NC_FUNCTION int ncCamFlushReadQueues(NcCam cam);
NC_FUNCTION int ncCamGetOverrun(NcCam cam, int* overrunOccurred);
NC_FUNCTION int ncCamGetSaveErrors(NcCam cam, int* saveErrors);
NC_FUNCTION int ncCamGetNbrDroppedImages(NcCam cam, int* nbrDroppedImages);
NC_FUNCTION int ncCamGetNbrTimeout(NcCam cam, int* nbrTimeout);

NC_FUNCTION int ncCamSetTimeout(NcCam cam, int timeMs);
NC_FUNCTION int ncCamGetTimeout(NcCam cam, int* timeTimeout);

NC_FUNCTION int ncCamEnableRawBufferingToDisk(NcCam cam, int acquisitionSize, const char * directory);
NC_FUNCTION int ncCamEnableRawBufferingToMem(NcCam cam, int acquisitionSize);
NC_FUNCTION int ncCamGetRawBuffersState(NcCam cam, int * freeBuffers, int * totalBuffers);

NC_FUNCTION int ncCamGetSize(NcCam cam, int *width, int *height);
NC_FUNCTION int ncCamGetFrameGrabberSize(NcCam cam, int *width, int *height);
NC_FUNCTION int ncCamGetMaxSize(NcCam cam, int *width, int *height);

NC_FUNCTION int	ncCamGetOverscanLines(NcCam cam, int *overscanLines);

NC_FUNCTION int ncCamSaveImage(NcCam cam, NcImage* imageNc, const char* saveName, enum ImageFormat saveFormat, const char* addComments, int overwriteFlag);
NC_FUNCTION int ncCamSaveUInt32Image(NcCam cam, const uint32_t *imageNc, const char *saveName, enum ImageFormat saveFormat, const char *addComments, int overwriteFlag);
NC_FUNCTION int ncCamSaveFloatImage(NcCam cam, const float *imageNc, const char *saveName, enum ImageFormat saveFormat, const char *addComments, int overwriteFlag);
NC_FUNCTION int ncCamSaveImageEx(NcCam cam, const void * imageNc, const char* saveName, enum ImageFormat saveFormat, enum ImageDataType dataFormat, const char* addComments, int overwriteFlag);
NC_FUNCTION int ncCamStartSaveAcquisition(NcCam cam, const char *saveName, enum ImageFormat saveFormat, int imagesPerCube, const char *addComments, int nbrOfCubes, int overwriteFlag);
NC_FUNCTION int ncCamStopSaveAcquisition(NcCam cam);

NC_FUNCTION int ncCamSaveImageSetHeaderCallback(NcCam cam, void (*fct)(NcCam cam, NcImageSaved *imageFile, void *data), void *data);
NC_FUNCTION int ncCamSaveImageWriteCallback(NcCam cam, void (*fct)(NcCam cam, int imageNo, void *data), void *data);
NC_FUNCTION int ncCamSaveImageCloseCallback(NcCam cam, void (*fct)(NcCam cam, int fileNo, void *data), void *data);
NC_FUNCTION int ncCamSaveImageSequenceEndCallback(NcCam cam, void(*fct)(NcCam cam, void *data, int received, int saved), void *data);
NC_FUNCTION int ncCamSaveImageSetCompressionType(NcCam cam, enum ImageCompression compress);
NC_FUNCTION int ncCamSaveImageGetCompressionType(NcCam cam, enum ImageCompression *compress);

NC_FUNCTION int ncCamResetTimer(NcCam cam, double timeOffset);

NC_FUNCTION int ncCamSetEvent(NcCam cam, NcCallbackFunc funcName, void *ncData);
NC_FUNCTION int ncCamCancelEvent(NcCam cam);

NC_FUNCTION int ncCamSetTimestampMode(NcCam cam, enum TimestampMode timestampMode);
NC_FUNCTION int ncCamGetTimestampMode(NcCam cam, int cameraRequest, enum TimestampMode *timestampMode, int *gpsSignalValid);
NC_FUNCTION int ncCamSetTimestampInternal(NcCam cam, struct tm *dateTime, int nbrUs);
NC_FUNCTION int ncCamGetCtrlTimestamp(NcCam cam, NcImage* imageAcqu, struct tm *ctrTimestamp, double *ctrlSecondFraction, int *status);
NC_FUNCTION int ncCamGetHostSystemTimestamp (NcCam cam, NcImage* imageAcqu, double *hostSystemTimestamp);

NC_FUNCTION int ncCamGetImageCtrlTimestamp(ImageParams imageParams, struct tm *ctrlDatetime, double *ctrlSecondFraction, int *status);
NC_FUNCTION int ncCamGetImageHostTimestamp(ImageParams imageParams, double *hostSystemTimestamp);
NC_FUNCTION int ncCamGetImageTimestampMode(ImageParams imageParams, int *hasCtrlTimestamp);

// Readout parameters
NC_FUNCTION int ncCamParamAvailable(NcCam cam, enum Features param, int setting);

NC_FUNCTION int ncCamSaveParam(NcCam cam, const char* saveName, int overwriteFlag);
NC_FUNCTION int ncCamLoadParam(NcCam cam, const char* saveName);
NC_FUNCTION int ncCamSaveParamSetHeaderCallback(NcCam cam, void (*fct)(NcProc ctx, NcImageSaved *imageFile, void *data), void *data);
NC_FUNCTION int ncCamLoadParamSetHeaderCallback(NcCam cam, void (*fct)(NcProc ctx, NcImageSaved *imageFile, void *data), void *data);

NC_FUNCTION int ncCamSetReadoutMode(NcCam cam, int value);
NC_FUNCTION int ncCamClearReadoutMode(NcCam cam);
NC_FUNCTION int ncCamGetCurrentReadoutMode(NcCam cam, int* readoutMode, enum Ampli* ampliType, char* ampliString, int *vertFreq, int *horizFreq);
NC_FUNCTION int ncCamGetReadoutMode(NcCam cam, int number, enum Ampli* ampliType, char* ampliString, int *vertFreq, int *horizFreq);
NC_FUNCTION int ncCamGetReadoutModeFlags(NcCam cam, int number, int * flags);
NC_FUNCTION int ncCamGetNbrReadoutModes(NcCam cam, int* nbrReadoutMode);

NC_FUNCTION int ncCamGetFramerate(NcCam cam, double * fps);
NC_FUNCTION int ncCamGetFramerateNominal(NcCam cam, int cameraRequest, double * fps);
NC_FUNCTION int ncCamGetTdiLinerateNominal(NcCam cam, int cameraRequest, double* lineRate_kHz);
NC_FUNCTION int ncCamGetFramerateMaximum(NcCam cam, int cameraRequest, double * fps);
NC_FUNCTION int ncCamGetReadoutTime(NcCam cam, double *time);
NC_FUNCTION int ncCamGetFrameTransferDuration(NcCam cam, double *frameTransferDuration);
NC_FUNCTION int ncCamGetFrameLatency(NcCam cam, int *frameLatency);

NC_FUNCTION int ncCamGetAmpliTypeAvail(NcCam cam, enum Ampli ampli, int *number);
NC_FUNCTION int ncCamGetFreqAvail(NcCam cam, enum Ampli ampli, int ampliNo, int *vertFreq, int *horizFreq, int* readoutModeNo);


// Intrinsic settings
NC_FUNCTION int ncCamSetExposureTime(NcCam cam, double exposureTime);
NC_FUNCTION int ncCamGetExposureTime(NcCam cam, int cameraRequest, double* exposureTime);
NC_FUNCTION int ncCamGetIntegrationTime(NcCam cam, int cameraRequest, double* integrationTime);

NC_FUNCTION int ncCamSetWaitingTime(NcCam cam, double waitingTime);
NC_FUNCTION int ncCamGetWaitingTime(NcCam cam, int cameraRequest, double* waitingTime);

NC_FUNCTION int ncCamSetTriggerMode(NcCam cam, enum TriggerMode triggerMode, int nbrImages);
NC_FUNCTION int ncCamGetTriggerMode(NcCam cam, int cameraRequest,  enum TriggerMode* triggerMode, int* nbrImagesPerTrig);

NC_FUNCTION int ncCamSetShutterMode(NcCam cam, enum ShutterMode shutterMode);
NC_FUNCTION int ncCamGetShutterMode(NcCam cam, int cameraRequest, enum ShutterMode* shutterMode);

NC_FUNCTION int ncCamSetShutterPolarity(NcCam cam, enum ExtPolarity shutterPolarity);
NC_FUNCTION int ncCamGetShutterPolarity(NcCam cam, int cameraRequest, enum ExtPolarity* shutterPolarity);

NC_FUNCTION int ncCamSetExternalShutter(NcCam cam, enum ExtShutter externalShutterPresence);
NC_FUNCTION int ncCamGetExternalShutter(NcCam cam, int cameraRequest, enum ExtShutter* externalShutterPresence);

NC_FUNCTION int ncCamSetExternalShutterMode(NcCam cam, enum ShutterMode externalShutterMode);
NC_FUNCTION int ncCamGetExternalShutterMode(NcCam cam, int cameraRequest, enum ShutterMode* externalShutterMode);

NC_FUNCTION int ncCamSetExternalShutterDelay(NcCam cam, double externalShutterDelay);
NC_FUNCTION int ncCamGetExternalShutterDelay(NcCam cam, int cameraRequest, double* externalShutterDelay);

NC_FUNCTION int ncCamSetFirePolarity(NcCam cam, enum ExtPolarity firePolarity);
NC_FUNCTION int ncCamGetFirePolarity(NcCam cam, int cameraRequest, enum ExtPolarity* firePolarity);

NC_FUNCTION int ncCamSetOutputMinimumPulseWidth(NcCam cam, double outputPulseWidth);
NC_FUNCTION int ncCamGetOutputMinimumPulseWidth(NcCam cam, int cameraRequest, double *outputPulseWidth);

NC_FUNCTION int ncCamSetArmPolarity(NcCam cam, enum ExtPolarity armPolarity);
NC_FUNCTION int ncCamGetArmPolarity(NcCam cam, int cameraRequest, enum ExtPolarity* armPolarity);


// Response settings
NC_FUNCTION int ncCamSetCalibratedEmGain(NcCam cam, int calibratedEmGain);
NC_FUNCTION int ncCamGetCalibratedEmGain(NcCam cam, int cameraRequest, int *calibratedEmGain);
NC_FUNCTION int ncCamGetCalibratedEmGainRange(NcCam cam, int* calibratedEmGainMin, int* calibratedEmGainMax);
NC_FUNCTION int ncCamGetCalibratedEmGainTempRange (NcCam cam, double* calibratedEmGainTempMin, double* calibratedEmGainTempMax);

NC_FUNCTION int ncCamSetRawEmGain(NcCam cam, int rawEmGain);
NC_FUNCTION int ncCamGetRawEmGain(NcCam cam, int cameraRequest, int* rawEmGain);
NC_FUNCTION int ncCamGetRawEmGainRange(NcCam cam, int* rawEmGainMin, int* rawEmGainMax);
/*Deprecated*/NC_FUNCTION int ncCamGetEmRawGainMin(NcCam cam, int* rawEmGainMin);
/*Deprecated*/NC_FUNCTION int ncCamGetEmRawGainMax(NcCam cam, int* rawEmGainMax);

NC_FUNCTION int ncCamGetKGainCount(NcCam cam, int* count);
NC_FUNCTION int ncCamGetKGainCountMax(NcCam cam, int* count);
NC_FUNCTION int ncCamGetKGain(NcCam cam, int index, float* kGain);

NC_FUNCTION int ncCamSetAnalogGain(NcCam cam, int analogGain);
NC_FUNCTION int ncCamGetAnalogGain(NcCam cam, int cameraRequest, int* analogGain);
NC_FUNCTION int ncCamGetAnalogGainRange(NcCam cam, int* analogGainMin, int* analogGainMax);
/*Deprecated*/NC_FUNCTION int ncCamGetAnalogGainMin (NcCam cam, int* analogGainMin);
/*Deprecated*/NC_FUNCTION int ncCamGetAnalogGainMax (NcCam cam, int* analogGainMax);

NC_FUNCTION int ncCamSetAnalogOffset(NcCam cam, int analogOffset);
NC_FUNCTION int ncCamGetAnalogOffset(NcCam cam, int cameraRequest, int* analogOffset);
NC_FUNCTION int ncCamGetAnalogOffsetRange (NcCam cam, int* analogOffsetMin, int* analogOffsetMax);
/*Deprecated*/NC_FUNCTION int ncCamGetAnalogOffsetMin (NcCam cam, int* analogOffsetMin);
/*Deprecated*/NC_FUNCTION int ncCamGetAnalogOffsetMax (NcCam cam, int* analogOffsetMax);


// Temperature control
NC_FUNCTION int ncCamSetTargetDetectorTemp(NcCam cam, double targetDetectorTemp);
NC_FUNCTION int ncCamGetDetectorTemp(NcCam cam, double* detectorTemp);
NC_FUNCTION int ncCamGetTargetDetectorTemp(NcCam cam, int cameraRequest, double* targetDetectorTemp);
NC_FUNCTION int ncCamGetTargetDetectorTempRange (NcCam cam, double *targetDetectorTempMin, double *targetDetectorTempMax);
/*Deprecated*/NC_FUNCTION int ncCamGetTargetDetectorTempMin (NcCam cam, double* targetDetectorTempMin);
/*Deprecated*/NC_FUNCTION int ncCamGetTargetDetectorTempMax (NcCam cam, double* targetDetectorTempMax);

NC_FUNCTION int ncCamGetComponentTemp(NcCam cam, enum NcTemperatureType temp, double * value);

/*Deprecated*/ NC_FUNCTION int ncCamGetControllerTemp(NcCam cam, double* controllerTemp);


// Identity
NC_FUNCTION int ncCamGetSerialNumber (NcCam cam, char *sn);
NC_FUNCTION int ncCamGetDetectorType(NcCam cam, enum DetectorType *type);
NC_FUNCTION int ncCamDetectorTypeEnumToString(enum DetectorType detectorType, const char** str);

NC_FUNCTION int ncCamGetVersion(NcCam cam, enum VersionType versionType, char * version, int bufferSize);
NC_FUNCTION int ncCamGetModel(NcCam cam, char* model, int modelSize);
NC_FUNCTION int ncCamGetFullCCDSize(NcCam cam, int *width, int *height);
NC_FUNCTION int ncCamGetActiveRegion(NcCam cam, int *width, int *height);


// Image layout
NC_FUNCTION int ncCamSetBinningMode(NcCam cam, int binXValue, int binYValue);
NC_FUNCTION int ncCamGetBinningMode(NcCam cam, int *binXValue, int *binYValue);
/*Deprecated*/NC_FUNCTION int ncCamGetBinningModesAvailable(NcCam cam, int *bin2XAvailable, int *bin4XAvailable, int *bin8XAvailable, int *bin16XAvailable, int* binYAvailable);

/*Deprecated*/NC_FUNCTION int ncCamSetRoi(NcCam cam, int startValueX, int endValueX, int startValueY, int endValueY);
/*Deprecated*/NC_FUNCTION int ncCamMoveRoi(NcCam cam, int startValueX, int startValueY);
/*Deprecated*/NC_FUNCTION int ncCamGetRoi(NcCam cam, int *startValueX, int *endValueX, int *startValueY, int *endValueY);
/*Deprecated*/NC_FUNCTION int ncCamGetRoisAvailable(NcCam cam);

NC_FUNCTION int ncCamSetMRoiSize(NcCam cam, int index, int width, int height);
NC_FUNCTION int ncCamGetMRoiSize(NcCam cam, int index, int * width, int * height);
NC_FUNCTION int ncCamSetMRoiPosition(NcCam cam, int index, int offsetX, int offsetY);
NC_FUNCTION int ncCamGetMRoiPosition(NcCam cam, int index, int * offsetX, int * offsetY);
NC_FUNCTION int ncCamGetMRoiCount(NcCam cam, int * count);
NC_FUNCTION int ncCamGetMRoiCountMax(NcCam cam, int * count);
NC_FUNCTION int ncCamAddMRoi(NcCam cam, int offsetX, int offsetY, int width, int height);
NC_FUNCTION int ncCamDeleteMRoi(NcCam cam, int index);

NC_FUNCTION int ncCamGetMRoiInputRegion(ImageParams params, int index, int * offsetX, int * offsetY, int * width, int * height);
NC_FUNCTION int ncCamGetMRoiOutputRegion(ImageParams params, int index, int * offsetX, int * offsetY, int * width, int * height);
NC_FUNCTION int ncCamGetMRoiRegionCount(ImageParams params, int * count);

NC_FUNCTION int ncCamMRoiApply(NcCam cam);
NC_FUNCTION int ncCamMRoiRollback(NcCam cam);
NC_FUNCTION int ncCamMRoiHasChanges(NcCam cam, int * hasChanges);
NC_FUNCTION int ncCamMRoiCanApplyWithoutStop(NcCam cam, int * nonStop);


NC_FUNCTION int ncCamSetCropMode( NcCam cam, enum CropMode mode, int paddingPixelsMinimumX, int paddingPixelsMinimumY );
NC_FUNCTION int ncCamGetCropMode( NcCam cam, enum CropMode* mode, int* paddingPixelsMinimumX, int* paddingPixelsMinimumY, float* figureOfMerit);

NC_FUNCTION int ncCropModeSolutionsOpen( NcCropModeSolutions* solutionSet, int cropWidth, int cropHeight, enum CropMode mode, int paddingPixelsMinimumX, int paddingPixelsMinimumY, NcCam cam);
NC_FUNCTION int ncCropModeSolutionsRefresh( NcCropModeSolutions solutionSet );
NC_FUNCTION int ncCropModeSolutionsSetParameters( NcCropModeSolutions solutionSet, int cropWidth, int cropHeight, enum CropMode mode, int paddingPixelsMinimumX, int paddingPixelsMinimumY);
NC_FUNCTION int ncCropModeSolutionsGetParameters( NcCropModeSolutions solutionSet, int* cropWidth, int* cropHeight, enum CropMode* mode, int* paddingPixelsMinimumX, int* paddingPixelsMinimumY);
NC_FUNCTION int ncCropModeSolutionsGetTotal( NcCropModeSolutions solutionSet, int* totalNbrSolutions);
NC_FUNCTION int ncCropModeSolutionsGetResult( NcCropModeSolutions solutionSet, unsigned int solutionIndex, float* figureOfMerit, int* startX_min, int* startX_max, int* startY_min, int* startY_max);
NC_FUNCTION int ncCropModeSolutionsGetLocationRanges( NcCropModeSolutions solutionSet, int *offsetX_min, int *offsetX_max, int *offsetY_min, int *offsetY_max);
NC_FUNCTION int ncCropModeSolutionsGetResultAtLocation( NcCropModeSolutions solutionSet, int offsetX, int offsetY, float *figureOfMerit, int *startX_min, int *startX_max, int *startY_min, int *startY_max);
NC_FUNCTION int ncCropModeSolutionsClose( NcCropModeSolutions solutionSet );

NC_FUNCTION int ncCamSetFastKineticsMode(NcCam cam, int imageCount);
NC_FUNCTION int ncCamGetFastKineticsMode(NcCam cam, int *imageCount);
NC_FUNCTION int ncCamSetFastKineticsModeDiscard(NcCam cam, int discardCount);
NC_FUNCTION int ncCamGetFastKineticsModeDiscard(NcCam cam, int * discardCount);
NC_FUNCTION int ncCamGetFastKineticsModeMaxImages(NcCam cam, int *maxImages);
NC_FUNCTION int ncCamGetFastKineticsModeImageCount(NcCam cam, int *imageCount);
NC_FUNCTION int ncCamGetFastKineticsModeImageParams(ImageParams params, int * count, int * discard, int * height, double * shift_ms, double * rate);

NC_FUNCTION int ncCamNbrImagesAcquired(NcCam cam, int *nbrImages);

NC_FUNCTION int ncCamGetControllerCameraLinkMode(NcCam cam, enum CameraLinkMode* CLmode);
NC_FUNCTION int ncCamGetControllerReadoutTaps(NcCam cam, int* taps);
NC_FUNCTION int ncCamGetControllerPixelDepth(NcCam cam, int* bits);
NC_FUNCTION int ncCamGetFrameGrabberBufferSize(NcCam cam, int* width, int* height);
NC_FUNCTION int ncCamGetFrameGrabberDataRegion(NcCam cam, int* offsetX, int* offsetY, int* width, int* height);


NC_FUNCTION int ncCamSetSpecialMode(NcCam cam, enum NcSpecialMode mode, const union NcSpecialModeArguments* arguments);
NC_FUNCTION int ncCamGetSpecialMode(NcCam cam, enum NcSpecialMode * mode, union NcSpecialModeArguments* arguments);


// On demand image processing
NC_FUNCTION int ncCamCreateBias(NcCam cam, int nbrImages, enum ShutterMode biasShuttermode);
NC_FUNCTION int ncCamCancelBiasCreation(NcCam cam);
NC_FUNCTION int ncCamSaveBiasImage(NcCam cam, const char *saveName, int overwriteFlag);
NC_FUNCTION int ncCamSetBiasClampLevel(NcCam cam, int clamp);
NC_FUNCTION int ncCamGetBiasClampLevel(NcCam cam, int * clamp);
NC_FUNCTION int ncCamGetProcType(NcCam cam, int * type, int * nbrImagesPc);
NC_FUNCTION int ncCamSetProcType(NcCam cam, int type, int nbrImagesPc);
NC_FUNCTION int ncCamCreateBiasNewImageCallback(NcCam cam, void (*fct)(NcCam cam, int imageNo, void *data), void *data);

NC_FUNCTION int ncCamStatsAddRegion(NcCam cam, int regionWidth, int regionHeight, int *regionIndex);
NC_FUNCTION int ncCamStatsRemoveRegion(NcCam cam, int regionIndex);
NC_FUNCTION int ncCamStatsResizeRegion(NcCam cam, int regionIndex, int regionWidth, int regionHeight);
NC_FUNCTION int ncCamStatsGetCrossSection(NcCam cam, int regionIndex, NcImage *image, int xCoord, int yCoord, double statsCtxRegion[5], double **histo, double **crossSectionHorizontal, double **crossSectionVertical);
NC_FUNCTION int ncCamStatsGetGaussFit(NcCam cam, int regionIndex, NcImage *image, int xCoord, int yCoord, double *maxAmplitude, double gaussSumHorizontal[3], double gaussSumVertical[3], int useActualCrossSection);


// Status & logging
NC_FUNCTION int ncCamSetOnStatusAlertCallback(NcCam cam, void (*fct)(NcCam cam, void* data, int errorCode, const char * errorString), void * data);
NC_FUNCTION int ncCamSetOnStatusUpdateCallback(NcCam cam, void (*fct)(NcCam cam, void* data), void * data);
NC_FUNCTION int ncCamSetStatusPollRate(NcCam cam, int periodMs);
NC_FUNCTION int ncCamGetStatusPollRate(NcCam cam, int * periodMs);
NC_FUNCTION int ncCamGetSafeShutdownTemperature(NcCam cam, double *safeTemperature, int *dontCare);

NC_FUNCTION int ncCamSilenceFunctionLogging(NcCam cam, const char * func_name, int silenced);


#ifdef NC_ADVANCED
// These functions are provided with no documentation.
// Their use may be suggested for specfic applications.
NC_FUNCTION int ncCamEnableActiveRegion(NcCam cam, int enable);
NC_FUNCTION int ncCamSetEnableFullOverscan(NcCam cam, int overscanXEnable, int overscanYEnable);
NC_FUNCTION int ncCamSetSerialCarTime(NcCam cam, double serialCarTime);
NC_FUNCTION int ncCamGetSerialCarTime(NcCam cam, int cameraRequest, double* serialCarTime);
// NOTE: Previously, some of these declarations and supporting definitions 
//        could be provided via a separate header.
//       Therefore, with the aim of avoiding name collisions ...
#ifndef NC_ADVANCED_H
#define NC_ADVANCED_H
#endif // NC_ADVANCED_H
#endif // NC_ADVANCED


// Standalone image processing
NC_FUNCTION int ncProcOpen(int width, int height, NcProc* procCtx);
NC_FUNCTION int ncProcClose(NcProc ctx);
NC_FUNCTION int ncProcResize (NcProc ctx, int width, int height);

NC_FUNCTION int ncProcAddBiasImage(NcProc ctx, NcImage *bias);
NC_FUNCTION int ncProcComputeBias(NcProc ctx);

NC_FUNCTION int ncProcSetProcType(NcProc ctx, int type);
NC_FUNCTION int ncProcGetProcType(NcProc ctx, int *type);

NC_FUNCTION int ncProcProcessDataImageInPlace(NcProc ctx, unsigned short *image);
NC_FUNCTION int ncProcProcessDataImageInPlaceForceType(NcProc ctx, unsigned short *image, int procType);
/*Deprecated*/NC_FUNCTION int ncProcGetOneImage(NcProc ctx, NcImage **imageOut, const int retrieveImageNumber);
NC_FUNCTION int ncProcGetImage(NcProc ctx, NcImage** image);
NC_FUNCTION int ncProcAddDataImage(NcProc ctx, NcImage *image);
NC_FUNCTION int ncProcReleaseImage(NcProc ctx, NcImage *image);
NC_FUNCTION int ncProcEmptyStack(NcProc ctx);

NC_FUNCTION int ncProcSetBiasClampLevel(NcProc ctx, int biasClampLevel);
NC_FUNCTION int ncProcGetBiasClampLevel(NcProc ctx, int* biasLevel);

NC_FUNCTION int ncProcSetOverscanLines(NcProc ctx, int overscanLines);
NC_FUNCTION int ncProcGetOverscanLines(NcProc ctx, int *overscanLines);

NC_FUNCTION int ncProcSave(NcProc ctx, const char *saveName, int overwriteFlag);
NC_FUNCTION int ncProcLoad(NcProc procCtx, const char *saveName);
NC_FUNCTION int ncProcSaveSetHeaderCallback(NcProc ctx, void (*fct)(NcProc ctx, NcImageSaved *imageFile, void *data), void *data);
NC_FUNCTION int ncProcLoadSetHeaderCallback(NcProc ctx, void (*fct)(NcProc ctx, NcImageSaved *imageFile, void *data), void *data);

//Stats functions
NC_FUNCTION int ncStatsOpen(int imageWidth, int imageHeight, NcStatsCtx** statsCtx);
NC_FUNCTION int ncStatsClose(NcStatsCtx *statsCtx);
NC_FUNCTION int ncStatsResize (NcStatsCtx *statsCtx, int imageWidth, int imageHeight);

NC_FUNCTION int ncStatsAddRegion (NcStatsCtx *statsCtx, int regionWidth, int regionHeight, int *regionIndex);
NC_FUNCTION int ncStatsRemoveRegion (NcStatsCtx *statsCtx, int regionIndex);
NC_FUNCTION int ncStatsResizeRegion (NcStatsCtx *statsCtx, int regionIndex, int regionWidth, int regionHeight);

NC_FUNCTION int ncStatsGetHistoCrossSection (NcStatsCtx *statsCtx, int regionIndex, NcImage *image, int xCoord, int yCoord, double statsCtxRegion[5], double **histo, double **crossSectionHorizontal, double **crossSectionVertical);
NC_FUNCTION int ncStatsGetGaussFit(NcStatsCtx *statsCtx, int regionIndex, NcImage *image, int xCoord, int yCoord, double *maxAmplitude, double gaussSumHorizontal[3], double gaussSumVertical[3], int useActualCrossSectionFlag);


//==========================
// Framegrabber settings functions (via grab context).
NC_FUNCTION int ncGrabParamSupportedInt(NcGrab grab, const char * paramName, int * supported);
NC_FUNCTION int ncGrabParamSupportedDbl(NcGrab grab, const char * paramName, int * supported);
NC_FUNCTION int ncGrabParamSupportedStr(NcGrab grab, const char * paramName, int * supported);
NC_FUNCTION int ncGrabParamSupportedVoidPtr(NcGrab grab, const char * paramName, int * supported);
NC_FUNCTION int ncGrabParamSupportedCallback(NcGrab grab, const char * paramName, int * supported);

NC_FUNCTION int ncGrabParamGetCountInt(NcGrab grab, int * count);
NC_FUNCTION int ncGrabParamGetCountDbl(NcGrab grab, int * count);
NC_FUNCTION int ncGrabParamGetCountStr(NcGrab grab, int * count);
NC_FUNCTION int ncGrabParamGetCountVoidPtr(NcGrab grab, int * count);
NC_FUNCTION int ncGrabParamGetCountCallback(NcGrab grab, int * count);

NC_FUNCTION int ncGrabParamGetNameInt(NcGrab grab, int index, const char ** name);
NC_FUNCTION int ncGrabParamGetNameDbl(NcGrab grab, int index, const char ** name);
NC_FUNCTION int ncGrabParamGetNameStr(NcGrab grab, int index, const char ** name);
NC_FUNCTION int ncGrabParamGetNameVoidPtr(NcGrab grab, int index, const char ** name);
NC_FUNCTION int ncGrabParamGetNameCallback(NcGrab grab, int index, const char ** name);

NC_FUNCTION int ncGrabParamSetInt(NcGrab grab, const char * paramName, int value);
NC_FUNCTION int ncGrabParamSetDbl(NcGrab grab, const char * paramName, double value);
NC_FUNCTION int ncGrabParamSetStr(NcGrab grab, const char * paramName, const char * value);
NC_FUNCTION int ncGrabParamSetVoidPtr(NcGrab grab, const char * paramName, void * value);
NC_FUNCTION int ncGrabParamSetCallback(NcGrab grab, const char * paramName,  void(*callback)(void*), void * data);

NC_FUNCTION int ncGrabParamUnsetInt(NcGrab grab, const char * paramName);
NC_FUNCTION int ncGrabParamUnsetDbl(NcGrab grab, const char * paramName);
NC_FUNCTION int ncGrabParamUnsetStr(NcGrab grab, const char * paramName);
NC_FUNCTION int ncGrabParamUnsetVoidPtr(NcGrab grab, const char * paramName);
NC_FUNCTION int ncGrabParamUnsetCallback(NcGrab grab, const char * paramName);

NC_FUNCTION int ncGrabParamGetInt(NcGrab grab, const char * paramName, int * value);
NC_FUNCTION int ncGrabParamGetDbl(NcGrab grab, const char * paramName, double * value);
NC_FUNCTION int ncGrabParamGetStr(NcGrab grab, const char * paramName, char * outBuffer, int bufferSize);
NC_FUNCTION int ncGrabParamGetStrSize(NcGrab grab, const char * paramName, int * valueSize);
NC_FUNCTION int ncGrabParamGetVoidPtr(NcGrab grab, const char * paramName, void ** value);
NC_FUNCTION int ncGrabParamGetCallback(NcGrab grab, const char * paramName,  void(**callback)(void*), void ** data);


//==========================
// Framegrabber settings functions (via camera context).
NC_FUNCTION int ncCamParamSupportedInt(NcCam cam, const char * paramName, int * supported);
NC_FUNCTION int ncCamParamSupportedDbl(NcCam cam, const char * paramName, int * supported);
NC_FUNCTION int ncCamParamSupportedStr(NcCam cam, const char * paramName, int * supported);
NC_FUNCTION int ncCamParamSupportedVoidPtr(NcCam cam, const char * paramName, int * supported);
NC_FUNCTION int ncCamParamSupportedCallback(NcCam cam, const char * paramName, int * supported);

NC_FUNCTION int ncCamParamGetCountInt(NcCam cam, int * count);
NC_FUNCTION int ncCamParamGetCountDbl(NcCam cam, int * count);
NC_FUNCTION int ncCamParamGetCountStr(NcCam cam, int * count);
NC_FUNCTION int ncCamParamGetCountVoidPtr(NcCam cam, int * count);
NC_FUNCTION int ncCamParamGetCountCallback(NcCam cam, int * count);

NC_FUNCTION int ncCamParamGetNameInt(NcCam cam, int index, const char ** name);
NC_FUNCTION int ncCamParamGetNameDbl(NcCam cam, int index, const char ** name);
NC_FUNCTION int ncCamParamGetNameStr(NcCam cam, int index, const char ** name);
NC_FUNCTION int ncCamParamGetNameVoidPtr(NcCam cam, int index, const char ** name);
NC_FUNCTION int ncCamParamGetNameCallback(NcCam cam, int index, const char ** name);

NC_FUNCTION int ncCamParamSetInt(NcCam cam, const char * paramName, int value);
NC_FUNCTION int ncCamParamSetDbl(NcCam cam, const char * paramName, double value);
NC_FUNCTION int ncCamParamSetStr(NcCam cam, const char * paramName, const char * value);
NC_FUNCTION int ncCamParamSetVoidPtr(NcCam cam, const char * paramName, void * value);
NC_FUNCTION int ncCamParamSetCallback(NcCam cam, const char * paramName,  void(*callback)(void*), void * data);

NC_FUNCTION int ncCamParamUnsetInt(NcCam cam, const char * paramName);
NC_FUNCTION int ncCamParamUnsetDbl(NcCam cam, const char * paramName);
NC_FUNCTION int ncCamParamUnsetStr(NcCam cam, const char * paramName);
NC_FUNCTION int ncCamParamUnsetVoidPtr(NcCam cam, const char * paramName);
NC_FUNCTION int ncCamParamUnsetCallback(NcCam cam, const char * paramName);

NC_FUNCTION int ncCamParamGetInt(NcCam cam, const char * paramName, int * value);
NC_FUNCTION int ncCamParamGetDbl(NcCam cam, const char * paramName, double * value);
NC_FUNCTION int ncCamParamGetStr(NcCam cam, const char * paramName, char * outBuffer, int bufferSize);
NC_FUNCTION int ncCamParamGetStrSize(NcCam cam, const char * paramName, int * valueSize);
NC_FUNCTION int ncCamParamGetVoidPtr(NcCam cam, const char * paramName, void ** value);
NC_FUNCTION int ncCamParamGetCallback(NcCam cam, const char * paramName,  void(**callback)(void*), void ** data);


#ifdef __cplusplus
} /* END extern "C" */
#endif

#endif	// NC_API_H

