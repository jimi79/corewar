import struct
import re

commands          = ['DAT', 'MOV', 'ADD', 'SUB', 'JMP', 'JMZ', 'DJZ', 'CMP']
mods	= ['@', ' ', '#']
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
	line=struct.pack('iiiii', code_instr, mod_A, mod_B, adr_A, adr_B)
	return line

def degenerate_line(line):
	type_,mod_A,mod_B,adr_A,adr_B=struct.unpack('iiiii', line)
	return "%s %s%d, %s%d" % (commands[type_], mods[mod_A], adr_A, mods[mod_B], adr_B)

def parse_line(s): 
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

def decompile(filename_src, filename_dest):
	src=open(filename_src, "rb")
	str_=src.read()
	size=len(str_)
	if (size % 20) != 0: 
		raise ERedParseError("Size doesn't match. It should be a multiple of 20. Size is %d." % size)
	out=""
	for i in range(0, size//20):
		out+=degenerate_line(str_[i*20:i*20+20])+"\n"
	src.close()
	dest=open(filename_dest, "w")
	dest.write(out)
	dest.close()
	return size//20

def compile(filename_src, filename_dest):
	dest=open(filename_dest, "wb")
	count_line=0
	for i in open(filename_src, "r").readlines():
		i.replace('\n','') 
		i=re.sub(r" *;.*$", "", i) 
		if i != "":
			if i[0]!=';':
				val=None
				try:
					instr, mod_A, adr_A, mod_B, adr_B = parse_line(i)
					if not instr is None:
						val=generate_line(instr, mod_A, adr_A, mod_B, adr_B)
						if not val is None:
							count_line+=1
							dest.write(val)
				except ERedParseError as err:
					print(err)
					break
				except:
					raise
				if not(val is None):	
					pass # i write stuff here
	dest.close() 
	return count_line


