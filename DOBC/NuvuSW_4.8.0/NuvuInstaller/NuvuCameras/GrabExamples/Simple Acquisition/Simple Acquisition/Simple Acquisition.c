//Header file that needs to be added to all your projects as all the the Nc functions are declared there
#include "nc_driver.h"
#include <stdio.h>

#ifdef WINDOWS
#include <conio.h>
#else
#include <unistd.h>
#endif

int main()
{

	NcGrab		myNcChannel;
	NcImage	*	myNcImage;

	int 		ccdSizeX = 128; // Smallest format supplied by Nuvu
	int 		ccdSizeY = 128; // Smallest format supplied by Nuvu

	// NOTE: To cope with latency, the framegrabber timeout should be set to 
	//        more than the readout time reported by the controller
	//        plus the exposure time and the waiting (blanking) time.
	//       Here we choose one second which is generous for a typical EM readout mode.
	// 	 This means each image will have up to one second to
	// 	  arrive at the frame grabber prior to the SDK flagging a timeout.
	const int 	acquisitionTimeout = 1000; // [ms]
	int		overrun = 0, newNbrTimouts = 0, nbrTimeouts = 0;
	char		imageName[31];
	int		i = 0, error;


	// NOTE: Use serial commands to set-up the camera controller
	//        with the desired readout mode and exposing time
	//        and start a continuous acquisition
	//        before running this example.

	// Opening the frame grabber channel using the 1st channel of the 1st board and 4 loop buffers (recommended)
	error = ncGrabOpen(NC_AUTO_UNIT, NC_AUTO_CHANNEL, 4, &myNcChannel);
	if (error)
		goto end;

	// The following setting is relevant only to the use of Pleora (Ethernet-based) frame grabbers
	error = ncGrabSetHeartbeat( myNcChannel, 20000);
        if (error)
        	goto end;

	// The default readout mode will typically read the full CCD size
	error = ncGrabSetSize(myNcChannel, ccdSizeX, ccdSizeY);
	if (error)
		goto end;

	// The framegrabber timeout should be set to more than the sum of waiting, exposure and readout time
	error = ncGrabSetTimeout(myNcChannel, acquisitionTimeout );
	if (error)
		goto end;

	// Launching 1 acquisition on the framegrabber (this function doesn't wait for the acquisition to be
	// complete before returning)
	error = ncGrabStart(myNcChannel, 1);
	if (error)
		goto end;

	// Read the received image 
	error = ncGrabRead(myNcChannel, &myNcImage);
	if (error && error != NC_ERROR_GRAB_TIMEOUT) // We will recover the number of timeouts below
		goto end;

	// We check if we received a timeout for our first image grabbed
	error = ncGrabGetNbrTimeout(myNcChannel, &nbrTimeouts);
	if (error)
		goto end;

	// If the image is valid (no timeout occurred), we save the image we just acquired
	if (nbrTimeouts == 0)
	{
		printf("You just acquired one image.\n");
		//We save the image acquired
		error = ncGrabSaveImage(myNcChannel, myNcImage, "FirstImage", FITS, 1);
		if (error)
			goto end;
	}
	else
	{
		printf("A timeout occurred while acquiring.\n");
	}

	printf("By pressing Enter you will acquire 10 new images\n");

#ifdef WINDOWS
	getch();
#else
	getchar();
#endif

	// Launching 10 acquisitions on the framegrabber (this function doesn't wait for the acquisition to be
	// complete before returning)
	error = ncGrabStart(myNcChannel, 10);
	if (error)
		goto end;

	//Loop in which we read each images acquired
	for ( i = 0; i < 10; ++i)
	{
		//Reading the image received
		error = ncGrabRead(myNcChannel, &myNcImage);
		if (error && error != NC_ERROR_GRAB_TIMEOUT) // We will recover the number of timeouts below
			goto end;

		//We check if an overrun occurred on the last image (meaning that the buffer we're reading as been
		//overwritten once prior to have this "read" call being done)
		error = ncGrabGetOverrun(myNcChannel, &overrun);
		if (error)
			goto end;
		
		if ( overrun != 0 )
			printf("An overrun occurred while acquiring image %d.\n", i);

		//If the total number of timeouts is the same as it was was previously, it means the image read
		//is valid
		error = ncGrabGetNbrTimeout(myNcChannel, &newNbrTimouts);
		if (error)
			goto end;

		if (nbrTimeouts == newNbrTimouts)
		{
			printf("You just acquired image %d.\n", i);
			//If you wish to proceed with some processing on each image read, it should be done over here
			//(in this example we'll only save the image)

			//We save each image acquired, at the end of each name the loop index will be added
			sprintf(imageName, "Image %d", i);
		//	error = ncGrabSaveImage(myNcChannel, myNcImage, imageName, FITS, 1);
			if (error)
				goto end;
		}
		else
		{
			printf("A timeout occurred while acquiring image %d.\n", i);
			//We normally don't want to do any processing if the image read isn't valid but if you want your
			//application to do so, it should be done over here.

			//We keep track of the number of timeouts
			nbrTimeouts = newNbrTimouts;
		}
	}

	//We close the frame grabber channel as we have finished with it.
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
