#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int size_mem=4096;

char file_A[1024];
char file_B[1024];
short debug_level;
short display; // 1 true or 0 false



// structure of source : 
// 4 bytes for the type
// 2 bytes for the mode for A
// 2 bytes for the mode for B
// 12 bytes for A value
// 12 bytes for B value
// total = 32 bits, so 4 chars

// 

struct code {
	short type; /*
0 DAT
1 MOV
2 ADD
3 SUB
4 JMP
5 JMZ
6 DJZ
7 CMP */
	short mod_A; /*
mode for adresse A : 
* 0 : immediate "#"
* 1 : relative (value of address) ""
* 2 : indirect (value of value of address) */
	short mod_B;
	int adr_A; // address value
	int adr_B;
};

int parse_parameters(int argc, char *argv[]) { // use ->truc instead of &val.truc
	short i; 
	short mandatory=2;
	for(i = 1; i < argc; i++) {
		char *ptr = strchr(argv[i], '=');
		if (ptr) {
			int index = ptr - argv[i];
			char code[index];
			memcpy(code, &argv[i][0], index);
			code[index] = '\0'; 
			char value[strlen(argv[i]) - index];
			memcpy(value, &argv[i][index + 1], strlen(argv[i]) - index);
			int ok=0;
			if (!strcmp(code, "srcA")) { 
				memcpy(file_A, value, strlen(value));
				mandatory--;
			}
			if (!strcmp(code, "srcB")) {
				memcpy(file_B, value, strlen(value));
				mandatory--;
			} 
			if (!strcmp(code, "debug_level")) {
				debug_level = (short)strtol(value, NULL, 10); 
			} 
			if (!strcmp(code, "display")) { 
				display = (short)strtol(value, NULL, 10); 
			} 
			
		} else
		{
			fprintf(stderr, "Error: all parameters should be code=value. '%s' isn't\n", argv[i]);
			return 0;
		};
	}; 
	if (mandatory > 0) {
		fprintf(stderr, "Error, one parameter is missing. Mandatory parameters are srcA and srcB.\n");
		return 0;
	}
	return 1;
}; 

int main(int argc, char *argv[]) {
	if (!parse_parameters(argc, argv)) {
		return 1;
	}

	struct code tmp;
	tmp.type=3;
	tmp.mod_A=1;
	tmp.mod_B=0;
	tmp.adr_A=255;
	tmp.adr_B=5099;


	FILE *out = NULL; 
	if ((out = fopen(file_A, "wb")) == NULL) {
		fprintf(stderr, "Error while opening %s\n", file_A);
	}
	else {
		fwrite(&tmp, 1, sizeof(struct code), out); 
	}
	fclose(out);

}
