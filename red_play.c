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

int verbose=1; // debugging

int mutate_change(struct s_program* program, int force_append, int big_mutate) { // 1 force append
	int idx;
	if ((force_append) || (program->size==0)) {
		program->size++;
		idx=program->size-1; 
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
	int r=10; // range of change
	if (big_mutate) { r=1000; }
	*pfield=random() % r - 5 + *pfield; 
	while (*pfield < 0) { *pfield=*pfield+max_val; }
	*pfield=*pfield % max_val; 
	return idx;
}

int mutate_duplicate_location(struct s_program* program, int a, int b, int pos) {
	// we pick a and b (intervert if needed)
	// then we insert, by moving one from x steps to the end. discard lines if outside max
	
	if (verbose) { printf("a=%d b=%d\n", a, b); }
	int size=b-a; // we don't pick line b actually
	if (verbose) { printf("size=%d, prog size=%d\n", size, program->size); }
	
	int shifted=0; // the source we copy is before
	if ((pos<b) && (pos>a)) { pos=pos+size; } // we have to insert not in the middle of what we copy
	if (pos<a) { shifted=1; }
		// if shifted, the source is after what we copy, so we read from the new location of it
	int i; 
	if (verbose) { printf("duplicating from %d to %d before pos %d\n", a, b, pos);  }
	int src,dst;
	program->size=program->size+size;
	if (program->size > MAX_SIZE_SRC) {
		program->size = MAX_SIZE_SRC; }
	if (pos<program->size) {
		for (i=program->size-1; i>=pos+size; i--) {
			if (i < MAX_SIZE_SRC) {
				src=i-size;
				dst=i;
				if (verbose) { printf("moving line %d to %d\n", src, dst); }
				copy_line(program, i-size, i); 
			}
		}
	}
	if (verbose) { printf("shifted=%d\n", shifted); }
	for (i=0; i<size; i++) {
		src=i+a+shifted*size;
		dst=i+pos;
		if ((dst < MAX_SIZE_SRC) && (src < MAX_SIZE_SRC)) {
			if (verbose) { printf("and moving line %d to %d\n", src, dst); }
			copy_line(program, src, dst);
		}
	}
}

int mutate_duplicate(struct s_program* program) {
	int a=random() % (program->size);
	int b=random() % program->size + 1; // b excluded from copy
	if (b<a) {
		int c; c=a; a=b; b=c; }
	if (b==a) {
		b=a+1; } // a can't be at the end anyway, so i can add 1 to it 
	int size=b-a; // we don't pick line b actually
	int pos=(random() % (program->size-size)) + 1; // we insert before that index
	return mutate_duplicate_location(program, a, b, pos);
}

int mutate_remove(struct s_program* program) {
	int a=random() % program->size;
	int b=random() % program->size + 1; // b excluded from copy
	if (b<a) {
		int c; c=a; a=b; b=c; }
	if (b==a) {
		b=a+1; } // a can't be at the end anyway, so i can add 1 to it 
	
	if (verbose) { printf("a=%d b=%d\n", a, b); }

	int size=b-a;
	int src, dst;
	if (verbose) { printf("removing from %d to %d\n", a, b); }
	int end=size;
	if (end > program->size-b) { end=program->size-b; }
	for (int i=0;i<end; i++) {
		dst=i+a;
		src=i+a+size;
		if (verbose) { printf("moving %d -> %d\n", src, dst); }
		copy_line(program, src, dst);
	}
	program->size=program->size-size;

}

int test(struct s_program* program) {
	printf("---- mutate_change force append ----\n");
	int i, a;
	for (i=0; i<10;i++) { // x lines to have a proper listing
		a=mutate_change(program, 1, 0);
	}
	printf("---- mutate_change no force append ----\n");
	for (i=0; i<100;i++) {
		a=mutate_change(program, 0, 1);
	} 
	print_listing(program);
	printf("---- mutate_duplicate ----\n");
	mutate_duplicate_location(program, 0, 5, 1);
	print_listing(program);
	printf("---- mutate_duplicate ----\n");
	mutate_duplicate_location(program, 3, 5, 7);
	print_listing(program);
	printf("---- mutate_duplicate ----\n");
	mutate_duplicate_location(program, 3, 5, 3);
	print_listing(program);
	printf("---- mutate_duplicate ----\n");
	mutate_duplicate_location(program, 0, 2, program->size);
	print_listing(program);
	printf("---- mutate_duplicate ----\n");
	mutate_duplicate_location(program, 0, 4, 0);
	print_listing(program);

	verbose=0;
	for (i=0;i<100;i++) {
		printf("---- mutate_duplicate ----\n");
		mutate_duplicate(program);
	}
	print_listing(program); 
	printf("---- mutate_change ----\n");
	for (i=0; i<1000;i++) {
		a=mutate_change(program, 0, 1);
	} 
	verbose=1;
	print_listing(program);
	mutate_remove(program);
	print_listing(program);


	// one line program to test removing
	struct s_program test;
	print_listing(&test);
	a=mutate_change(&test, 1, 1);
	print_listing(&test);
	mutate_remove(&test);
	print_listing(&test);


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
	load_prog("bin/dwarf.cw", &program[1]);
	programs[1].size=0; 

	int ev=0; // index of the thing to evolve

	

	//run_fight(); // arg, parameters, cursor and everything. godamnit
	// either i use an object, either i keep that global.
	// i need a fucking object to store all of that.
	// or give pointers. yeah :
	// pointer to code
	// pointer to cursor_A
	// pointer to cursor_B
	// pointer to fighter_A (a program)
	// pointer to fighter_B (a program)

	
	
}


