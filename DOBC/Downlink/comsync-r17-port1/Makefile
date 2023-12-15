# Comment/uncomment the following line to enable/disable debugging
#DEBUG = y


ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DSCULLV_DEBUG # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

EXTRA_CFLAGS += $(DEBFLAGS) -I$(LDDINC) -DLINUX -w

TARGET = comsync

ifneq ($(KERNELRELEASE),)

comsync-objs :=  osapi.o iusc.o escc.o isr-iusc.o isr-escc.o main.o

obj-m	:= comsync.o

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) LDDINC=$(PWD) modules

endif


install:
	install -d $(INSTALLDIR)
	install -c $(TARGET).o $(INSTALLDIR)

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions


depend .depend dep:
	$(CC) $(EXTRA_CFLAGS) -M *.c > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
