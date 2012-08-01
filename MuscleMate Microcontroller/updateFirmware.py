import serial
import os

import ctypes
import itertools
import string
import time

def get_available_drives():
    drive_bitmask = ctypes.cdll.kernel32.GetLogicalDrives()
    return list(itertools.compress(string.ascii_uppercase,
               map(lambda x:ord(x) - ord('0'), bin(drive_bitmask)[:1:-1])))

port = 'COM3'
filename = 'mmm2'
drive = 'E'

commands = ['hex2bin.exe -l 8000 %(fn)s.hex',
			'lpcrc.exe %(fn)s.bin',
			'move "%(fn)s.bin" "%(drive)s:/%(fn)s.bin"',
			]
			
num_retries = 3
if __name__ == "__main__":

	for retry in range(num_retries):
		if retry: print 'retrying...'
		
		ser = serial.Serial(port, baudrate = 115200, timeout = 1)
		for y in [8, 0, 0, 0]:#range(4):	
			x = ser.write(chr(y))
			if x != 1:
				print 'failed to write'
				exit(1)
		
		count = 0
		countLimit = 10
		while not drive in get_available_drives() and count < countLimit:
			time.sleep(0.5)
			count+=1
		
		print get_available_drives()
		
		if count >= countLimit:
			print get_available_drives()
			print 'failed to mount usb drive'
			ser.close()
			
			if retry == num_retries-1:
				print 'failed too many times, quitting'
				exit(1)
		
		else:
			break
			
	paramsDict = {	'fn' : filename, 
					'drive' : drive
					}
					
	try:
		os.remove('%s.bin' % filename)
	except Exception, e:
		print 'old .bin file not there to be deleted'
	try:		
		x = os.remove('%s:/firmware.bin' % drive)	
		print 'old firmware deleted: %d' % x
	except Exception, e:
		print 'firmware.bin not found on the drive'

	for command in commands:
		cmd = command % paramsDict
		x = os.system(cmd)
		time.sleep(1)
		if x:
			print 'failed on last command: %s' % cmd
			exit(x)
			
	
	for retry in range(num_retries):
		if retry:
			print 'retrying to unmount usb drive...'
			
		ser.close()
		
		count = 0
		while drive in get_available_drives() and count < countLimit:
			time.sleep(0.5)
			count+=1
			
		if count >= countLimit:
			print get_available_drives()
			print 'failed to unmount usb drive'
		else:
			print 'great success'
			break
	
		time.sleep(1.0)
		ser = serial.Serial(port, baudrate = 115200, timeout = 1)
	