del mmm2.bin
hex2bin.exe -l 8000 mmm2.hex
lpcrc.exe mmm2.bin
del E:\firmware.bin
move mmm2.bin E:\mmm2.bin