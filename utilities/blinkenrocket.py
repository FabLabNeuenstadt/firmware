#!/usr/bin/env python

import sys, wave

class modem:

	# Modem specific constants
	bits = [[3 * chr(0), 5 * chr(0)], [3 * chr(255), 5 * chr(255)]]
	sync = [17 * chr(0), 17 * chr(255)]
	# Variable to alternate high and low
	hilo = 0
	supportedFrequencies = [16000,22050,24000,32000,44100,48000]
	cnt = 0
	# Data variables
	data = []
	parity = True
	frequency = 48000

	# Hamming code translation table
	_hammingCalculateParityLowNibble = [0,  3,  5,  6,  6,  5,  3,  0,  7,  4,  2,  1,  1,  2,  4,  7]
	_hammingCalculateParityHighNibble = [0,  9, 10,  3, 11,  2,  1,  8, 12,  5,  6, 15,  7, 14, 13,  4]

	# Almost nothing here
	def __init__(self, data=[], parity=True, frequency=48000):
		self.data = data
		self.parity = parity
		self.frequency = frequency if frequency in self.supportedFrequencies else 48000

	# Calculate Hamming parity for 12,8 code (12 bit of which 8bit data)
	def hammingCalculateParity128(self, byte):
		return self._hammingCalculateParityLowNibble[byte&0x0F] ^ self._hammingCalculateParityHighNibble[byte >> 4]

	# Calculate Hamming parity for 24,16 code (24 bit of which 16 bit are data)
	def hammingCalculateParity2416(self, first, second):
		return self.hammingCalculateParity128(second) << 4 | self.hammingCalculateParity128(first)

	# Generate one sync-pulse
	def syncsignal(self):
		self.hilo ^= 1
		return self.sync[self.hilo]

	# Generate a number of sync signals
	def generateSyncSignal(self, number):
		sound = ""
		for i in xrange(number):
			sound += self.syncsignal()
		return sound

	# Decode bits to modem signals
	def modemcode(self, byte):
		bleep = ""
		for x in xrange(8):
			self.hilo ^= 1
			bleep += self.bits[self.hilo][byte & 0x01]
			byte >>= 1
		return bleep

	# Return <length> samples of silence
	def silence(self, length):
		return chr(127) * length

	# Set data for modem code
	def setData(self, data):
		self.data = data

	# Set whether to use parity or not
	def setParity(self, parity):
		self.parity = parity

	# Set the frequency for the audio
	def setFrequency(self, frequency):
		self.frequency = frequency if frequency in self.supportedFrequencies else 48000

	# Generates the audio frames based on the data
	def generateAudioFrames(self):
		if self.parity:
			tmpdata = []
			# for uneven length data, we have to append a null byte
			if not len(self.data) % 2 == 0:
				self.data.append('0x00')
			# insert the parity information every two bytes, sorry for the heavy casting
			for index in range(0, len(self.data), 2):
				tmpdata.extend(self.data[index:index+2])
				tmpdata.append(chr(self.hammingCalculateParity2416(ord(self.data[index]),ord(self.data[index+1]))))
			self.data = tmpdata
		# generate the audio itself
		# add some sync signal in front
		sound = self.generateSyncSignal(4)
		# process the data and insert sync signal every 10 bytes
		for byte in self.data:
			sound += self.modemcode(ord(byte))
			self.cnt += 1
			if self.cnt == 10:
				sound += self.generateSyncSignal(2)
				self.cnt = 0
		# add some sync signals in the end
		sound += self.generateSyncSignal(4)
		return sound

	def saveAudio(self,filename):
		wav = wave.open(filename, 'wb')
		wav.setparams((1, 1, self.frequency, 0, "NONE", None))
		wav.writeframes(self.generateAudioFrames())
		wav.close()

class Frame( object ):
    """ Returns the representation """
    def getRepresentation( self ):
        raise NotImplementedError( "Should have implemented this" )

class textFrame(Frame):
	text = ""
	speed = 0
	delay = 0
	direction = 0

	def __init__(self,text,speed=1,delay=0,direction=0):
		self.text = text
		self.setSpeed(speed)
		self.setDelay(delay)
		self.setDirection(direction)

	def setSpeed(self,speed):
		self.speed = speed if speed < 16 else 1

	def setDelay(self,delay):
		self.delay = delay if delay < 16 else 0

	def setDirection(self,direction):
		self.direction = direction if direction in [0,1] else 0

	# Header -> 4bit speed, 4 bit delay, 4 bit direction, 4 bit zero
	def getHeader(self):
		return [chr(self.speed << 4 | self.delay), chr(self.direction << 4 | 0x00)]

	def getRepresentation(self):
		retval = []
		retval.extend(self.getHeader())
		retval.extend(list(self.text))
		return retval

class animationFrame(Frame):
	animation = []
	speed = 0
	delay = 0

	def __init__(self,animation,speed=1,delay=0):
		self.setAnimation(animation)
		self.setSpeed(speed)
		self.setDelay(delay)

	def setAnimation(self,animation):
		if len(animation) % 8 is not 0:
			raise Exception
		else:
			self.animation = animation

	def setSpeed(self,speed):
		self.speed = speed if speed < 16 else 1

	def setDelay(self,delay):
		self.delay = delay if delay < 16 else 0

	# Header -> 4bit zero, 4bit speed, 4 bit zero, 4 bit direction
	def getHeader(self):
		return [chr(self.speed), chr(self.delay)]

	def getRepresentation(self):
		retval = []
		retval.extend(self.getHeader())
		retval.extend(self.animation)
		return retval


class blinkenrocket():

	eeprom_size = 65536
	startcode = chr(0x99)
	patterncode = chr(0xA9)
	frames = []

	def __init__(self,eeprom_size=65536):
		self.eeprom_size = eeprom_size if eeprom_size < 256*1024*1024 else 65536
	
	def addFrame(self, frame):
		if not isinstance(frame, Frame):
			raise RuntimeError("Incorrect frame supplied")
		else:
			self.frames.append(frame)

	def getMessage(self):
		output = [self.startcode, self.startcode]
		for frame in self.frames:
			output.extend([self.patterncode,self.patterncode])
			output.extend(frame.getRepresentation())
		return output




if __name__ == '__main__':
	m = modem(parity=False)
	#print list(open(sys.argv[1]).read())
	m.setData(list(open(sys.argv[1]).read()))
	m.saveAudio(sys.argv[2])


