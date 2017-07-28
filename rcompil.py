#!/usr/bin/python3

import argparse
import os
import struct
import re

commands          = ['DAT', 'MOV', 'ADD', 'SUB', 'JMP', 'JMZ', 'DJZ', 'CMP']
values_expected   = [  1,     2,     2,     2,     1,     2,     2,     2]

class ERedParseError(BaseException):
	pass

def parse_adr(s):
	if len(s)==0:
		raise ERedParseError('empty address')
	mod=1 # default mode = relative
	if s[0]=='#':
		s=s[1:]
		mod=0 # immediate
	if s[0]=='@':
		s=s[1:]
		mod=2 # indirect
	val=int(s)
	return mod,val

def generate_line(code_instr, mod_A, adr_A, mod_B, adr_B):
	line=     struct.pack('i', code_instr)
	line=line+struct.pack('i', mod_A)
	line=line+struct.pack('i', mod_B)
	line=line+struct.pack('i', adr_A)
	line=line+struct.pack('i', adr_B)
	return line

def parse(s): 
	out=''
	s=s.replace('\n', '');
	s=s.replace(',', ' ');
	s=s.split(' ')
	s=list(filter(('').__ne__, s))
	if (len(s))==0:
		return None, None, None, None, None
	if s.count!=0:
		command=s[0]
		if command in commands:
			code_instr=commands.index(command)
		else:
			raise ERedParseError('Unknown command %s' % command) 

		if len(s)==2:
			mod_A = 1
			adr_A = 0
			mod_B, adr_B=parse_adr(s[1])	
		if len(s)==3:
			mod_A, adr_A=parse_adr(s[1])	
			mod_B, adr_B=parse_adr(s[2])	
		if (len(s) < 2) or (len(s) > 3):
			raise ERedParseError("One or two paramters accepted, but not %d", s.count())

	return code_instr, mod_A, adr_A, mod_B, adr_B

# output will be the source with a .red extension 
def main():
	parser=argparse.ArgumentParser()
	parser.add_argument("source", help="source in redcode") 
	args=parser.parse_args() 
	pre, ext=os.path.splitext(args.source)
	name_dest=pre+'.red'
	dest=open(name_dest, "wb")
	line=0
	for i in open(args.source, "r").readlines():
		i.replace('\n','') 
		i=re.sub(r" *;.*$", "", i) 
		if i != "":
			if i[0]!=';':
				try:
					instr, mod_A, adr_A, mod_B, adr_B = parse(i)
					if not instr is None:
						val=generate_line(instr, mod_A, adr_A, mod_B, adr_B)
						if not val is None:
							line+=1
							dest.write(val)
				except ERedParseError as err:
					print(err)
					break
				except:
					raise
				if not(val is None):	
					pass # i write stuff here
	dest.close() 
	print("%d lines compiled. Compiled file is %s." % (line, name_dest))
				
main()
