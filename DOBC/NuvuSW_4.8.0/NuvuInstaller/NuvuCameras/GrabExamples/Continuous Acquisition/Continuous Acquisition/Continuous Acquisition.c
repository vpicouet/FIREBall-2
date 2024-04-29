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
	NcImage		*myNcImage;

	int			nbrTimeouts = 0, newNbrTimouts = 0, overrun = 0, totalNbrImages = 0;
	char		imageName[31];
	int			i =0, error;


	//Opening the frame grabber channel using the 1st channel of the 1st board and 4 loop buffers (recommended)
	error = ncGrabOpen(NC_AUTO_UNIT, NC_AUTO_CHANNEL, 4, &myNcChannel);
	if (error)
		goto end;

    error = ncGrabSetSize(myNcChannel, 128, 128);
	if (error)
		goto end;

	//We modify the timeout value to wait for "one second".  This means each image have up to one second to
	//arrive to the frame grabber prior to indicate a timeout.
	error = ncGrabSetTimeout(myNcChannel, 1000);
	if (error)
		goto end;


#ifdef WINDOWS
	printf("Press any key to start a continuous acquisition. The images will be saved in a loop of images from 0 to 9\n");
	getch();
#else
	printf("Enter the number of images to acquire \n");
	int numberImages;
	scanf("%d", &numberImages);
	int cont=0;
#endif

	//Launching continuous acquisitions on the framegrabber (this function doesn't wait for the acquisition to
	//be complete before returning)
	error = ncGrabStart(myNcChannel, 0);
	if (error)
		goto end;

	//Loop in which we read each images acquired
	do
	{
		//Reading the image received
		error = ncGrabRead(myNcChannel, &myNcImage);
		if (error)
			goto end;

		//We check if an overrun occurred on the last image (meaning that the buffer we're reading as been
		//overwritten once prior to have this "read" call being done)
		error = ncGrabGetOverrun(myNcChannel, &overrun);
		if (error)
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

			i = i+1;
			if (i ==10)
				i=0;

			totalNbrImages = totalNbrImages +1;
			printf ("%d", totalNbrImages);
			printf("\r");
			fflush(stdout);
		}
		else
		{
			//We normally don't want to do any processing if the image read isn't valid but if you want your
			//application to do so, it should be done over here.

			//As the number of timeouts changed we want to adjust our variable nbrTimeouts according to this
			error = ncGrabGetNbrTimeout(myNcChannel, &nbrTimeouts );
			if (error)
				goto end;
		}
	}
#ifdef WINDOWS
	while (!kbhit());
#else
	while(++cont<numberImages);
#endif

//We use the "abort" function as we want to tell our frame grabber to stop acquiring frames from the camera
	error = ncGrabAbort(myNcChannel);
	if (error)
		goto end;

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
	printf("By hitting any key the cmd window will be closed");
    getch();
#endif

	return 0;
}
