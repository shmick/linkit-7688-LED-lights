#!/usr/bin/python
import mraa
import sys

command = sys.argv[1]

D7 = mraa.Gpio(0)

if command == 'on':
 print "Turning the lights %s" % (command)
 D7.dir(mraa.DIR_OUT_LOW)

elif command == 'off':
 print "Turning the lights %s" % (command)
 D7.dir(mraa.DIR_OUT_HIGH)

elif command == 'status':
 PinStatus = D7.read()
 print "%s" % (PinStatus)
 if PinStatus == 0:
  print "The lights should be on"
 elif PinStatus == 1:
  print "The lights should be off"
