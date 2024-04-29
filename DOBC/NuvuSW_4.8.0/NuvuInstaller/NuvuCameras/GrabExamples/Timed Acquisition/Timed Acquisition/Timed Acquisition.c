//Header file that need to be added to all your projects as all the the Nc functions are define in there
#include "nc_driver.h"
#include <stdio.h>

#ifdef WINDOWS
#include <conio.h>
#else
#include <unistd.h>
#endif

void main()
{
	NcGrab		myNcChannel;
	NcImage	* 	myNcImage;

	int			overrun = 0, nbrTimeouts = 0, newNbrTimouts = 0;
	char		imageName[31];
	int			i = 0, error;
	double	ncImageTime;


	//Opening the frame grabber channel using the 1st channel of the 1st board and 4 loop buffers (recommended)
	error = ncGrabOpen(NC_AUTO_UNIT, NC_AUTO_CHANNEL, 4, &myNcChannel);
	if (error)
		goto end;

    error = ncGrabSetSize(myNcChannel, 128, 128);
	if (error)
		goto end;

	printf("The camera just finished its initialization, we'll now reset the timer and grab a 'timed' image.\n");
	printf("Please press Enter to continue.\n");
#ifdef WINDOWS
	getch();
#else
	getchar();
#endif

	error = ncGrabResetTimer(myNcChannel, 0);
	if (error)
		goto end;

	//Launching 1 acquisition on the framegrabber and requesting one image from the camera (this function
	//doesn't wait for the acquisition to be complete before returning)
	error = ncGrabStart(myNcChannel, 1);
	if (error)
		goto end;

	//Reading the image received
	error = ncGrabReadTimed(myNcChannel, &myNcImage, &ncImageTime);
	if (error && error != NC_ERROR_GRAB_TIMEOUT) // We will recover the number of timeouts below
		goto end;

	//We check if we received a timeout for our first image grabbed
	error = ncGrabGetNbrTimeout(myNcChannel, &nbrTimeouts );
	if (error)
		goto end;

	//If the image is valid (no timeout happen on it), we save the image we just acquired
	if (nbrTimeouts ==0)
	{
		printf("The exact time at which the image was placed in host memory is %fms after the reset of the timer (done just before requesting an image from the camera).\n\n", ncImageTime);
		error = ncGrabSaveImage(myNcChannel, myNcImage, "FirstImage", FITS, 1);
		if (error)
			goto end;
	}

	printf("You just acquired one image (if no timeout happen), by pressing Enter you will acquired 15 new images\n");
#ifdef WINDOWS
    getch();
#else
	getchar();
#endif

	//Launching 15 acquisitions on the framegrabber and requesting 15 images from the camera (this function
	//doesn't wait for the acquisition to be complete before returning)
	error = ncGrabStart(myNcChannel, 15);
	if (error)
		goto end;

	//Loop in which we read each images acquired
	for (i=0; i<15; i++)
	{
		//Reading the image received
		error = ncGrabReadTimed(myNcChannel, &myNcImage, &ncImageTime);
		if (error && error != NC_ERROR_GRAB_TIMEOUT ) // We will recover the number of timeouts below
			goto end;

		//We check if an overrun occurred on the last image (meaning that the buffer we're reading as been
		//overwritten once prior to have this "read" call being done)
		error = ncGrabGetOverrun(myNcChannel, &overrun);
		if (error)
			goto end;

		//If the total number of timeout is the same as it was previously, it means the image read
		//is valid
		error = ncGrabGetNbrTimeout(myNcChannel, &newNbrTimouts);
		if (error)
			goto end;

		if (nbrTimeouts == newNbrTimouts)
		{
			printf("The exact time at which the image was placed in host memory is %fms after the reset of the timer (done just before requesting an image from the camera).\n\n", ncImageTime);

			//If you wish to proceed with some processing on each image read, it should be done over here
			//(in this example we'll only save the image)

			//We save each image acquired, at the end of each name the loop index will be added
			sprintf(imageName, "Image %d", i);
			//We save the image acquired
			error = ncGrabSaveImage(myNcChannel, myNcImage, imageName, FITS, 1);
			if (error)
				goto end;
		}
		else
		{
			//We normally don't want to do any processing if the image read isn't valid but if you want your
			//application to do so, it should be done over here.

			//As the number of timeouts changed we want to adjust our variable nbrTimeouts according to this
			error = ncGrabGetNbrTimeout(myNcChannel, &nbrTimeouts);
			if (error)
				goto end;
		}
	}

	//We close the frame grabber channel as we<re not looking to use it anymore
	error = ncGrabClose(myNcChannel);
	if (error)
		goto end;

end:
	if (error)
		printf ("The error %d happened during the example.\nFor more information about this error, the file nc_error.h can be used\n", error);
	else
		printf ("The program finished without errors\n");

#ifdef WINDOWS
    printf("\nBy hitting Enter the cmd window will be closed");
	getch();
#endif

	return 0;
}
