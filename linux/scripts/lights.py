#!/usr/bin/python
import mraa
import sys

command = sys.argv[1]

D7 = mraa.Gpio(0)
D7.dir(mraa.DIR_OUT)

if command == 'on':
 D7.write(0)
elif command == 'off':
 D7.write(1)
