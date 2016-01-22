#!/usr/bin/python

import sys

_hammingCalculateParityLowNibble = [0,  3,  5,  6,  6,  5,  3,  0,  7,  4,  2,  1,  1,  2,  4,  7]
_hammingCalculateParityHighNibble = [0,  9, 10,  3, 11,  2,  1,  8, 12,  5,  6, 15,  7, 14, 13,  4]

def HammingCalculateParity128(byte):
	return _hammingCalculateParityLowNibble[byte&0x0F] ^ _hammingCalculateParityHighNibble[byte >> 4]

def HammingCalculateParity2416(first, second):
	return HammingCalculateParity128(second) << 4 | HammingCalculateParity128(first)

output = ""
bytepos = 0
oldbyte = None

# Read bytewise
f = open(sys.argv[1])
for byte in f.read():
	output += byte
	oldbyte = byte
	bytepos += 1
	if bytepos == 2:
		bytepos = 0
		output += str(HammingCalculateParity2416(ord(oldbyte), ord(byte)))
if bytepos == 1:
	output += " "
	output += str(HammingCalculateParity2416(ord(oldbyte), ord(" ")))

print output