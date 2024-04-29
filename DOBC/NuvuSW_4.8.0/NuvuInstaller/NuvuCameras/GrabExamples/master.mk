CC            = gcc
CXX           = g++
CFLAGS        = -Wall 
CFLAGS += -DLINUX -fopenmp -pipe
CFLAGS += -O2 
CFLAGS += -I/usr/lib/x86_64-linux-gnu -I/usr/include 
CFLAGS += -I/opt/NuvuCameras/include 

LIBS  = /opt/NuvuCameras/lib/libnuvu.so 

UNAME = $(shell cat /etc/lsb-release | grep -i distrib_id | sed s/DISTRIB_ID=//)
ifeq ($(UNAME),Ubuntu)
	#Libraries for Ubuntu
	LIBS += /usr/lib/gcc/x86_64-linux-gnu/4.6/libgomp.a /usr/lib/x86_64-linux-gnu/libtiff.so /usr/lib/x86_64-linux-gnu/libdl.so
else
	#Libraries for Scientific Linux & CentOS
	LIBS += /usr/lib/gcc/x86_64-redhat-linux/4.4.4/libgomp.a /usr/lib64/libtiff.so /usr/lib64/libdl.so
endif


$(FILE): $(FILE).o
	@echo -e "Linking : \t$@"
	@$(CC) $(CFLAGS) -o $(FILE)  $(FILE).o $(LIBS) 

$(FILE).o: $(FILE).c
	@echo -e "Compiling : \t$@"
	@$(CC) -c $(CFLAGS) $(FILE).c

clean:
	rm -f *.o
