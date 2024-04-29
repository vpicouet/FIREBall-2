//Header file that needs to be added to all your projects as all the Nc.. functions and types are defined in there
#include "nc_driver.h"

#include <math.h>
#include <stdio.h>

#ifdef WINDOWS
#include <conio.h>
#endif

int main() 
{
	//Size of the images that will be acquired
	int 		width = 256, height = 256;
	NcGrab 		myNcChannel;
	NcProcCtx * 	ctx = NULL;
	NcImage * 	image;
	NcImage * 	PcImage;
	int 		error, i, nbrFramesInt;

	// NOTE: Use serial commands to set-up the camera controller
	//        with the desired readout mode and exposing time
	//        and start a continuous acquisition
	//        before running this example.
	
	//Opening the frame grabber channel using the 1st channel of the 1st board and 4 loop buffers (recommended)
	error = ncGrabOpen(0, 0, 4, &myNcChannel);
        if (error)
        	goto end;

	//We first set the size of the images the frame grabber should expect
	error = ncGrabSetSize(myNcChannel, width, height);
        if (error)
        	goto end;

	//We open the context we'll be using
	error = ncProcOpen(width, height, &ctx);
        if (error)
        	goto end;
	


	/***Procedure to take the Bias****/

	// Close the shutter as we need to acquire dark images
	//  from which to compute the bias image.
	// NOTE: Ideally these images would have a zero exposing time also.
	printf("\nClose the shutter and then hit any key to acquire for the bias image");
#ifdef WINDOWS
	getch();
#else
	getchar();
#endif

	//Launching continuous acquisitions on the framegrabber (this function doesn't wait for the acquisition to be complete before returning)
	error = ncGrabStart(myNcChannel, 0);
        if (error)
        	goto end;

	// Accumulate Bias images
	for(i=0; i<200; i++) 
	{
		printf("Capturing bias image %d\n", i);
		error = ncGrabRead(myNcChannel, &image);
                if (error)
                	goto end;
		error = ncProcAddBiasImage(ctx, image);
                if (error)
                	goto end;
	}

	//We use the "abort" function as we want to tell our frame grabber to stop acquiring frames from the 
	//camera as we already have all the required images to calculate the Bias image
	error = ncGrabAbort(myNcChannel);
        if (error)
        	goto end;
	
	error = ncProcComputeBias(ctx);
        if (error)
        	goto end;
	printf("Bias image calculated\n");

	// Open the shutter to capture an image with some integrated light
	// NOTE: If zero exposure was set for the bias acquisition, you would restore the required exposure time here
	printf("\nOpen the shutter and then hit any key to acquire images to be processed");
#ifdef WINDOWS
	getch();
#else
	getchar();
#endif

	//Launching 1 acquisition on the framegrabber (this function doesn't wait for the acquisition to be complete before returning)
	error = ncGrabStart(myNcChannel, 1);
        if (error)
        	goto end;

	//Reading the image received 
	 error = ncGrabRead(myNcChannel, &image);
         if (error)
         	goto end;

	 //We save an un-corrected image to FITS
	//There's no need to specify a width and height, as the function will take the ones defined in the NcGrab structure
	error = ncGrabSaveImage(myNcChannel, image, "ncImage", FITS, 1);
        if (error)
        	goto end;



	/***Procedure to apply image correction****/

	//We set the clamp level to 300 (standard value)
	error = ncProcSetBiasClampLevel(ctx, 300);
        if (error)
        	goto end;
	
	//We set the processing type to LM (this means the bias image will be subtracted from the image capture)
	error = ncProcSetProcType(ctx, LM);
        if (error)
        	goto end;
	
	//Launching 1 acquisition on the framegrabber and requesting one image from the camera (this function 
	//doesn't wait for the acquisition to be complete before returning)
	error = ncGrabStart(myNcChannel, 1);
        if (error)
        	goto end;

	//Reading the image received 
	 error = ncGrabRead(myNcChannel, &image);
         if (error)
         	goto end;

	//Using this function the image captured is being corrected at the same address and now have an offset of the clamp level (300)
	error = ncProcProcessDataImageInPlace(ctx, image);
        if (error)
        	goto end;

	//We save the corrected image in FITS
	//There's no need to specify a width and height, as the function will take the ones defined in the NcGrab structure
	error = ncGrabSaveImage(myNcChannel, image, "ncAmImage", FITS, 1);
        if (error)
        	goto end;



	/***Procedure to save the processing context****/
	error = ncProcSave(ctx, "ncContext", 1);
        if (error)
        	goto end;



	/***Procedure to apply photon counting****/

	//We set the processing type to PC (Photon Counting)
	error = ncProcSetProcType(ctx, PC);
        if (error)
        	goto end;

	//We set the number of frames we want to integrate in PC
	nbrFramesInt = 100;

	//Launching 1 acquisition on the framegrabber (this function doesn't wait for the acquisition to be complete before returning)
	error = ncGrabStart(myNcChannel, nbrFramesInt);
        if (error)
        	goto end;

	for (i = 0; i<nbrFramesInt; i++)
	{
		error = ncGrabRead(myNcChannel, &image);
                if (error)
                	goto end;
		error = ncProcAddDataImage(ctx, image);
                if (error)
                	goto end;
	}

	//Recover the PC-processed image
	error = ncProcGetImage(ctx, &PcImage);
        if (error)
        	goto end;

	//We save the PC image in FITS
	//There's no need to specify a width and height, as the function will take the ones defined in the NcGrab structure
	error = ncGrabSaveImage(myNcChannel, PcImage, "ncPcImage", FITS, 1);
        if (error)
        	goto end;

	//We can now free this image as it will no longer be used
	error = ncProcReleaseImage(ctx, PcImage);
        if (error)
        	goto end;



	/***Procedure to load the processing context****/
	
	//As the loading function also opens a new context (ctx) we'll close the current one
	error = ncProcClose(ctx);
        if (error)
        	goto end;

	error = ncProcLoad(ctx, "ncContext");
        if (error)
        	goto end;

	//Launching 1 acquisition on the framegrabber (this function doesn't wait for the acquisition to be complete before returning)
	error = ncGrabStart(myNcChannel, 1);
        if (error)
        	goto end;

	//Reading the image received 
	 error = ncGrabRead(myNcChannel, &image);
         if (error)
         	goto end;

	//Using this function the image captured is being corrected at the same address and now have an offset of the clamp level (300)
	error = ncProcProcessDataImageInPlace(ctx, image);
        if (error)
        	goto end;

	//We save the corrected image in FITS
	//There's no need to specify a width and height, as the function will take the ones defined in the NcGrab structure
	error = ncGrabSaveImage(myNcChannel, image, "nc2ndAmImageWithCtxLoaded", FITS, 1);
        if (error)
        	goto end;




	error = ncProcClose(ctx);
        if (error)
        	goto end;

	//We close the frame grabber channel as we're not looking to use it anymore
	error = ncGrabClose(myNcChannel);
        if (error)
        	goto end;

end:
	if (error)
		printf ("The error %d happened during the example.\nFor more information about this error, the file nc_error.h can be used\n", error);
	else
		printf ("The program finished without errors\n");
#ifdef WINDOWS
	printf("\nBy hitting any key the cmd window will be closed");
	getch();
#endif

	return 0;
}
