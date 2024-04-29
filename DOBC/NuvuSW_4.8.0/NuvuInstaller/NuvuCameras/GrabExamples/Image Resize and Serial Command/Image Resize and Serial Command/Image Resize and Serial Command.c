//Header file that need to be added to all your projects as all the the Nc functions are define in there
#include "nc_driver.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef WINDOWS
#include <conio.h>
#endif

int main()
{
	NcGrab 		myNcChannel;
	NcImage * 	myNcImage;

	int	       	error;
	int 		nbrTimeouts = 0;
	int 		replyBytesReceived = 0, replyBytesToRead = 0;
	int 		width, height;

	const int 	baudrate = 115200;
	const char 	exposureCommand[] = "se 40\n"; // Sets 40-ms exposure time 
	unsigned int 	commandLength = sizeof(exposureCommand); // NOTE: non-const, see use below.
	enum NcSerialProtocol protocol = NC_SERIAL_LIVE; // The default uses the active protocol 
	char * 		commandDispatch;

	char 		serialCommandRecBuffer[512];
	const int   serialReplyBufferSize = sizeof(serialCommandRecBuffer) / sizeof(*serialCommandRecBuffer);
	char *		serialCommandRecBufferPtr = serialCommandRecBuffer;
	int 	replyTimeout = 1000; // [ms]

	//Opening the frame grabber channel using the 1st channel of the 1st board and 4 loop buffers (recommended)
	error = ncGrabOpen(NC_AUTO_UNIT, NC_AUTO_CHANNEL, 4, &myNcChannel);
	if (error)
		goto end;

	//We modify the size of the image expected by the host.  
	//Please make sure your CCD is sending at least that amount of data, otherwise a timeout will happen.
	error = ncGrabSetSize(myNcChannel, 128, 128);
	if (error)
		goto end;

	error = ncGrabGetSize(myNcChannel, &width, &height);
	if (error)
		goto end;

	//We modify the timeout value to wait for "one second".  This means each image will have up to one second to
	//arrive at the frame grabber prior to flagging a timeout.
	error = ncGrabSetTimeout(myNcChannel, 1000);
	if (error)
		goto end;

	printf("The new size of my buffers that are acquiring the images is of %d x %d\n\n", width, height);
	printf("If the camera isn't sending an image of at least that size, a timeout will happen when trying to read the buffers.\n\n");
	printf("Please press Enter to continue\n");

#ifdef windows
    getch();
#else
    getchar();
#endif

	//Launching 1 acquisition on the framegrabber (this function doesn't wait for the acquisition to be
	//complete before returning)
	error = ncGrabStart(myNcChannel, 1);
	if (error)
		goto end;

	//Reading the image received
	error = ncGrabRead(myNcChannel, &myNcImage);
	if (error)
		goto end;

	//We check if we received a timeout for our first image acquired
	error = ncGrabGetNbrTimeout(myNcChannel, &nbrTimeouts);
	if (error)
		goto end;

	//If the image is valid (no timeout happen on it), we save the image we just acquired
	if (nbrTimeouts == 0)
	{
		error = ncGrabSaveImage(myNcChannel, myNcImage, "NcImage", FITS, 1);
		if (error)
			goto end;
	}

	printf("You just acquired one image (if no timeout happen)\n\n");
	printf("By pressing Enter you will modify the exposing time of your camera to 40 ms\n");
#ifdef WINDOWS
	getch();
#else
	getchar();
#endif


	//Setting the serial communication speed to use with your camera
	error = ncGrabSetBaudrate (myNcChannel, baudrate);
	if (error)
		goto end;


	// If we've been able to modify the serial communication speed, 
	//  we proceed to send the "se" (set exposure) command to the camera.
	// But, first we get a version of the command that is 
	//  formatted appropriately for the camera to receive it.

	// Query the length of the re-formatted command:
	//  we pass the current length and it is adjusted to the final value
	error = ncGrabFormatSerialCommand(myNcChannel, protocol, exposureCommand, &commandLength, NULL);
	if (error)
		goto end;
	
	// Prepare a buffer to accept the re-formatted command
	commandDispatch = malloc(commandLength);

	commandLength = sizeof(exposureCommand);
	error = ncGrabFormatSerialCommand(myNcChannel, protocol, exposureCommand, &commandLength, commandDispatch);
	if (error)
		goto end;
	
	// Send the command
	error = ncGrabSendSerialBinaryComm (myNcChannel, commandDispatch, commandLength);
	if (error)
		goto end;
	
	// Done with the re-formatted command buffer
	free(commandDispatch);


	// Now we receive the reply to this command

	// We wait long enough for the serial reply buffer to be filled:
	//  for the set exposure command the response will be fast and short,
	//  so this timeout should be very generous.
	replyTimeout = ( commandLength + serialReplyBufferSize ) / ( baudrate / 1000 );
	error = ncGrabSetSerialTimeout (myNcChannel, replyTimeout);
	if (error)
		return error;


	//We wait for 50 characters to come back from the camera 
	// otherwise you'll wait for the serial timeout to happen
	error = ncGrabWaitSerialCmd(myNcChannel, 50, &replyBytesReceived);
	if (error)
		goto end;

	replyBytesToRead = ((size_t)replyBytesReceived > serialReplyBufferSize) ? serialReplyBufferSize : (size_t)replyBytesReceived; 
	replyBytesToRead -= 1; // Allow for NUL-termination

	//We put the results coming back from the camera into a buffer so we can use it
	error = ncGrabRecSerial (myNcChannel, serialCommandRecBufferPtr, replyBytesToRead, &replyBytesReceived);
	if (error)
		goto end;

	serialCommandRecBuffer[(size_t)replyBytesToRead] = '\0';

	printf("%s", serialCommandRecBuffer);

	printf("\n\nYou just modified the exposure time of your camera (if you communicated properly with your camera)\n\n");
	printf("By pressing Enter you will acquire a new image using that new exposure time\n");

#ifdef WINDOWS
    getch();
#else
    getchar();
#endif

	//Launching 1 acquisition on the framegrabber (this function doesn't wait for the acquisition to be
	//complete before returning)
	error = ncGrabStart(myNcChannel, 1);
	if (error)
		goto end;

	//Reading the image received
	error = ncGrabRead(myNcChannel, &myNcImage);
	if (error)
		goto end;

	//We check if we received a timeout for our first image grabbed
	error = ncGrabGetNbrTimeout(myNcChannel, &nbrTimeouts);
	if (error)
		goto end;

	//I fthe image is error (no timeout happen on it), we save the image we just acquired
	if (nbrTimeouts ==0)
	{
		error = ncGrabSaveImage(myNcChannel, myNcImage, "SecondNcImage", FITS, 1);
		if (error)
			goto end;
	}


end:
	if (error)
		printf ("The error %d happened during the example.\nFor more information about this error, refer to the file nc_error.h\n", error);

	printf ("The grab context will now be closed\n");

	//We close the frame grabber channel as we<re not looking to use it anymore
	error = ncGrabClose(myNcChannel);
	if (error)
		goto end;

	if (error)
		printf ("The error %d happened while closing the grab context.\nFor more information about this error, refer to the file nc_error.h\n", error);

#ifdef WINDOWS
	printf("\nBy hitting any key the cmd window will be closed");
	getch();
#endif

	return 0;
}
