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

port = 'COM11'
filename = 'mmm2'
drive = 'E'

commands = ['hex2bin.exe -l 8000 %(fn)s.hex',
			'lpcrc.exe %(fn)s.bin',
			'rm E:/firmware.bin',
			'move %(fn)s.bin E:/%(fn)s.bin',
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
		
		if count >= countLimit:
			print get_available_drives()
			print 'failed to mount usb drive'
			ser.close()
		
		else:
			break
			
	if retry == num_retries:
		print 'failed too many times, quitting'
		exit(1)
		
	paramsDict = {	'fn' : filename, 
					'drive' : drive
					}
					
	os.system('rm %s.bin' % filename)
		
	for command in commands:
		cmd = command % paramsDict
		x = os.system(cmd)
		if x:
			print 'failed on last command'
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
	
		ser = serial.Serial(port, baudrate = 115200, timeout = 1)
	