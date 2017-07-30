#!/usr/bin/python3

import argparse
import os 
import lib_rcompil

# output will be the source with a .red extension 
def main():
	parser=argparse.ArgumentParser()
	parser.add_argument("source", help="source in redcode") 
	args=parser.parse_args() 
	pre, ext=os.path.splitext(args.source)
	name_dest=pre+'.red'
	count_line=lib_rcompil.compile(args.source, name_dest)
	print("%d lines compiled. Compiled file is %s." % (count_line, name_dest))
				
main()
