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
	struct s_program A;
	A.size=0;
	for (int i=0; i < 7;i++) {
		mutate_change(&A, 1, 0);
	}
	for (int i=0; i < 30;i++) {
		mutate_change(&A, 0, 0);
	}
	print_listing(&A);
	save_prog("test.red", &A);

}
