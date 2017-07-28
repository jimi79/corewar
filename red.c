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

struct s_red_line {
	int type; /*
0 DAT
1 MOV
2 ADD
3 SUB
4 JMP
5 JMZ
6 DJZ
7 CMP */
	int mod_A; /*
mode for adresse A : 
* 0 : immediate "#"
* 1 : relative (value of address) ""
* 2 : indirect (value of value of address) */
	int mod_B;
	int adr_A; // address value
	int adr_B;
};

int print_red_line(struct s_red_line *s) {
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

	printf("%s %c%u, %c%u\n", type, mod_A, s->adr_A, mod_B, s->adr_B);
}

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

int read_src(char filename[1024], struct s_red_line *src) {
	int count=0;
	FILE *in = NULL;
	if ((in = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "Error while opening %s\n", filename);
		return 1;
	}
	struct s_red_line tmp;
	int read=sizeof(struct s_red_line);
	while (read==sizeof(struct s_red_line)) {
		read=fread(&tmp, 1, sizeof(struct s_red_line), in);
		if (read==sizeof(struct s_red_line)) {
			count++;
		}
	}

	printf("here?\n");
	src=malloc(count * sizeof(*src)); // N times sizeof the struct
	int i=0;
	fseek(in, 0, SEEK_SET);
	int j; // temp var
	while (1) { 
		src[i].type=0; src[i].mod_A=0; src[i].mod_B=0; src[i].adr_A=0; src[i].adr_B=0;
		read=fread(&j, 1, sizeof(j), in); if (read) { src[i].type =j; } else break;
		read=fread(&j, 1, sizeof(j), in); if (read) { src[i].mod_A=j; } else break;
		read=fread(&j, 1, sizeof(j), in); if (read) { src[i].mod_B=j; } else break;
		read=fread(&j, 1, sizeof(j), in); if (read) { src[i].adr_A=j; } else break;
		read=fread(&j, 1, sizeof(j), in); if (read) { src[i].adr_B=j; } else break; 
		i++;
	} 
	return count;
}

int main(int argc, char *argv[]) {
	if (!parse_parameters(argc, argv)) {
		return 1;
	} 

	struct s_red_line *src_A; // araray of struct s_red_line
	int count;
	count=read_src(file_A, src_A); // SRC_A will have to be allocated in the function 

	int i;
	for(i=0;i<count;i++) {
		print_red_line(&src_A[i]); // but then maybe here i could work without & and change my function
	} 
	free(src_A);


}
