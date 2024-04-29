USB_README.txt
-------------------------------------------------------------------------------
David Hale
dhale@astro.caltech.edu
2014-05-06

Introduction
------------
This is a brief description of configuring the linux udev device manager to
properly identify the device node for an attached USB device. When a USB
device is connected to the computer, udev will automatically create a device
node as /dev/ttyUSB#, where # is some number; however, this number # can
change with computer reboots, device power cycling, or device reconnection.
This document describes how to identify a particular device node and how to
write a udev rule to create a persistent symbolic link (an "alias") so that a
given USB device can always be referenced using the same identifier,
regardless of the somewhat arbitrary /dev/ttyUSB# number that may be assigned
to it.

Identify Your Device
--------------------
You need to find some unique identifiers for your device in the udev database.
To do this, query the udev database as follows:

$ udevadm info -a -n /dev/ttyUSB0

where, for this example I'm assuming the device is /dev/ttyUSB0. If you don't
know the number of your device, there are a few ways to figure this out. One
is to unplug your USB hardware, perform a directory listing of /dev/ttyUSB*,
then plug in your hardware, wait a few seconds and perform another directory
listing. The new /dev/ttyUSB# device node that just appeared is your hardware.
Other ways of identifying your hardware is to look more closely at the output
of the udevadm info command, which we need to do now anyway.

NB. It may be helpful to pipe the udevadm info command into more, or redirect
    it to a file for easier viewing.

udevadm info contains information about the device, provided by the
manufacturer. Often you'll see product names and serial numbers.  In order to
uniquely identify your hardware, you want to find a couple of characteristics
which will later be used to write a rule used to create a fixed name to your
otherwise changing device file node. Look for properties such as follows:

    ATTRS{idVendor}=="0403"
    ATTRS{idProduct}=="faf0"
    ATTRS{manufacturer}=="Thorlabs"
    ATTRS{product}=="APT Stepper Motor Controller"
    ATTRS{serial}=="70827008"

where typically the serial property will correspond to the actual serial
number of the hardware.

NB. When selecting properties to identify your hardware for the purpose of
    creating a udev rule, only properties from one parent of the device and
    from the device itself can be used for creating a match.

Creating Rules
--------------
Rule files are stored in /etc/udev/rules.d/ directory. You need to have
superuser privileges to write in this directory. Rule files should be named
xx-descriptive-name.rules, the xx should be chosen first according to the
following sequence points:

  < 60  most user rules; if you want to prevent an assignment being overriden
        by default rules, use the := operator.  these cannot access persistent
        information such as that from vol_id

  < 70  rules that run helpers such as vol_id to populate the udev db

  < 90  rules that run other programs (often using information in the udev db)

  >=90  rules that should run last


To create a rule to match the above example for the Thorlabs motor controller,
I created the file

/etc/udev/rules.d/60-thorlabs.rules

which contains the following lines:

SUBSYSTEMS=="usb", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="faf0", ATTRS{serial}=="70827008", SYMLINK+="APT_70827008", MODE="0666", GROUP="developer"
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="faf0", ATTRS{serial}=="70826554", SYMLINK+="APT_70826554", MODE="0666", GROUP="developer"

These say that if idVendor and idProduct and serial all match, then a symbolic
link is created. Thus, no matter which /det/ttyUSB# is used, the hardware
corresponding to ATTRS{serial}=="70826554" can be referenced as
/dev/APT_70826554

Testing Rules
-------------
Rules can be reloaded (as superuser) by:

# udevadm control --reload-rules
# udevadm trigger

Special Case for Trulink USB <-> Serial
---------------------------------------
The Trulink USB/Serial converter allows connection of up to four RS232 devices
and looks like a USB hub. The problem is that the four ports of the hub each
have the same identifying information. The RS232 interface does not provide
any manufacturer information, so there is no apparent way to uniquely identify
the hardware using the methods I've just described.

The solution is to ensure that the RS232 hardware is always connected to the
same RS232 port on the Trulink hub/converter. Even though the USB devices may
change with reboots or reconnecting the hub, the port number corresponding to
the numbered RS232 connectors appears to remain constant.

To handle devices connected to the Trulink, I wrote the following rule:

  /etc/udev/rules.d/81-trulink.rules

which contains:

SUBSYSTEMS=="usb", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", ATTRS{product}=="USB <-> Serial", PROGRAM="/usr/local/sbin/parse-usbport-devpath.py %p", SYMLINK+="%c", MODE ="0666", GROUP="developer"

The attributes of idVendor, idProduct and product will match all four USB
devices created by the Trulink. The next command will run a program, which
here is a python script /usr/local/sbin/parse-usbport-devpath.py

The %p argument passed to this script is the full device path. The %c in the
SYMLINK command is the result of the script. The script returns the path
device hierarchy. The last number is the port of the Trulink, with one caveat.
That is, there appears to be an anticorrelation between the device port and
the labeled RS232 connector. In other words, the script creates symlinks

/dev/trulink/2/1/3/1
/dev/trulink/2/1/3/2
/dev/trulink/2/1/3/3
/dev/trulink/2/1/3/4

which instead of corresponding to RS232 ports 1,2,3,4, actually correspond to
RS232 ports 4,3,2,1, respectively.

To add to user-friendliness, a set of symlinks has been created, pointing to
the symlinks that will be created by the udev rule,

/dev/power -> /dev/trulink/2/1/3/4               (RS232 port #1)
/dev/filterwheel -> /dev/trulink/2/1/3/3         (RS232 port #2)
/dev/exposurecontroller -> /dev/trulink/2/1/3/2  (RS232 port #3)

RS232 port #4 is currently not used.


