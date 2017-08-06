#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "lib_red.h"

int verbose=0; // debugging

struct s_fight_params {
	struct s_core core;
	// i do not store the program because it doesn't change
	int cursor_A;
	int cursor_B;
	int res;
};

void *run_fight_thread(void *p) {
	struct s_fight_params* params=p;
	//printf("c2=%d\n", params->cursor_A);
	//params->res=testt3   (&params->core, &params->cursor_A, &params->cursor_B);
	params->res=run_fight(&params->core, &params->cursor_A, &params->cursor_B);
	//testt1(params->cursor_A);
	//testt2(&params->cursor_A);
};

int test3() {
	struct s_program A; 
	A.size=0;
	for (int i=0; i < 7;i++) {
		mutate_change(&A, 1, 0);
	}
	struct s_program B; 
	B.size=0;
	for (int i=0; i < 3;i++) {
		mutate_change(&B, 1, 0);
	}
	print_two_listing(&A, &B);
} 

int test2() {
	debug_level=1;
	struct s_program A; 
	A.size=0;
	for (int i=0; i < 7;i++) {
		mutate_change(&A, 1, 0);
	}
	print_listing(&A);
	for (int i=0; i < 100;i++) {
		mutate_change(&A, 0, 0);
	}
	print_listing(&A);
}

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
	float min_percent=75; // percentages of fight to win to be declared better
	int count_rounds=50; // number of rounds for each meeting 
	int max_mutations_before_reset=100; // number of mutations before we restart from the first one and try another tree
	struct s_program prgA;
	struct s_program prgB; 
	struct s_core core;
	prgA.size=0;
	prgB.size=0; 
	//mutate_change(&prgA, 0, 0); // to force a program of size 1 at least 
	load_prog("red/jim.red", &prgA);
	get_term_size(); // not sure it's useful 
	int cursor_A, cursor_B;
	int outcome;
	int win_A, win_B, count;
	float percent; 

	int r_mut;
	int n=0;
	int mid_dis=0; // some midtime display from time to time
	int count_mutations;
	int generation=0;

	pthread_t th[count_rounds]; 
	struct s_fight_params params[count_rounds];
	void *status=0;

	char filenameA[1024];
	char filenameB[1024];

	while (1) {
		n++;
		count_mutations=0;
		percent=0; 
		while (percent < min_percent) {
			win_A=0;
			count=0; 

			for (i=0;i<count_rounds;i++) { // n rounds 
				init_core(&params[i].core);
				get_random(&params[i].cursor_A, &params[i].cursor_B, &prgA, &prgB); 
				install_program(&params[i].core, &prgA, params[i].cursor_A, 1); 
				install_program(&params[i].core, &prgB, params[i].cursor_B, 1); 
				pthread_create(&th[i], NULL, run_fight_thread, (void*) &params[i]); 
				count++;
			}

			for (i=0;i<count_rounds;i++) { // n rounds 
				pthread_join(th[i], &status);
			} 

			for (i=0;i<count_rounds;i++) { // n rounds 
				outcome=params[i].res;
				switch (outcome) {
					case 100: { } break;
					case 101: { win_A++; } break;
					case 102: { win_B++; } break;
					default: { printf("error #%d\n", outcome); } break;
				} 
	
			}
			//printf("\n");
			percent=win_A*1.0/count*100;
			percent=percent+100*debug_level; // TODO always win so don't loop endlessly if debugging
			if (percent<min_percent) { // if still losing
				/*mid_dis++;
				if (mid_dis>10000) {
					printf("win %0.0f%% of fights\n", percent);
					printf("generation %d\n", generation);
					print_listing_limit(&prgA, 19);
					mid_dis=0;
				}****/
				if (count_mutations>max_mutations_before_reset) { // reset if dna too far away from the opponent
					//printf("clean that source\n");
					//print_listing(&prgA);
					count_mutations=0; 
					for (i=0;i<prgB.size;i++) {
						prgA.lines[i].type=prgB.lines[i].type;
						prgA.lines[i].mod_A=prgB.lines[i].mod_A;
						prgA.lines[i].mod_B=prgB.lines[i].mod_B;
						prgA.lines[i].adr_A=prgB.lines[i].adr_A;
						prgA.lines[i].adr_B=prgB.lines[i].adr_B;
					}
					prgA.size=prgB.size;
				} 
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
				count_mutations++;
				if (prgA.size==0) { 
					return 99;
				}
			} 
		} 
		generation++;
		printf("generation %d, win %0.0f%% of fights\n", generation, percent);
		if (print_two_listing(&prgA, &prgB)>30) {
			printf("generation %d, win %0.0f%% of fights\n", generation, percent);
		}

		// we write the files
		sprintf(filenameA, "a%d.red", generation);
		sprintf(filenameB, "b%d.red", generation);
		save_prog(filenameA, &prgA);
		save_prog(filenameB, &prgB);
		printf("both programs saved as %s (current one) and %s (old one)\n", filenameA, filenameB); 

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
	debug_level=0;
	if (debug_level==1) {
		test();
		test2();
		test3();
		return 0;
	}
	run(argc, argv);
}
