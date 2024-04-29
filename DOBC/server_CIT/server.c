#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include "camera.h"
#include "server.h"
#include "socketid.h"

const char helpstr[]=" \
\nCommands:\
\ncdsoffset [int]\
\nxsize [int]\
\nysize [int]\
\nburst [int]\
\nemgain [int]\
\nvss [int]\
\nexit\
\nexpose\
\ndark\
\nbias\
\nclose_shutter\
\nopen_shutter\
\nexptime [float]\
\nshutterdelay [int]\
\nhelp\
\nimno [int]\
\nlast\
\nnext\
\npath [validpath]\
\nroot [string]\
\ntrigger [int]\
\nextshutter [int]\
\nshutter [int]\n";



void error(const char *msg)
{
    perror(msg);
}

// variables needed for sockets
int sockfd, newsockfd, portno;
socklen_t clilen;
char buffer[256], response[2048];
int resplen, n;
int port_open = 0;
struct sockaddr_in serv_addr, cli_addr;
int argct, retval;


int open_server(){
  // make a socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    error("ERROR opening socket");
    return -1;
  };

  // set options and clear values
  struct linger lo = { 1, 0 };
  setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &lo, sizeof(lo));
  bzero((char *) &serv_addr, sizeof(serv_addr));

  portno = CAM_PORT;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr *) &serv_addr,
	   sizeof(serv_addr)) < 0){
    error("ERROR on binding");
    return -1;
  };
  
  // set up listening.
  listen(sockfd,5);
  clilen = sizeof(cli_addr);

  port_open = 1;
  printf("Server opened.\n");
  return 0;
};

int close_server(){
  if(port_open){
    close(sockfd);
    printf("Server closed\n");
    port_open = 0;
  }; // port_open
  return 0;
};

int listen_server(){
  char *cmd;
  char *arg;
  int argc;
  int res,val;
  float fval;
  if(port_open){
    newsockfd = accept(sockfd, 
		       (struct sockaddr *) &cli_addr, 
		       &clilen);
    if (newsockfd < 0) {
      error("ERROR on accept");
      return 0;
    };
    bzero(buffer,256);
    n = read(newsockfd,buffer,255);
    if (n < 0){
      error("ERROR reading from socket");
      return 0;
    };
    printf("Received command: %s.\n",buffer);
    printf("%d\n",strcmp(buffer,"exit"));

    // *********************************************
    // OK. Here is where we process commands
    // set up the defaults;
    argc = 0;
    retval = 1;
    resplen=sprintf(response,"Invalid command.");

    cmd = strtok(buffer," =");
    // if the command is null, we're in trouble
    //   printf("Command [%d] %s\n",strlen(cmd),cmd);
    
    if(cmd != NULL){
      // see if there is an argument
      arg = strtok(NULL," ");
      printf("Command  %s\n",cmd);
      if(arg != NULL) printf("Argument %s\n",arg);
      
      if(arg!=NULL) argc=1;
      // exit command received
      if( strcmp(cmd,"exit")==0){
	retval = 0;
	resplen = sprintf(response,"Exit requested.");
      };

      // help
      if( strcmp(cmd,"help")==0){
	resplen = sprintf(response,"%s",helpstr);
      };

      // set/get image number
      if (strcmp(cmd,"imno")==0){
	  if(argc == 1){
	    val = atoi(arg);
	    res = set_exposure_number(val);
	    if(res){
	      resplen = sprintf(response,"Error setting image number.");
	    } else { 
	      resplen = sprintf(response,"%d",val);
	    };
	  } else {
	    res = get_exposure_number(&val);
	    if (res){
	      resplen = sprintf(response,"Error getting image number.");
	    } else {
	      resplen = sprintf(response,"%d",val);
	    };
	  }; //argument list
	};// set image number

      // set/get xsize for array
      if (strcmp(cmd,"xsize")==0){
	  if(argc == 1){
	    val = atoi(arg);
	    res = set_xsize(val);
	    if(res){
	      resplen = sprintf(response,"Error setting x array size.");
	    } else { 
	      resplen = sprintf(response,"%d",val);
	    };
	  } else {
	    res = get_xsize(&val);
	    if (res){
	      resplen = sprintf(response,"Error getting x array size.");
	    } else {
	      resplen = sprintf(response,"%d",val);
	    };
	  }; //argument list
	};// set xsize for array

      // set/get ysize for array
      if (strcmp(cmd,"ysize")==0){
	  if(argc == 1){
	    val = atoi(arg);
	    res = set_ysize(val);
	    if(res){
	      resplen = sprintf(response,"Error setting y array size.");
	    } else { 
	      resplen = sprintf(response,"%d",val);
	    };
	  } else {
	    res = get_ysize(&val);
	    if (res){
	      resplen = sprintf(response,"Error getting y array size.");
	    } else {
	      resplen = sprintf(response,"%d",val);
	    };
	  }; //argument list
	};// set ysize for array
    
    
      // set/get image burst
	if (strcmp(cmd,"burst")==0){
	  if(argc == 1){
	    val = atoi(arg);
	    res = set_exposure_burst(val);
	    if(res){
	      resplen = sprintf(response,"Error setting image burst.");
	    } else { 
	      resplen = sprintf(response,"%d",val);
	    };
	  } else {
	    res = get_exposure_burst(&val);
	    if (res){
	      resplen = sprintf(response,"Error getting image burst.");
	    } else {
	      resplen = sprintf(response,"%d",val);
	    };
	  }; // argument list
	}; // set image burst
	
      // set/get emgain
      if (strcmp(cmd,"emgain")==0){
	  if(argc == 1){
	    val = atoi(arg);
	    res = set_emgain(val);
	    if(res){
	      resplen = sprintf(response,"Error setting emgain.");
	    } else { 
	      resplen = sprintf(response,"%d",val);
	    };
	  } else {
	    res = get_emgain(&val);
	    if (res){
	      resplen = sprintf(response,"Error getting emgain.");
	    } else {
	      resplen = sprintf(response,"%d",val);
	    };
	  }; //argument list
      };// set emgain

      // set/get vss
      if (strcmp(cmd,"vss")==0){
	  if(argc == 1){
	    val = atoi(arg);
	    res = set_vss(val);
	    if(res){
	      resplen = sprintf(response,"Error setting vss.");
	    } else { 
	      resplen = sprintf(response,"%d",val);
	    };
	  } else {
	    res = get_vss(&val);
	    if (res){
	      resplen = sprintf(response,"Error getting vss.");
	    } else {
	      resplen = sprintf(response,"%d",val);
	    };
	  }; //argument list
      };// set vss

      // set/get cdsoffset
      if (strcmp(cmd,"cdsoffset")==0){
	  if(argc == 1){
	    val = atoi(arg);
	    res = set_cdsoffset(val);
	    if(res){
	      resplen = sprintf(response,"Error setting cdsoffset.");
	    } else { 
	      resplen = sprintf(response,"%d",val);
	    };
	  } else {
	    res = get_cdsoffset(&val);
	    if (res){
	      resplen = sprintf(response,"Error getting cdsoffset.");
	    } else {
	     resplen = sprintf(response,"%d",val);
	    };
	  }; //argument list
      };// set cdsoffset

      // set/get shutterdelay
      if (strcmp(cmd,"shutterdelay")==0){
	  if(argc == 1){
	    val = atoi(arg);
	    res = set_shutterdelay(val);
	    if(res){
	      resplen = sprintf(response,"Error setting shutter delay.");
	    } else { 
	      resplen = sprintf(response,"%d",val);
	    };
	  } else {
	    res = get_shutterdelay(&val);
	    if (res){
	      resplen = sprintf(response,"Error getting shutter delay.");
	    } else {
	     resplen = sprintf(response,"%d",val);
	    };
	  }; //argument list
      };// set shutter shutterdelay


      // set/get shutter trigger
      if (strcmp(cmd,"trigger")==0){
	  if(argc == 1){
	    val = atoi(arg);
	    res = set_trigger(val);
	    if(res){
	      resplen = sprintf(response,"Error setting shutter trigger mode.");
	    } else { 
	      resplen = sprintf(response,"%d",val);
	    };
	  } else {
	    res = get_trigger(&val);
	    if (res){
	      resplen = sprintf(response,"Error getting shutter trigger mode.");
	    } else {
	     resplen = sprintf(response,"%d",val);
	    };
	  }; //argument list
      };// set shutter trigger

      // set/get extshutter
      if (strcmp(cmd,"extshutter")==0){
	  if(argc == 1){
	    val = atoi(arg);
	    res = set_extshutter(val);
	    if(res){
	      resplen = sprintf(response,"Error setting extshutter mode.");
	    } else { 
	      resplen = sprintf(response,"%d",val);
	    };
	  } else {
	    res = get_extshutter(&val);
	    if (res){
	      resplen = sprintf(response,"Error getting extshutter mode.");
	    } else {
	     resplen = sprintf(response,"%d",val);
	    };
	  }; //argument list
      };// set extshutter

      // set/get exptime
      if (strcmp(cmd,"exptime")==0){
	if(argc == 1){
	  fval = atof(arg);
	    res = set_exposure_time(fval);
	    if(res){
	      resplen = sprintf(response,"Error setting exposure time.");
	    } else { 
	      resplen = sprintf(response,"%0.2f",fval);
	    };
	  } else {
	    res = get_exposure_time(&fval);
	    if (res){
	      resplen = sprintf(response,"Error getting exposure time.");
	    } else {
	      resplen = sprintf(response,"%0.2f",fval);
	    };
	}; //argument list
      };// set/get exptime
    

      if (strcmp(cmd,"expose")==0){
	res = expose();
	if (res){
	  resplen = sprintf(response,"Exposure error.");
	} else {
	  resplen = sprintf(response,"Exposure complete.");
	};
      };
     
      if (strcmp(cmd,"dark")==0){
	res = dark();
	if (res){
	  resplen = sprintf(response,"Exposure error.");
	} else {
	  resplen = sprintf(response,"Exposure complete.");
	};
      };

      if (strcmp(cmd,"bias")==0){
	res = bias();
	if (res){
	  resplen = sprintf(response,"Exposure error.");
	} else {
	  resplen = sprintf(response,"Exposure complete.");
	};
      };

      if (strcmp(cmd,"shutter")==0){
	if(argc == 1){
	  val = atoi(arg);
	  res = set_shutter(val);
	  if (res){
	    resplen = sprintf(response,"Error setting shutter setting.");
	  } else {
	    resplen = sprintf(response,"%d",val);
	  };
	} else {
	  res = get_shutter(&val);
	  if(res){
	    resplen = sprintf(response,"Error getting shutter setting.");
	  } else {
	    resplen = sprintf(response,"%d",val);
	  };
	};
      }; // shutter
      
      // WARNING, THIS DOES NO DIRECTORY ERROR CHECKING
      if (strcmp(cmd,"path")==0){
	if(argc == 1){
	  set_imagename_path(arg);
	} else {
	};
	resplen=get_imagename_path(response);
      }; // path

      // WARNING, THIS DOES NO VALID CHARACTER CHECKING
      if (strcmp(cmd,"root")==0){
	if(argc == 1){
	  set_imagename_root(arg);
	} else {
	};
	resplen=get_imagename_root(response);	  
      }; // root

      if (strcmp(cmd,"last")==0){
	resplen = get_last_filename(response);
	if(resplen < 0){
	  resplen = sprintf(response,"error getting filename");
	};
      };

      if (strcmp(cmd,"next")==0){
	resplen = get_next_filename(response);
	if(resplen < 0){
	  resplen = sprintf(response,"error getting filename");
	};
      };

      
    } else {
      // if the command is null, return the defaults
    };
    
    
    // *********************************************
    
    n = write(newsockfd,response,resplen);
    if (n < 0) error("ERROR writing to socket");
    close(newsockfd);
    return retval;
  } else { 
    return 0;
  };
  return 1;
};


