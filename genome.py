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
import operator

dup_max_size=0.1 # percentage of code duplicated (min=1 line)
max_src_size=300
random_adr=max_src_size
number_fights=1
max_red=100
count_opponents=30 # number of opponents to fight against
path_dna='/tmp/dna/'
number_generation=1


name_first=['c','d','p','l','m','r','s','t','z','f']
name_second=['a','e','i','o','u']


def random_name(): 
	for i in range(0,100):
		return ''.join([random.choice(name_first) + random.choice(name_second) for j in range(0,3)])


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


def championship(scores,files_):
# i want for each files_ to fight against a numer of ennemies
# i need to count for each the number of fights, because some will fight more than others
	count=[0 for i in range(0,len(files_))]  # count of fights per player 
	idx_all_opp=[i for i in range(0, len(files_))] # list of possible indexes for opponents 
	for i in range(0, len(files_)): # index of the first guy 
		#s="%0.0f%%" % (i/len(files_)*100) 
		#print(s, end="", flush=True) 
		idx_opp=copy.copy(idx_all_opp)
		idx_opp.remove(i) 
		random.shuffle(idx_opp)
		idx_opp=idx_opp[0:count_opponents] 
		print("opponents to fight against %s" % idx_opp)
		for j in idx_opp: 
			a=files_[i]
			b=files_[j]
			res=run("bin/red srcA=%s srcB=%s" % (a,b))
			if res==101:
				scores[i]+=1
				scores[j]-=1
			if res==102:
				scores[i]+=1
				scores[j]-=1
		count[i]+=1
		count[j]+=1

	print("scores %s" % scores)
	print("count %s" % count)
	scores=[scores[s]/count[s] for s in range(0, len(scores))]

	print("scores avg %s" % scores)

		#print("\033[%dD" % len(s), end="", flush=True)


# generate the list of genomes available in the dir (dir = 'dna')
def generate_empty_file(path,filename): 
	f=open(path+filename+'.cw','w')
	f.write(get_newline()+'\n')
	f.close()

def run(command): 
	process = Popen(shlex.split(command), stdout=PIPE)
	out, err = process.communicate()    # execute it, the output goes to the stdout
	out=out.decode('utf-8').splitlines()
	exit_code = process.wait()    # when finished, get the exit code
	return exit_code

def main(path): 
	path=os.path.join(path,'')
	files=get_list(path, 'cw')
	if len(files)<2:
		generate_empty_file(path, 'eve');
		generate_empty_file(path, 'adamn');
		files=get_list(path, 'cw') 
	scores=[0 for i in files]

	for generation in range(0,number_generation): # number of fights
		print("mutation ",end="",flush=True)

		files2=copy.copy(files)
		number_of_files_to_mutate=0.1*len(files2)
		number_of_files_to_mutate=int(number_of_files_to_mutate)
		if (number_of_files_to_mutate < 2):
			number_of_files_to_mutate=2
		files_to_mutate=[]
		for i in range(0, number_of_files_to_mutate):
			files_to_mutate.append(files2.pop(random.randrange(len(files2))))

		for file_ in files_to_mutate:
			#print("Mutate %s" % file_)
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
					new_filename=os.path.join(path, "%s_%s.cw" % (random_name(), time.strftime("%H_%M", time.gmtime())))
					inc=0
					base=new_filename
					while os.path.isfile(new_filename):
						new_filename="%d_%s" % (inc, base)
						inc+=1
					f=open(new_filename,"w")
					for line in src:
						f.write(line+'\n')
					f.close()

		print("compilation ",end="",flush=True)

		for file_ in files:
			pre, ext=os.path.splitext(file_)
			filedest=pre+'.red'
			if not(os.path.isfile(filedest)):
				count_line=lib_rcompil.compile(file_, filedest)

		print("fight ",end="",flush=True)

		files_championship=get_list(path, 'red') 
		scores=[0 for i in files]
		championship(scores, files_championship) 

		#print(" ", end="", flush=True) # championship displays some dots for each player
    # not anymore

		idx,val=max(enumerate(scores), key=operator.itemgetter(1))
		best_name=files_championship[idx]
		best_name=os.path.splitext(best_name)[0]+".cw"
		print("best %s" % (os.path.basename(best_name)))
		print(''.join(open(best_name, "r").readlines()))

		remove=0
		while len(files_championship) > max_red:
			idx,val=min(enumerate(scores), key=operator.itemgetter(1))
			f=files_championship[idx]
			base=os.path.splitext(f)[0]
			f2=base+".cw"
			os.remove(f)
			os.remove(f2)
			scores.pop(idx)
			files_championship.pop(idx) 
			remove+=1

		print("removed %d " % remove, end="", flush=True)

		files=get_list(path, 'cw')


# then we compile everything (a function)
# and championship (a function that feed scores)
# then we remove if more than xx (10)
#pre, ext = os.path.splitext(renamee)
#os.rename(renamee, pre + new_extension)

#for 10%, either mutate or random or both (1/3 chance mutate, 1/3 random, 1/3 both)


if __name__ == '__main__':
	main(path_dna)
