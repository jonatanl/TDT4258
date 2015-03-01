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

# takes in binarynumber as a string and returns
def binToHex(number):
	return hex(int(number, 2))

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

def writeHexArrayToFile(hexArray):
	file = open('tetrisTheme.c', 'w')
	file.write('#include <stdint.h> \n')
	file.write('uint16_t songArray = {')

	for hexNumber in hexArray:
		file.write(hexNumber + ',')

	file.write('};')
	file.close()

def main():
	songeArray = []
	for i in xrange(len(note)):
		binaryNumber = ''
		# Append all bits
		binaryNumber += noteToBin(note[i])
		binaryNumber += octaveToBin(octave[i])
		binaryNumber += amplitudeToBin(amplitude[i])
		binaryNumber += durationToBin(duration[i])

		# Convert to hex
		hexNumber = binToHex(binaryNumber)

		songeArray.append(hexNumber)
	writeHexArrayToFile(songeArray)

# Song values
note 		= ['E', 'H', 'C', 'D', 'C', 'H', 'A', 'A', 'C', 'E', 'D', 'C', 'H', 'C', 'D', 'E', 'C', 'A', 'A', 'A', 'D', 'F', 'A', 'G', 'F', 'E', 'C', 'E', 'D', 'C', 'H', 'H', 'C', 'D', 'E'] # done with 7
octave 		= [4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4]
amplitude 	= [4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4]
duration 	= [4, 2, 2, 4, 2, 2, 4, 2, 2, 4, 2, 2, 6, 2, 4, 4, 4, 4, 8, 2, 4, 2, 4, 2, 2, 6, 2, 4, 2, 2, 4, 2, 2, 4, 4]

main()