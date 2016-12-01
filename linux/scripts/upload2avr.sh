#!/bin/sh
if [ -z "$1" ]
then
echo "Usage: $0 PathToHexFile"
exit 1
else
avrdude -p m32u4 -c linuxgpio -V -U flash:w:$1:i
fi
