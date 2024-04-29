
//  This program will read the temperature diodes through the Lakeshore Controller
//  Make sure Lakeshore sits on the right Serial /dev/ttyUSB0 | ttyUSB1 | ttyUSB2
//  Generally the Ceryocooler connects on ttyUSB0

// Libraries
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/io.h>
#include <math.h>
#include <stdlib.h>

#define MAX_COMMAND_LENGTH	100

	int main(int argc, char *argv[])
  {
	  int temp, n;
	  int port=0;
	  struct termios tty;
	  int speed = B9600;
	  int delay=0;
	  char buffer[MAX_COMMAND_LENGTH];
	  char tmp[255];
	  float temperature;

	  if (argc != 2) {
		printf("Incorrect number of arguments\n");
		printf("Correct usage: ./DewarDiodes <time delay in seconds>\n");
	  }


	delay = atoi(argv[1])*1000;
	port = open("/dev/ttyUSB1", O_RDWR  | O_NONBLOCK | O_NDELAY | O_NOCTTY );


	  if (port < 0) 
	  {
		printf("Error opening port\n");
		return -1;
	  }
	  else 
	  fcntl(port, F_SETFL,0);	

	memset(&tty, 0, sizeof tty);

	  if (tcgetattr (port, &tty) != 0)
	  {
                printf("error %d from tcgetattr", errno);
                return -1;
	  } 
	
        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        // disable IGNBRK for mismatched speed tests; otherwise receive break
        //         // as \000 chars
        tty.c_iflag |= ( IGNBRK  );		// ignore break signal
        tty.c_lflag = 0;			// no signaling chars, no echo,
        tty.c_oflag = 0;                	// no remapping, no delays

        //tty.c_cc[VMIN]  = 0;			// read doesn't block
	//tty.c_cc[VTIME] = 50;			// 0.5 seconds read timeout

        tty.c_cc[VMIN]  = 0;			// read doesn't block
        tty.c_cc[VTIME] = 50;			// 0.5 seconds read timeout

     	tty.c_iflag &= ~(IXON | IXOFF | IXANY);	// shut off xon/xoff ctrl
        tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS7;			// 7-bit chars
	tty.c_cflag |= PARENB;			// enable parity
	tty.c_cflag |= PARODD;			//set parity to odd

	tty.c_cflag |= (CLOCAL | CREAD);	// ignore modem controls,

        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;
	tty.c_cflag |= HUPCL;
	tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	tty.c_oflag &= ~OPOST;			// make raw

	  if (tcsetattr (port, TCSANOW, &tty) != 0)
     	  {
		printf("error %d from tcsetattr", errno);
		return -1;
	  }
	
	memset (&tmp, '\0', sizeof(tmp));	

	sprintf(buffer, "*CLS?\r\n");
	n = write(port, buffer, strlen(buffer));
	  usleep(100000);
	n = read(port, &tmp, 42);
	
	memset (&tmp, '\0', sizeof(tmp));
	
	sprintf(buffer, "CRDG? 0\r\n");
	  usleep(100000);

	n = write(port, buffer, strlen(buffer));		
	printf("Bytes written: %i\n", n);
	  usleep(100000);

	n = read(port, &tmp, 42);	
	printf("temp[%i] --> %s\n",n,tmp);			
	tmp[n-2] = 0;
	printf("temp[%i] --> %s\n",n,tmp);
			
	temperature = strtof(tmp, NULL);
	printf("The temperature is: %f\n",temperature);

	close(port);
	temp = 1;
	return temp;
  }
