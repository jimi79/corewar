#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "lib_red.h"

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

	debug_level=0;
	for (i=0;i<100;i++) {
		printf("---- mutate_duplicate ----\n");
		mutate_duplicate(program);
	}
	print_listing(program); 
	printf("---- mutate_change ----\n");
	for (i=0; i<1000;i++) {
		a=mutate_change(program, 0, 1);
	} 
	debug_level=1;
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

	int cursor_A, cursor_B;
	struct cell core[SIZE_CORE]; // that has to be local 

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
						init_core(core);
						get_random(&cursor_A, &cursor_B, &programs[i], &programs[opp]); 
						install_program(core, &programs[i], cursor_A, 1); 
						install_program(core, &programs[opp], cursor_B, 2);
						outcome=run_fight(core, &cursor_A, &cursor_B); 
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
} 
