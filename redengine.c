#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "lib_red.h"

void handler_sigsegv(int sig) {
	void *array[10];
	size_t size;

	// get void*'s for all entries on the stack
	size = backtrace(array, 10);

	// print out all the frames to stderr
	fprintf(stderr, "Error: signal %d:\n", sig);
	backtrace_symbols_fd(array, size, STDERR_FILENO);
	exit(1);
}

int parse_parameters(int argc, char *argv[]) { // use ->truc instead of &val.truc
	short i; 
	short mandatory=2;
	for(i=1; i < argc; i++) {
		int ok=0;
		if (!strcmp(argv[i], "--srcA")) { 
			i++;
			memcpy(filename_src_A, argv[i], strlen(argv[i]));
			mandatory--;
			ok=1;
		}
		if (!strcmp(argv[i], "--srcB")) {
			i++;
			memcpy(filename_src_B, argv[i], strlen(argv[i]));
			mandatory--;
			ok=1;
		} 
		if (!strcmp(argv[i], "--debug")) {
			i++;
			debug_level=(short)strtol(argv[i], NULL, 10); 
			if (debug_level>0) { display=1; }
			ok=1;
		} 
		if (!strcmp(argv[i], "--display")) { 
			i++;
			display=(short)strtol(argv[i], NULL, 10); 
			ok=1;
		} 
		//if (!strcmp(code, "log")) { 
		//	memcpy(log_filename, value, strlen(value));
		//	log_enabled=1;
		//} 
		if (!ok) {
			fprintf(stderr, "error, parameter %s unknown\n", argv[i]);
			return 0;
		}
		
	}; 
	if (mandatory > 0) {
		fprintf(stderr, "error, one parameter is missing. Mandatory parameters are srcA and srcB\n");
		return 0;
	}
	return 1;
}; 

int main(int argc, char *argv[]) { 
	signal(SIGSEGV, handler_sigsegv);   // install our handler

	randomize();

	if (!parse_parameters(argc, argv)) {
		return 1;
	} 
	get_term_size(); 
	int size_A=read_src(filename_src_A, src_A); // src_A will have to be allocated in the function 
	int size_B=read_src(filename_src_B, src_B); // src_B will have to be allocated in the function 

	if (!size_A) { return 0; } 
	if (!size_B) { return 0; }

	if (size_A + size_B > SIZE_CORE * 2) {
		fprintf(stderr, "programs are too big, or memory too short\n");
		return 1;
	}

	int i;
	for (i=0;i<SIZE_CORE;i++) {
		core[i].code.type=0; // at first, we write DAT #0, #0 everywhere
		core[i].code.mod_A=0;
		core[i].code.mod_B=0;
		core[i].code.adr_A=0;
		core[i].code.adr_B=0;
	} 

	cursor_A=rand() % SIZE_CORE;
	int left;
	left=SIZE_CORE - size_A - size_B;
	cursor_B=rand() % left;
	if (cursor_B > (cursor_A - size_B)) {
		cursor_B=cursor_B + size_A + size_B;
	}
	install_program(src_A, size_A, cursor_A, 1);
	install_program(src_B, size_B, cursor_B, 2);
	
	int outcome;
	outcome=run_fight();

	if (display) {
		switch (outcome) {
			case 100: { printf("Tie\n"); } break;
			case 101: { printf("A win\n"); } break;
			case 102: { printf("B win\n"); } break;
		}
	} 

	if (debug_level) {
		display_core_dump();
	} 
	// output start location to find why i segfault from time to time
	// do that with a debug level
	// if debug_level >=1, display starting pos. debug > x makes request of presskey each time. actually maybe 1 anyway.
	// have a list of all debug level needed, and sort them
	return outcome; 
}
