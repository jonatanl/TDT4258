# Bitfield used to designate pitch of a note:
# 0 = A, 1 = A#,  2 = H, 3 = C, 4 = C#, 5 = D, 6 = D#, 7 = E, 8 = F, 9 = F#, 10 = G, 11 = G#
noteToNumber = {'A': 0, 'Ax': 1, 'H': 2, 'C': 3, 'Cx': 4, 'D': 5, 'Dx': 6, 'E': 7, 'F': 8, 'Fx': 9, 'G': 10, 'Gx': 11}

# 16 bit with the following structure:
# note = 4;
# octave = 4;
# amplitude = 3;
# duration = 5;

# number is an int value
# but return value is in bit as a string
def decimalToBin(number):
	return bin(number)[2:]

# takes in binary number as a string and returns hex number as a string
def binToHex(number):
	return hex(int(number, 2))

# Each part has a given number of bits. The number will not generate correct number of 0's in front.
# Fill missing 0's at the beginning.
def fillBitsIfMissing(number, numberOfBits):
	while numberOfBits > len(number):
		number = '0' + number
	return number

def noteToBin(note):
	binaryNumber = decimalToBin(noteToNumber[note])
	binaryNumber = fillBitsIfMissing(binaryNumber, 4)
	return binaryNumber

def octaveToBin(octave):
	binaryNumber = decimalToBin(octave)
	binaryNumber = fillBitsIfMissing(binaryNumber, 4)
	return binaryNumber

def amplitudeToBin(amplitude):
	binaryNumber = decimalToBin(amplitude)
	binaryNumber = fillBitsIfMissing(binaryNumber, 3)
	return binaryNumber

def durationToBin(duration):
	binaryNumber = decimalToBin(duration)
	binaryNumber = fillBitsIfMissing(binaryNumber, 5)
	return binaryNumber

def writeHexArrayToFile(file, hexArray):
	size = len(hexArray)

	file.write('uint16_t songArray[' + str(size) + '] = {')

	for hexNumber in hexArray:
		file.write(hexNumber + ',')

	file.write('}; \n')

def writeSongToFile(part0, part1):
	file = open('tetrisTheme.h', 'w')
	file.write('#include <stdint.h> \n')

	# Create array for channel 0
	writeHexArrayToFile(file, part0)
	file.write('synth_part part0 = { .start = (synth_note*) &notes_part0[0], .n_notes =' + str(len(part0)) +', .channel = 0 }; \n')

	file.write(' \n')
	
	# Create array for channel 1
	writeHexArrayToFile(file, part1)
	file.write('synth_part part1 = { .start = (synth_note*) &notes_part1[0], .n_notes =' + str(len(part1)) +', .channel = 1 }; \n\n')
	
	file.write('synth_song tetrisSong = { .part0 = &part0, .part1 = &part1, .default_duration_unit = 60 };')
	file.close()

def createHexArray(note, octave, amplitude, duration):
	songArray = []
	size = len(note)

	for i in xrange(size):
		binaryNumber = ''
		
		# Append all bits
		binaryNumber += durationToBin(duration[i])
		binaryNumber += amplitudeToBin(amplitude[i])
		binaryNumber += octaveToBin(octave[i])
		binaryNumber += noteToBin(note[i])

		# Convert to hex
		hexNumber = binToHex(binaryNumber)

		songArray.append(hexNumber)

	return songArray

def main():
	part0 = createHexArray(note0, octave0, amplitude0, duration0)
	part1 = createHexArray(note1, octave1, amplitude1, duration1)

	writeSongToFile(part0, part1)

# Part 0. Channel 0
note0 		= ['E', 'H', 'C', 'D', 'C', 'H', 'A', 'A', 'C', 'E', 'D', 'C', 'H', 'C', 'D', 'E', 'C', 'A', 'A', 'A', 'D', 'F', 'A', 'G', 'F', 'E', 'C', 'E', 'D', 'C', 'H', 'H', 'C', 'D', 'E', 'C', 'A', 'A'] # done with 8
octave0 	= [5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5]
amplitude0 	= [4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4]
duration0 	= [4, 2, 2, 4, 2, 2, 4, 2, 2, 4, 2, 2, 6, 2, 4, 4, 4, 4, 8, 2, 4, 2, 4, 2, 2, 6, 2, 4, 2, 2, 4, 2, 2, 4, 4, 4, 4, 8]

# Part 1. Channel 1
note1 		= ['C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'E', 'E', 'C', 'C', 'C', 'C', 'F', 'F', 'F', 'F', 'F', 'F', 'G', 'A', 'H', 'H', 'H', 'H', 'H', 'H', 'H', 'H', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'F', 'F', 'E', 'E', 'C', 'C', 'C', 'C', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F'] # done with 8
octave1 	= [4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5]
amplitude1 	= [4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4]
duration1 	= [2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2]

main()
