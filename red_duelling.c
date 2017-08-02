#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "lib_red.h"

int verbose=0; // debugging

int test() {
	debug_level=1;
	struct s_program A; 
	A.size=0;
	for (int i=0; i < 7;i++) {
		mutate_change(&A, 1, 0);
	}
	printf("--\n");
	print_listing(&A);
	mutate_remove(&A);
	printf("--\n");
	print_listing(&A);


	for (int i=0; i < 30;i++) {
		mutate_change(&A, 1, 0);
	}
	printf("--\n");
	print_listing(&A);
	mutate_remove(&A);
	printf("--\n");
	print_listing(&A);
}

int run(int argc, char *argv[]) {
	int i,j,k,l; // some increments
	float min_percent=60; // percentages of fight to win to be declared better
	int count_rounds=10; // number of rounds for each meeting 
	struct s_program prgA;
	struct s_program prgB; 
	prgA.size=0;
	prgB.size=0; 
	mutate_change(&prgA, 0, 0); // to force a program of size 1 at least 
	get_term_size(); // not sure it's useful 
	int cursor_A, cursor_B;
	int outcome;
	int win_A, win_B, count;
	float percent; 

	int r_mut;
	int n=0;
	int mid_dis=0; // some midtime display from time to time
	while (n<10) {
		n++;
		win_A=0;
		count=0; 
		percent=0;
		while (percent < min_percent) {
			for (i=0;i<count_rounds;i++) {
				init_core();
				get_random(&cursor_A, &cursor_B, &prgA, &prgB); 
				install_program(&prgA, cursor_A, 1); 
				install_program(&prgB, cursor_B, 2);
				outcome=run_fight(&cursor_A, &cursor_B); 
				count++;
				switch (outcome) {
					case 100: { } break;
					case 101: { win_A++; } break;
					case 102: { win_B++; } break;
					default: { printf("error #%d\n", outcome); } break;
				} 
			}
			percent=win_A*1.0/count*100;
			if (percent<min_percent) {
				for (i=0;i<prgB.size;i++) {
					prgA.lines[i].type=prgB.lines[i].type;
					prgA.lines[i].mod_A=prgB.lines[i].mod_A;
					prgA.lines[i].mod_B=prgB.lines[i].mod_B;
					prgA.lines[i].adr_A=prgB.lines[i].adr_A;
					prgA.lines[i].adr_B=prgB.lines[i].adr_B;
				}
				prgA.size=prgB.size;
				// if we failed, we restart from the opponent
				if (prgA.size==0) { r_mut=0; } 
				else {
					if (prgA.size<10) { r_mut=random()%2; }
					else { r_mut=random()%2; }
				}
				switch (r_mut) {
					case 0: { mutate_change(&prgA, 0, 0); } break;
					case 1: { mutate_duplicate(&prgA); } break;
					case 2: { mutate_remove(&prgA); } break;
				}
				if (prgA.size==0) { 
					return 99;
				}
			} 
			mid_dis++;
			if (mid_dis>1000) {
				print_listing(&prgA);
				mid_dis=0;
			} 
		} 
		printf("win %f%% of fights.", percent);
		printf("\nlisting");
		print_listing(&prgA);
		// print something
		// copy A to B
		// redo the same thing
		for (i=0;i<prgA.size;i++) {
			prgB.lines[i].type=prgA.lines[i].type;
			prgB.lines[i].mod_A=prgA.lines[i].mod_A;
			prgB.lines[i].mod_B=prgA.lines[i].mod_B;
			prgB.lines[i].adr_A=prgA.lines[i].adr_A;
			prgB.lines[i].adr_B=prgA.lines[i].adr_B;
		}
		prgB.size=prgA.size;
	}
} 

int main(int argc, char *argv[]) {
	randomize();
	/*test();
	return 0;*/
	run(argc, argv);
}
