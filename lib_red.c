#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "var_red.h"


int get_term_size() { 
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w); 
	screen_width=w.ws_col;
	screen_height=w.ws_row;
}

int init_line(struct s_red_line* line, int type, int mod_A, int mod_B, int adr_A, int adr_B) {
	line->type=type;
	line->mod_A=mod_A;
	line->mod_B=mod_B;
	line->adr_A=adr_A;
	line->adr_B=adr_B;
}

int print_red_line(struct s_red_line* s) {
	//printf("%d %d %d %d %d\n", s->type, s->mod_A, s->mod_B, s->adr_A, s->adr_B); // only to debug
	char type[4]; // instruction
	char mod_A; // type of address for A
	char mod_B; 
	switch (s->type) {
		case 0 : { strcpy(type,"DAT"); } break;
		case 1 : { strcpy(type,"MOV"); } break;
		case 2 : { strcpy(type,"ADD"); } break;
		case 3 : { strcpy(type,"SUB"); } break;
		case 4 : { strcpy(type,"JMP"); } break;
		case 5 : { strcpy(type,"JMZ"); } break;
		case 6 : { strcpy(type,"DJZ"); } break;
		case 7 : { strcpy(type,"CMP"); } break;
	}
	switch (s->mod_A) {
		case 0 : { mod_A='#'; } break;
		case 1 : { mod_A=' '; } break;
		case 2 : { mod_A='@'; } break;
	}
	switch (s->mod_B) {
		case 0 : { mod_B='#'; } break;
		case 1 : { mod_B=' '; } break;
		case 2 : { mod_B='@'; } break;
	}	
	printf("%s %c%d, %c%d", type, mod_A, s->adr_A, mod_B, s->adr_B);
}

int print_short_type(struct s_red_line* s) {
	switch (s->type) {
		case -1 : { printf(" "); } break; // cannot happen, default value in mem is DAT #0,#0
		case 0  : { printf("."); } break;
		case 1  : { printf("M"); } break;
		case 2  : { printf("A"); } break;
		case 3  : { printf("S"); } break;
		case 4  : { printf("J"); } break;
		case 5  : { printf("j"); } break;
		case 6  : { printf("d"); } break; // DJZ, not DAT
		case 7  : { printf("C"); } break;
	}
} 

int locate_cell(int idx) {
	int x,y;
	x=idx % screen_width + 1;
	y=idx / screen_width + 1; 
	printf("\033[%d;%dH", y, x);
}

int randomize() { 
	struct timeval t1;
	gettimeofday(&t1, NULL);
	srand(t1.tv_usec * t1.tv_sec);
}

int display_cell(int idx) {
// add a paramter to underline the instruction
	locate_cell(idx);
	int bgcolor, owner;
	owner=core[idx].owner;
	if (owner==0) { bgcolor=0; }
	if (owner==1) { bgcolor=41; }
	if (owner==2) { bgcolor=44; }
	printf("\033[%um", bgcolor); 
	print_short_type(&core[idx].code);
	printf("\033[0m"); 
}

int display_full_core() {
	int i;
	for (i=0;i < SIZE_CORE; i++) {
		display_cell(i);
	}
}

int copy_cell(int from, int to) {
	unsigned int to_offset = to % SIZE_CORE;
	unsigned int from_offset = from % SIZE_CORE;

  memcpy(&core[to_offset], &core[from_offset], sizeof(core[to_offset]));
	/*
	core[to_offset].code.type =core[from_offset].code.type;
	core[to_offset].code.mod_A=core[from_offset].code.mod_A;
	core[to_offset].code.mod_B=core[from_offset].code.mod_B;
	core[to_offset].code.adr_A=core[from_offset].code.adr_A;
	core[to_offset].code.adr_B=core[from_offset].code.adr_B;
	*/
	if (display) { display_cell(to); }
	return 1;
}

int copy_line(struct s_program* program, int a, int b) {
	program->lines[b].type=program->lines[a].type;
	program->lines[b].mod_A=program->lines[a].mod_A;
	program->lines[b].mod_B=program->lines[a].mod_B;
	program->lines[b].adr_A=program->lines[a].adr_A;
	program->lines[b].adr_B=program->lines[a].adr_B;
	return 1;
}

int pause_locate(int cursor) {
	locate_cell(cursor);
	fflush(stdout);
	if (debug_level>1) { getchar(); } else {
		if (display) { usleep(display * 1000); }
	}
} 

int locate_log(int shift) {
	printf("\033[%d;%dH\033[2K", screen_height-2+shift, 0);
} 

int compare_two_cells(int a, int b) { 
	if ((a<0) || (a>SIZE_CORE) ||
	    (b<0) || (b>SIZE_CORE)) { return 0; }
	return ((core[a].code.type ==core[b].code.type ) &&
					(core[a].code.mod_A==core[b].code.mod_A) &&
					(core[a].code.mod_B==core[b].code.mod_B) &&
					(core[a].code.adr_A==core[b].code.adr_A) &&
					(core[a].code.adr_B==core[b].code.adr_B));
}

int load_prog(char filename[MAX_SIZE_SRC], struct s_program* prog) {
	FILE* in=NULL;
	if ((in=fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "Error while opening %s\n", filename);
		return 0;
	}
	int i=0;
	int j;
	int read;
	while (1) { 
		prog->lines[i].type =0;
		prog->lines[i].mod_A=0;
		prog->lines[i].mod_B=0;
		prog->lines[i].adr_A=0;
		prog->lines[i].adr_B=0;
		read=fread(&j, 1, sizeof(j), in); if (read) { prog->lines[i].type =j; } else break;
		read=fread(&j, 1, sizeof(j), in); if (read) { prog->lines[i].mod_A=j; } else break;
		read=fread(&j, 1, sizeof(j), in); if (read) { prog->lines[i].mod_B=j; } else break;
		read=fread(&j, 1, sizeof(j), in); if (read) { prog->lines[i].adr_A=j; } else break;
		read=fread(&j, 1, sizeof(j), in); if (read) { prog->lines[i].adr_B=j; } else break; 
		//TODO check syntax here, like type between 0 and 7, mod between 0 and 3, and that's all
		i++;
	} 
	prog->size=i;
	return i;
}

int install_program(struct s_program* prog, int to, int owner) {
	int i;
	int dest;
	for (i=0; i < prog->size; i++) { 
		dest=(to + i) % SIZE_CORE;
		core[dest].code.type =prog->lines[i].type;
		core[dest].code.mod_A=prog->lines[i].mod_A; 
		core[dest].code.mod_B=prog->lines[i].mod_B; 
		core[dest].code.adr_A=prog->lines[i].adr_A; 
		core[dest].code.adr_B=prog->lines[i].adr_B; 
		core[dest].owner=owner;
	}
} 

int get_val(int method, int address) {
	// return val
	if (method==0) { return address; }
	if (method==1) { 
		if (address < 0) { address+=SIZE_CORE; }
		// we have to return value of B/A 
	}
}

int adr(int val) {// return proper address, based on core size
	if (val < 0) { val+=SIZE_CORE; }
	val=val%SIZE_CORE;
	return val;
} 

int execute(int idx, int owner) {
	struct s_red_line r;

	char short_name=' ';
	short_name = 'A' + (owner - 1); 

	int A, B; // temp values
	int A_is_adr=1; // A is an address. otherwise it's just a value. If it's an address, you copy the entire instruction

	int err=0; 
	int jump=0; // if 1, then we don't increment cursor location

	r=core[idx].code;

	if (debug_level) { 
		locate_log(-1);
		print_red_line(&r);
	}

	// return -1 if the guy lose
	if ((r.type<1) || (r.type>7)) {
		if (debug_level) {
			locate_log(0);
			printf("program %c, syntax error", short_name);
		}
		return -1;
	} 

	switch (r.type) {
		case 1: { // MOV
			switch (r.mod_A) {
				case 0: { A_is_adr=0; A=r.adr_A;
				} break;
				case 1: { A=adr(idx+r.adr_A); } break; 
				case 2: { A=adr(idx+r.adr_A); 
									A=adr(idx+core[A].code.adr_B);
				} break; 
			}
			switch (r.mod_B) {
				case 0: { err=1; } break;
				case 1: { B=adr(idx+r.adr_B); } break;
				case 2: {
					B=adr(idx+r.adr_B);
					B=adr(idx+core[B].code.adr_B);
				} break;
			} 
			if (!err) {
				if (A_is_adr) {
					if (debug_level) {
						locate_log(0);
						printf("program %c, copying command at %d to %d (%d)", short_name, A, adr(B-idx), B);
					}
					copy_cell(A, B);
					core[B].owner=owner; // does writing a value change the ownership ? boarpf
					if (display) { display_cell(B); }
				}
				else {
					if (debug_level) {
						locate_log(0);
						printf("program %c, writing value of B %d in %d (%d)", short_name, A, adr(B-idx), B);
					}
					core[B].code.adr_B=A; 
					core[B].owner=owner; // does writing a value change the ownership ? boarpf
					if (display) { display_cell(B); }
				} 
			}
		} break;
		case 2: case 3: { // ADD // SUB
			switch (r.mod_A) {
				case 0: { A=r.adr_A;
				} break;
				case 1: { A=adr(idx+r.adr_A); 
									A=core[A].code.adr_B; 
				} break; 
				case 2: { A=adr(idx+r.adr_A); 
									A=adr(core[A].code.adr_B);
									A=adr(core[idx+A].code.adr_B); 
				} break; 
			}
			switch (r.mod_B) {
				case 0: { err=1; } break;
				case 1: { B=r.adr_B; } break;
				case 2: { 
					B=adr(r.adr_B+idx);
					B=adr(core[B].code.adr_B);
				} break;
			} 
			if (!err) { 
				B=adr(B+idx);

				if (r.type==2) {
					core[B].code.adr_B=adr(core[B].code.adr_B+A);
					if (debug_level) {
						locate_log(0);
						printf("program %c, adding value %d in %d (%d) -> %d", short_name, A, adr(B-idx), B, core[B].code.adr_B);
					}
				}
				if (r.type==3) {
					core[B].code.adr_B=adr(core[B].code.adr_B-A);
					if (debug_level) {
						locate_log(0);
						printf("program %c, substracting value %d in %d (%d) -> %d", short_name, A, adr(B-idx), B, core[B].code.adr_B);
					} 
				}
			}
		} break;
		case 4: { // JMP
			switch (r.mod_B) {
				case 0: { err=1; } break;
				case 1: { B=adr(idx+r.adr_B); } break;
				case 2: {
					B=adr(idx+r.adr_B);
					B=adr(idx+core[B].code.adr_B);
				} break;
			} 
			if (!err) {
				jump=1;
				if (debug_level) {
					locate_log(0);
					printf("program %c, jumping to %d (%d)", short_name, adr(B-idx), B);
				}
				idx=B;
			}
		} break;
		case 5: { // JMZ
			switch (r.mod_A) {
				case 0: { A=r.adr_A;
				} break;
				case 1: { A=adr(idx+r.adr_A); 
									A=adr(core[A].code.adr_B); 
				} break; 
				case 2: { A=adr(idx+r.adr_A); 
									A=adr(idx+core[A].code.adr_B);
									A=adr(core[A].code.adr_B); 
				} break; 
			}
			switch (r.mod_B) {
				case 0: { err=1; } break;
				case 1: { B=r.adr_B; } break;
				case 2: { 
					B=adr(r.adr_B+idx);
					B=core[B].code.adr_B;
				} break;
			} 
			if (A==0) {
				if (debug_level) {
					locate_log(0);
					printf("program %c, jumping because %d == 0, to %d (%d)", short_name, A, adr(B-idx), B);
				}
				jump=1;
				idx=adr(B+idx);
			} 
			else {
				if (debug_level) {
					locate_log(0);
					printf("program %c, NOT jumping because %d != 0", short_name, A);
				}
			}
		}  break;
		case 6: { // DJZ 
			switch (r.mod_A) {
				case 0: { err=1; } break;
				case 1: { A=r.adr_A; } break;
				case 2: { 
					A=adr(r.adr_A+idx);
					A=adr(core[A].code.adr_A);
				} break;
			} 
			switch (r.mod_B) {
				case 0: { err=1; } break;
				case 1: { B=adr(idx+r.adr_B); } break;
				case 2: {
					B=adr(idx+r.adr_B);
					B=adr(idx+core[B].code.adr_B);
				} break;
			} 
			if (!err) {
				A=adr(idx+A);
				core[A].code.adr_B-=1;
				A=core[A].code.adr_B;
				if (A==0) {
					if (debug_level) {
						locate_log(0);
						printf("program %c, jumping because %d == 0, to %d (%d)", short_name, A, adr(B-idx), B);
					}
					jump=1;
					idx=B;
				} 
				else {
					if (debug_level) {
						locate_log(0);
						printf("program %c, NOT jumping because %d != 0", short_name, A);
					}
				}
			}
		} break; 
		case 7: { // CMP : skip if not equal
			switch (r.mod_A) {
				case 0: { A=r.adr_A;
				} break;
				case 1: {
					A=adr(idx+r.adr_A); 
					A=core[A].code.adr_B;
				} break; 
				case 2: {
					A=adr(idx+r.adr_A); 
					A=adr(idx+core[A].code.adr_B);
					A=core[A].code.adr_B;
				} break; 
			}
			switch (r.mod_B) {
				case 0: { B=r.adr_B;
				} break;
				case 1: {
					B=adr(idx+r.adr_B); 
					B=core[B].code.adr_B;
				} break; 
				case 2: {
					B=adr(idx+r.adr_B); 
					B=adr(idx+core[B].code.adr_B);
					B=core[B].code.adr_B;
				} break; 
			}
			if (A==B) {
				if (debug_level) {
					locate_log(0);
					printf("program %c, NOT skipping next line because %d == %d", short_name, A, B);
				}
			}
			else {
				jump=1;
				idx=adr(idx+2); 
				if (debug_level) {
					locate_log(0);
					printf("program %c, skipping next line because %d != %d", short_name, A, B);
				}
			} 
		}
	}
	if (err) {
		idx=-1;
		if (debug_level) {
			locate_log(0);
			printf("Syntax error");
		}
	}
	else {
		if (!jump) { idx++; idx=adr(idx); };
	}

	return idx;
}

int get_random(int* cursor_A, int* cursor_B, struct s_program* prog_A, struct s_program* prog_B) {
	*cursor_A=rand() % SIZE_CORE;
	int left;
	left=SIZE_CORE - prog_A->size - prog_B->size;
	*cursor_B=rand() % left;
	if (*cursor_B > (*cursor_A - prog_B->size)) {
		cursor_B=cursor_B + prog_A->size + prog_B->size;
	}
	return 1;
}

int init_core() {
	int i;
	for (i=0;i<SIZE_CORE;i++) {
		core[i].code.type=0; // at first, we write DAT #0, #0 everywhere
		core[i].code.mod_A=0;
		core[i].code.mod_B=0;
		core[i].code.adr_A=0;
		core[i].code.adr_B=0;
	} 
} 

int display_core_dump(int cursor_A, int cursor_B) {
	int i;
	int dotwritten=0;
	printf("\n------core dump -----\n");
	i=0;
	int repetition=0; // number of repetitions of the last instruction.
	while (i <= SIZE_CORE) { 
		if (compare_two_cells(i, i-1)) { repetition++; }
		else { 
			if (repetition > 0) { printf("< %d times>\n", repetition); repetition=0; }
			if (i < SIZE_CORE) { 
				print_red_line(&core[i].code);
				if (i==cursor_A) {
					 printf(" <--- cursor A");
				};
				if (i==cursor_B) {
					 printf(" <--- cursor B");
				};
				printf("\n");
			}
		}
		i++;
	}
} 

int run_fight(int* cursor_A, int* cursor_B) { // cursor values are modified
	int i, tmp_A, tmp_B;
	tmp_A=*cursor_A;
	tmp_B=*cursor_B;

	if (display) {
		printf("\033[2J");
		display_full_core(); 
		printf("\033[%u;%uH", screen_height - 1,0);
	}

	int outcome=100; // outcome of the match. 100=tie, 101 A wins, 102 B wins

	int max_run = SIZE_CORE * 2;

	for (i=0;i<max_run;i++) {
		tmp_A=execute(*cursor_A, 1); // if -1, then lose
		if (display) { pause_locate(*cursor_A); };
		tmp_B=execute(*cursor_B, 2); // if -1, then lose
		if (display) { pause_locate(*cursor_B); };

		if ((tmp_A==-1) && (tmp_B==-1)) {
			outcome=100;
			break;
		} // tie if both lose

		if (tmp_A==-1) {
			outcome=102;
			break;
		} 
		if (tmp_B==-1) {
			outcome=101;
			break;
		} 
		*cursor_A=tmp_A;
		*cursor_B=tmp_B; 
	} 
	if (display) {
		printf("\033[%d;%dH", screen_height-1, 0);
	}
	return outcome;
}

int print_listing(struct s_program* prog) {
	printf("listing %d lines\n", prog->size);
	for (int i=0;i<prog->size;i++) {
		printf("%d: ", i);
		print_red_line(&prog->lines[i]);
		printf("\n");
	}
}

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

	if (debug_level) { printf("a=%d b=%d\n", a, b); }
	int size=b-a; // we don't pick line b actually
	if (debug_level) { printf("size=%d, prog size=%d\n", size, program->size); }
	
	int shifted=0; // the source we copy is before
	if ((pos<b) && (pos>a)) { pos=pos+size; } // we have to insert not in the middle of what we copy
	if (pos<a) { shifted=1; }
		// if shifted, the source is after what we copy, so we read from the new location of it
	int i; 
	if (debug_level) { printf("duplicating from %d to %d before pos %d\n", a, b, pos);  }
	int src,dst;
	program->size=program->size+size;
	if (program->size > MAX_SIZE_SRC) {
		program->size = MAX_SIZE_SRC; }
	if (pos<program->size) {
		for (i=program->size-1; i>=pos+size; i--) {
			if (i < MAX_SIZE_SRC) {
				src=i-size;
				dst=i;
				if (debug_level) { printf("moving line %d to %d\n", src, dst); }
				copy_line(program, i-size, i); 
			}
		}
	}
	if (debug_level) { printf("shifted=%d\n", shifted); }
	for (i=0; i<size; i++) {
		src=i+a+shifted*size;
		dst=i+pos;
		if ((dst < MAX_SIZE_SRC) && (src < MAX_SIZE_SRC)) {
			if (debug_level) { printf("and moving line %d to %d\n", src, dst); }
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
	
	if (debug_level) { printf("a=%d b=%d\n", a, b); }

	int size=b-a;
	int src, dst;
	if (debug_level) { printf("removing from %d to %d\n", a, b); }
	int end=size;
	if (end > program->size-b) { end=program->size-b; }
	for (int i=0;i<end; i++) {
		dst=i+a;
		src=i+a+size;
		if (debug_level) { printf("moving %d -> %d\n", src, dst); }
		copy_line(program, src, dst);
	}
	program->size=program->size-size;

}


