#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "lib_red.h"

int main(int argc, char *argv[]) {
	// we add some lines, and we save the program
	randomize();
	struct s_program a;
	struct s_program b;
	a.size=0;
	for (int i=0; i < 10;i++) {
		mutate_change(&a, 1, 0);
	}
	for (int i=0; i < 30;i++) {
		mutate_change(&a, 0, 0);
	}
	for (int i=0;i<a.size;i++) {
		b.lines[i].type =a.lines[i].type;
		b.lines[i].mod_A=a.lines[i].mod_A;
		b.lines[i].mod_B=a.lines[i].mod_B;
		b.lines[i].adr_A=a.lines[i].adr_A;
		b.lines[i].adr_B=a.lines[i].adr_B;
	}
	b.size=a.size; 
	mutate_change(&a, 0, 0);
	print_two_listing(&a,&b);

}
