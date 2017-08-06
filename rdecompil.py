#!/usr/bin/python3

import argparse
import os 
import lib_rcompil

# output will be the source with a .red extension 
def main():
	parser=argparse.ArgumentParser()
	parser.add_argument("source", help="compiled source in redcode")
	args=parser.parse_args() 
	pre, ext=os.path.splitext(args.source)
	name_dest=pre+'.cw'
	count_line=lib_rcompil.decompile(args.source, name_dest)
				
main()

