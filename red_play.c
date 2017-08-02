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

int verbose=0; // debugging

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
	
	if (program->size==0) { return 1; } //  error !

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
	if (program->size==0) { return 0; }
	int a=random() % (program->size);
	int b=random() % program->size + 1; // b excluded from copy
	if (b<a) {
		int c; c=a; a=b; b=c; }
	if (b==a) {
		b=a+1; } // a can't be at the end anyway, so i can add 1 to it 
	int size=b-a; // we don't pick line b actually
	int pos;
	pos=(random() % (program->size-size+1));
	return mutate_duplicate_location(program, a, b, pos);
}

int mutate_remove(struct s_program* program) {
	if (program->size==0) { return 0; }
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
	int i,j,k,l; // some increments
	randomize();
	float min_percent=50; // percentages of fight to win to be declared better
	int count_rounds=10; // number of rounds for each meeting 
	int count_fighters=10;
	int count_turns=1000; // number of turnes amongst all fighters
	struct s_program programs[count_fighters];
	for (i=0;i<count_fighters;i++) {
		programs[i].size=0; 
		mutate_change(&programs[i], 0, 0); // to force a program of size 1 at least
	}

	// we initialize both programs empty

	int cursor_A, cursor_B;

	get_term_size();
	int outcome;
	int win_A=0;
	int win_B=0;
	int opp; // opponent index
	int count;
	int r_mut; // choice of the mutation
	float percent;

	for (k=0; k<count_turns; k++) {
		for (i=0; i<count_fighters; i++) {
			percent=0;
			while (percent<min_percent) {
				count=0; // total number of fights
				win_A=0;
				win_B=0;
				for (j=0; j<count_rounds-1;j++) {
					for (l=0; l<count_rounds; l++) {
						opp=(i+j) % count_fighters;
						init_core();
						get_random(&cursor_A, &cursor_B, &programs[i], &programs[opp]); 
						install_program(&programs[i], cursor_A, 1); 
						install_program(&programs[opp], cursor_B, 2);
						outcome=run_fight(&cursor_A, &cursor_B); 
						count++;
						switch (outcome) {
							case 100: { } break;
							case 101: { win_A++; } break;
							case 102: { win_B++; } break;
							default: { printf("error #%d\n", outcome); } break;
						}
					}
				}
				percent=win_A*1.0/count*100;
				if (percent<min_percent) {
					r_mut=random()%3;
					switch (r_mut) {
						case 0: { mutate_change(&programs[i], 0, 0); } break;
						case 1: { mutate_duplicate(&programs[i]); } break;
						case 2: { mutate_remove(&programs[i]); } break;
					}
					//printf("-- after_mutate --\n");
					//print_listing(&programs[i]);
				}
			}
			printf("figher %d : %d/%d wins, %0.2f%%\n", i, win_A, count, percent); 
			print_listing(&programs[i]); 
		}
	}

/*
we init all program to be empty
we fight first vs 2, first vs 3, etc
then we fight second vs 3, vs 4, then vs 1st
then we fight nth versus n+1, n+2, ..., n-1 (modulo used)
for each :
	we fight all other fights, each x times, then we mutate (one out of the three method picked at random)
	and we keep going on till it win n% of the fights

so
for k times
	for i each fighters
		percent of win =0
		for j each other fighter (<count all fighters -1 )
			other fighter = i + j modulo count of fighers
			update number of win
		percent of win = number of win / count of fights
		while percent of win < 0.5
			for l number of fights
				fight
			make the avg
			if percent of win < 0.5
				mutate one way or another

		each time a mutation win more than 0.5 %, we display the source code


	for (i=0; i<number_of_fights; i++) {
		init_core();
		get_random(&cursor_A, &cursor_B, &programs[ev], &programs[adv]); 
		install_program(&programs[ev], cursor_A, 1); 
		install_program(&programs[adv], cursor_B, 2);
		outcome=run_fight(&cursor_A, &cursor_B); 
		switch (outcome) {
			case 100: { } break;
			case 101: { win_A++; } break;
			case 102: { win_B++; } break;
			default: { printf("error #%d\n", outcome); } break;
		}
	}
	float percent=win_A*1.0/number_of_fights*100;
	printf("%d/%d wins, %0.2f%%\n", win_A, number_of_fights, percent);
*/

	
	
}


