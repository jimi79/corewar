// create two empty fighter. (or more like n empty fights actually)
// try_to_improve(src_A, size_A)
// meaning install A, have it fight 10 times. if doesn't win 75% of the time, mutate(srcA)
// mutate :
//   change a random line
//	 OR
//   duplicate a random sequence (inserting it ... no idea how i will do that) (do not write over the limit !). Use a function to write x to y, so that it return 0 if not written (return the number of lines written)
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


int mutate_change(struct s_program* program) {
	int idx;
	if (program->size==0) {
		program->size=1;
		idx=0; 
		init_line(&program->lines[idx], 0, 0, 0, 0, 0);
	} else { idx=rand() % program->size; }

	int field=random() % 5; 
	int* pfield;
	int max_val; // max value + 1, will be used as %
	switch (field) {
		case 0: {
			max_val=8;
			pfield=&(program->lines[idx].type);
		} break;
		case 1: {
			max_val=3;
			pfield=&program->lines[idx].mod_A; 
		} break; 
		case 2: {
			max_val=3;
			pfield=&program->lines[idx].mod_B; 
		} break;
		case 3: {
			max_val=SIZE_CORE;
			pfield=&program->lines[idx].adr_A; 
		} break;
		case 4: {
			max_val=SIZE_CORE;
			pfield=&program->lines[idx].adr_B; 
		} break;
	} 
	int b; 
	b=*pfield;
	*pfield=random() % 10 - 5 + *pfield; 
	while (*pfield < 0) { *pfield=*pfield+max_val; }
	*pfield=*pfield % max_val; 
}

int test(struct s_program* program) {
	for (int i=0; i<10;i++) {
		printf("%dth mutation\n", i);
		mutate_change(program);
		print_listing(program);
		printf("\n");
	}
}

int main(int argc, char *argv[]) {
	randomize();
	// we create a list of programs, and init all of them with a DAT #0,#0
	int program_count=2; // count of poll of programs, that we'll have evolving till it can win against each other, at least xx % of the time
	float percent_fight_to_win=0.75; // percentages of fight to win to be declared better
	int count_fights=10; // number of rounds for each meeting


	struct s_program programs[program_count];
	// we initialize both programs empty

	programs[0].size=0; 

	test(&programs[0]);

	// then we mutate_change the first one 3 times
	// then we mutate_change the second one 6 times
	// then we mutate_duplicate
	// then we mutate_remove
	// and each time we check that is going allright



// TODO 
/*

mutate change = pick a random field, change its value by 1 to 5, modulo the max value (7 for type, 3 for address type, mem_size for address)


// i also need to save a program, at some point (write that in lib */
}


