
from ubuntu, 
dmesg -T

find which usb-uart port was mapped, keep in mind that PIC32MZ-EF-Starter-Kit
is using the mini-usb connection which is located underneath ethernet daughter board.

sudo picocom /dev/ttyACM0 -b 115200

you will see "hello world" print out.
