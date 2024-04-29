//Header file that need to be added to all your projects as all the the Nc functions are define in there
#include "nc_driver.h"

#include <stdio.h>

#ifdef WINDOWS
#include <conio.h>
#include <windows.h>
#else
#include <curses.h>
#include <unistd.h>
#endif

int main()
{
	NcGrab		myNcChannel;
	NcImage		*myNcImage;

	int		nbrTimeouts = 0, newNbrTimouts = 0, nbrImagesSkipped;
	char		imageName[31];
	int		i = 0, error;

	// NOTE: Use serial commands to set-up the camera controller
	//        with the desired readout mode and exposing time
	//        and start a continuous acquisition
	//        before running this example.


	//Opening the frame grabber channel using the 1st channel of the 1st board and 4 loop buffers (recommended)
	error = ncGrabOpen(NC_AUTO_UNIT, NC_AUTO_CHANNEL, 4, &myNcChannel);
	if (error)
		goto end;

	//We first say the size of the images the frame grabber is expecting
	error = ncGrabSetSize(myNcChannel, 128, 128);
	if (error)
		goto end;
	//We modify the timeout value to wait for "one second".  This means each image have up to one second to
	//arrive to the frame grabber prior to indicate a timeout.
	error = ncGrabSetTimeout(myNcChannel, 1000);
	if (error)
		goto end;

	printf("Press any key to start a sequence of 7 images acquisition. As we simulate a long processing time, some buffers (the 2nd and 3rd one) of your loop buffers will be overwritten.\n\n");
	printf("The ncGrabReadChronological function will step over these frames to read the oldest of the loop buffers\n\n");
	printf("Please press Enter to continue\n");
#ifdef WINDOWS
	getch();
#else
	getchar();
#endif

	//Launching 7 acquisitions on the framegrabber (this function doesn't wait for the acquisition to be complete before returning)
	//We assume the camera has been set to send 7 frames at 1fps
	error = ncGrabStart(myNcChannel, 7);
	if (error)
		goto end;

	while (nbrTimeouts == 0)
	{
		//Reading the first image of the loop buffer
		error = ncGrabReadChronological(myNcChannel, &myNcImage, &nbrImagesSkipped);
		if (error == NC_ERROR_GRAB_TIMEOUT)
			break;
		else if (error)
			goto end;

		//If the total number of timeout is the same as it was was previously, it means the image read
		//is valid
		error = ncGrabGetNbrTimeout(myNcChannel, &newNbrTimouts);
		if (error)
			goto end;

		if (nbrTimeouts == newNbrTimouts)
		{
			//If you wish to proceed with some processing on each image read, it should be done over here
			//(in this example we'll only save the image)

			//We save each image acquired, at the end of each name the loop index will be added
			sprintf(imageName, "Image %d", i);
			//We save the image acquired
			error = ncGrabSaveImage(myNcChannel, myNcImage, imageName, FITS, 1);
			if (error)
				goto end;

			i++;
			printf ("Number of images skipped to get the %d image: %d\n", i, nbrImagesSkipped);

			// ********
			//For the purpose of this example we'll simulate a processing time that is longer then the
			//acquisition rate (of course it depends of the frame rate you currently have set on your camera).
			//The application will be sleeping for 1000ms here.  This should make the images coming from
			//the camera overwrite the previous ones in your loop buffer.
#ifdef WINDOWS
			Sleep(1000);
#else
			sleep(1);
#endif
		}
		else
		{
			//We normally don't want to do any processing if the image read isn't valid but if you want your
			//application to do so, it should be done over here.

			// We keep track of the number of timeouts
			nbrTimeouts = newNbrTimouts;
		}
	}

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
