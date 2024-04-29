
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include "socketid.h"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main( int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    

    char buffer[256];
  int len;

  if( argc < 2 || argc > 3) { 
    printf("Invalid number of arguments.\n");
    exit(-1);
  };

  if(argc == 2) sprintf(buffer,"%s",argv[1]);
  if(argc == 3) sprintf(buffer,"%s %s",argv[1],argv[2]);

  portno = CAM_PORT;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(CAM_HOST);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting to camera server");
    len = strlen(buffer);
    //    buffer[len-1]='\0';
    n = write(sockfd,buffer,len);
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);
    close(sockfd);
    return 0;
}

