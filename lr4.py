#!/usr/bin/python

import sys
import os

replaces = dict()
output = []
header = []
outputline = 0
haveheader = 0
baseaddr = 0
dataadd = 0
nstart = 0

def useage():
	print("This is the label replacer for the Nibble Knowledge 4-bit computer")
	print("");
	print("Usage: " + sys.argv[0] + " definitionsfile inputasmfile outputasmfile")

def zerod(numstr):
	if numstr[0:2] == "0d":
		return numstr[2:len(numstr)]
	else:
		return numstr

def replace(string):

	replacestr = string.split("[")[0]
	if replacestr[0] == "*":
		replacestr = replacestr[1:]
	offset = 0
	brackets = string.find("[")
	if brackets == -1:
		offset = 0
	else:
		try:
			offset = int(zerod(string.split("[")[1][:-1]), 0)
		except:
			print "Invalid offset!"
			sys.exit(1)

	if replacestr in replaces:
		return str(replaces[replacestr] + offset)
	else:
		return string
		

def fillreplace(replacefile):
	global nstart

	for line in replacefile:
		words = line.split()
		if words[0][0] == "#" or words[0][0] == ";":
			continue
		elif words[0] == "N_:":
			nstart = 1			
		replaces[words[0][0:len(words[0]) - 1]] = int(zerod(words[1]), 0)

def inf(infline, inputfile):
	global haveheader
	global baseaddr
	global dataadd
	simpleheader = 0
	inpinf = 0
	gotbaddr = 0
	baddrnext = 0

	if haveheader == 1:
		print "Only 1 INF header per file!"
		sys.exit(1)
	else:
		haveheader = 1
		infwords = infline.split()
		for infword in infwords:
			header.append("INF")
			try:
				simpleheader = 1
				baseaddr = int(zerod(replace(infwords[1])), 0)
				header.append("PINF")
				header.append("BADR " + str(baseaddr))
				header.append("EPINF")
				header.append("DSEC D_SEC")
				dataadd = header.index("DSEC D_SEC")
				if nstart == 1:
					header.append("NSTART " + replace("N_"))
				header.append("EINF")
				return
			except:
				if len(infwords) > 1:
					print "Invalid base address " + infwords[1]
					sys.exit(1)
				simpleheader = 0

		for line in inputfile:
			words = line.split()
			if words[0][0] == "#" or words[0][0] == ";":
				continue
			elif words[0] == "PINF":
				inpinf = 1
				baddrnext = 1
				header.append("PINF")
			elif words[0] == "EPINF":
				header.append("EPINF")
				inpinf = 0
			elif words[0] == "EINF":
				header.append("EINF")
				return
			elif baddrnext == 1:
				if words[0] != "BADR":
					print "BADR is required to be the first psuedo-instruction after PINF!"
					sys.exit(1)
				else:
					try:
						baseaddr = int(zerod(replace(words[1])), 0)
						baddrnext = 0
					except:
						print "Invalid base address!"
						sys.exit(1)
					header.append("BADR " + str(baseaddr))
			elif inpinf == 1:
				header.append(words[0])
				for word in words[1:len(words)]:
					if word[0] == "#" or word[0] == ";":
						break
					header[len(header) - 1] += " "
					header[len(header) - 1] += replace(word)
				header[len(header) - 1].rstrip()
			else:
				print "Invalid header line " + line
				sys.exit(1)

def adddata(count, size):
	global dataadd
	
	dataadd += 1
	header.insert(dataadd, "DNUM " + str(count))
	dataadd += 1
	header.insert(dataadd, "DSIZE " + str(size))

def readinput(inputfile):
	global outputline
	datasize = 0
	datacnt = 0
	gotdata = 0
	i = 2

	while i > nstart: # This is a bad solution
		for line in inputfile:
			skip = 0
			indata = 0
			kind = ""
			asciistr = ""
			words = line.split()
			if words == []:
				continue
			elif words[0][0] == "#" or words[0][0] == ";":
				continue
			else:
				for word in words:
					if word[0] == "#" or word[0] == ";":
						outputline += 1
						skip = 1
						break
					elif word == "INF":
						inf(line, inputfile)
						skip = 1
						break
					elif word == ".data" or word == ".ascii" or word == ".asciiz":
						kind = word
						if gotdata == 0:
							gotdata = 1
							word = "D_SEC: " + word
						indata = 1
					elif indata == 1:
						if kind == ".data":
							try:
								size = int(zerod(word), 0)
								indata = 2
							except:
								print "Invalid .data size!"
								sys.exit(1)
							if size == datasize:
								datacnt += 1
							else:
								if datasize > 0:
									adddata(datacnt, datasize)
								datasize = size
								datacnt = 1
							if kind == ".ascii" or kind == ".asciiz":
								asciistr += word + " "
					elif indata == 2:
						if kind == ".data":
							if replace(word) != word:
								if datasize >= 4:
									word = replace(word)
								else:
									print ".data with a pointer statement must be bigger than 4 nibbles!"
							indata = 0
					
					if(len(output) <= outputline):
						output.append(word)
					else:
						output[outputline] += replace(word)
									
					output[outputline] += " "
			
				asciistr = asciistr.rstrip()
				if len(asciistr) > 0:
					if asciistr[0] == '"' and asciistr[len(asciistr) - 1] == '"':
						size = len(asciistr.decode("string_escape")) - 2
						if kind == ".asciiz":
							size += 1
						if size == datasize:
							datacnt += 1
						else:
							if datasize > 0:
								adddata(datacnt, datasize)
							datasize = size
							datacnt = 1

				if skip == 0:
					output[outputline] = output[outputline].rstrip()
					outputline += 1
		i -= 1
		if nstart == 0:
			inputfile = ["N_: .data 4 0x0123", ".data 4 0x4567", ".data 4 0x89AB", ".data 4 0xCDEF"]
	if datasize > 0 and haveheader == 1:
		adddata(datacnt, datasize)
	if gotdata == 0:
		output.append("D_SEC:")

if __name__ == '__main__':
	if len(sys.argv) < 4:
		useage()
		sys.exit(0)
	
	replacefile = open(sys.argv[1], "r")
	fillreplace(replacefile)
	replacefile.close()

	inputfile = open(sys.argv[2], "r")
	readinput(inputfile)
	inputfile.close()

	for line in header:
		print line
	
	for line in output:
		print line
	
