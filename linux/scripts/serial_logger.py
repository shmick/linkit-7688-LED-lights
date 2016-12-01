#!/usr/bin/python

import serial 
serial = serial.Serial("/dev/ttyS0", 57600, timeout=2)

f = open("serial.log", "a+")

while True:
 data = serial.read()

# if data == "0": 
 f.write(data)
 f.flush()
