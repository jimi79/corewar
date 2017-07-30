#!/usr/bin/python3


# or maybe it would be just python [dir].
# and it will play with what's in it

# generate or modify a program
# input is the program. If empty, then i add a DAT #0,#0 in it, to have something to work on
# -m the number of things you randomly modify/insert (mutate)
# -d the number of duplications you do (totally random)
# -l number of line max

import copy
import math
import os
import glob
import random
import lib_rcompil
import shlex
from subprocess import Popen, PIPE
import time

dup_max_size=0.1 # percentage of code duplicated (min=1 line)
random_adr=100 # max random address (neg/pos/2)
max_src_size=100
match_count=50

def get_list(path, ext):
	return glob.glob('%s/*.%s' % (path, ext))

def rand_line():
	return int(math.pow(random.random()*2-1,5)*random_adr)

def get_newline():
	return "%s %s%d,%s%d" % (random.choice(lib_rcompil.commands), random.choice(lib_rcompil.mods), rand_line(), random.choice(lib_rcompil.mods), rand_line())

def mutate_change(src):
	#if (len(src)==0) or (random.randrange(1,10)==1):
	#	
	#else:
	newline=get_newline()
	if (len(src)!=0):
		src[random.randrange(0, len(src))] = newline
	else:
		src.insert(random.randrange(0, len(src) + 1), newline)
	# no return, modify list

def mutate_duplicate(src):
	if (len(src)!=0):
		a=random.randrange(0, len(src))
		b=random.randrange(0, len(src))
		if a>b:
			c=a
			a=b
			b=c
		pos=random.randrange(0, len(src) + 1)
		src[pos:pos]=src[a:b+1]

def mutate_delete(src):
	if (len(src)!=0):
		a=random.randrange(0, len(src))
		b=random.randrange(0, len(src))
		if a>b:
			c=a
			a=b
			b=c
		pos=random.randrange(0, len(src) + 1)
		src[a:b]=[]

def championship(files_):
	for i in range(0, len(files_)-1):
		a=files_[i]
		for j in range(i+1, len(files_)):
			b=files_[j]
			score_A=0
			score_B=0
# we got a fight here
			for x in range(0,match_count):
				res=run("bin/red srcA=%s srcB=%s" % (a,b))
				if res==101:
					score_A+=1
					score_B-=1
				if res==102:
					score_A-=1
					score_B+=1
				print("%s versus %s -> %d" % (a,b,res))
			score_A=score_A / match_count
			score_B=score_B / match_count 
			print("%s:%f %s:%f" % (a,score_A, b, score_B))

# generate the list of genomes available in the dir (dir = 'dna')
def generate_empty_file(path,filename): 
	f=open(path+filename+'.cw','w')
	f.write(get_newline()+'\n')
	f.close()

def run(command): 
	process = Popen(shlex.split(command), stdout=PIPE)
	out, err = process.communicate()    # execute it, the output goes to the stdout
	out=out.decode('utf-8').splitlines()
	#print('\n'.join(out))
	exit_code = process.wait()    # when finished, get the exit code
	return exit_code

def main(path): 
	new_file_header=time.strftime("%Y-%m-%d_%H_%M_%S", time.gmtime())
	new_file_inc=0 
	path=os.path.join(path,'')
	files=get_list(path, 'cw')
	if len(files)<2:
		generate_empty_file(path, '1');
		generate_empty_file(path, '2');
		files=get_list(path, 'cw') 
	scores=[0 for i in files]

	for i in range(0,1): # number of fights
		files2=copy.copy(files)
		number_of_files_to_mutate=int(0.1*len(files2))
		if (number_of_files_to_mutate) < 2:
			number_of_files_to_mutate=len(files2)
		print("%d files to mutate" % number_of_files_to_mutate)
		files_to_mutate=[]
		for i in range(0, number_of_files_to_mutate):
			files_to_mutate.append(files2.pop(random.randrange(len(files2))))

		for file_ in files_to_mutate:
			print("Mutate %s" % file_)
			f=open(file_,"r")
			src=f.read().splitlines() 
			f.close() 
			change=False
			if random.randrange(1,2)==1:
				mutate_duplicate(src)
				change=True
			if random.randrange(1,2)==1:
				mutate_change(src) 
				change=True
			if random.randrange(1,2)==1:
				mutate_delete(src) 
				change=True
			if change:
				if len(src) > max_src_size:
					src=src[0:max_src_size] 
				if len(src) > 0:
					new_filename=os.path.join(path, "%s_%s.cw" % (new_file_header, new_file_inc))
					new_file_inc+=1
					f=open(new_filename,"w")
					for line in src:
						f.write(line+'\n')
					f.close()

		for file_ in files:
			run('./rcompil.py %s' % file_)

		files_championship=get_list(path, 'red') 
		scores=[0 for i in files]
		championship(files_championship)

		files=get_list(path, 'cw')
# then we compile everything (a function)
# and championship (a function that feed scores)
# then we remove if more than xx (10)
#pre, ext = os.path.splitext(renamee)
#os.rename(renamee, pre + new_extension)

#for 10%, either mutate or random or both (1/3 chance mutate, 1/3 random, 1/3 both)


if __name__ == '__main__':
	main('dna')
