// create two empty fighter. (or more like n empty fights actually)
// try_to_improve(src_A, size_A)
// meaning install A, have it fight 10 times. if doesn't win 75% of the time, mutate(srcA)
// mutate :
//   change a random line
//	 OR
//   duplicate a random sequence (inserting it ... no idea how i will do that)
//   OR
// 	 remove a random part

// first open a source, and mutate it, and display the result, to test the three commands
// there will be a 'test' function

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "lib_red.h"

int main(int argc, char *argv[]) {
	// we create a list of programs, and init all of them with a DAT #0,#0
	int program_count=2; // count of poll of programs, that we'll have evolving till it can win against each other, at least xx % of the time
	float 0.75; // percentages of fight to win to be declared better
	int count_fights=10; // number of rounds for each meeting


	srcs=struct 
}
