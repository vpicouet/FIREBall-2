#ifndef NC_TYPES_H
#define NC_TYPES_H

#include "nc_platform.h"
#include "nc_error.h"
#include "nc_shared_types.h"

typedef struct NcCamHandle * NcCam;
typedef struct NcGrabHandle * NcGrab;
typedef struct NcImagParamHandle * ImageParams;
typedef struct NcProcHandle * NcProc;
typedef struct NcCtrlListHandle * NcCtrlList;
typedef struct NcCropModeSolutionsHandle * NcCropModeSolutions;

// For compatibility with existing code
typedef struct NcProcHandle NcProcCtx;
typedef struct NcImageSavedHandle NcImageSaved;

#endif	// NC_TYPES_H
