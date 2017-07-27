#!/usr/bin/python3



# i have to read the width and height of the screen, to find out if i can display the memory.
# i use all but 3 lines, and use the last two to display maybe %age of owned cells

import argparse

parser=argparse.ArgumentParser()
parser.add_argument("--file1", help="first source code")
parser.add_argument("--file2", help="second source code")
parser.add_argument("--display", help="display the memory content")

