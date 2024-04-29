# FILE = 

CC            = gcc
CXX           = g++
CFLAGS    += -DLINUX -pipe -O2 -Wall -W -fopenmp -I/usr/include -I/opt/EDTpdv/tiff-4.0.3/libtiff -I/opt/NuvuCameras/include -I/opt/NuvuCameras/lib -I/lib/x86_64-linux-gnu/ -I/opt/pleora/ebus_sdk/Ubuntu-12.04-x86_64/include -L/usr/lib/x86_64-linux-gnu -I/usr/lib/x86_64-linux-gnu -I/usr/lib/ -I/opt/pleora/ebus_sdk/Ubuntu-12.04-x86_64/lib/genicam/bin/Linux64_x64 -I/opt/pleora/ebus_sdk/Ubuntu-12.04-x86_64/lib


LIBS      = -lncurses \
/usr/lib/gcc/x86_64-linux-gnu/4.8/libgomp.a \
/opt/NuvuCameras/lib/libnuvu.so \
/usr/lib/x86_64-linux-gnu/libtiff.so.4 \
/usr/lib/libcfitsio.a \
/usr/lib/x86_64-linux-gnu/libdl.so

####### Compile

#$(FILE): $(FILE).o
#	$(CXX) $(CFLAGS) -o $(FILE) $(FILE).o $(LIBS)
#
#%.o: %.c
#	$(CXX) -c $(FILE).c
#
#%.o: %.cpp
#	$(CXX) -c $(CFLAGS) $(FILE).cpp


all: 
	@clear
	@rm -f *.o
	@rm -f *~
	@rm -f obj/*
	@echo "Compiling server..."
	@g++ -c -o obj/server.o server.c	
	@echo "Done."
	@echo "Compiling camera..."	
	@g++ -c -o obj/camera.o $(CFLAGS) camera.cpp 	
	@echo "Done."
	@echo "Compiling server... " 
	@g++ obj/server.o obj/camera.o $(CFLAGS) $(LIBS) -o bin/camserver_cit camserver.c
	@echo "Done."
	@echo "Compiling client... "
	@gcc -o bin/cam_cit camclient.c 
	@echo "Done."

clean:
	@echo "Cleaning... "
	rm -f *.o
	rm -f *~
	rm -f obj/*
	@echo "Done."

