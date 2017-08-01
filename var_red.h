#define SIZE_CORE 1024
#define MAX_SIZE_SRC 1024

struct s_red_line {
	signed int type; /*
0 DAT
1 MOV
2 ADD
3 SUB
4 JMP
5 JMZ
6 DJZ
7 CMP */
	signed int mod_A; /*
mode for adresse A : 
* 0 : immediate "#"
* 1 : relative (value of address) ""
* 2 : indirect (value of value of address) */
	signed int mod_B;
	signed int adr_A; // address value
	signed int adr_B;
};

struct cell {
	struct s_red_line code;
	int owner; // 0 for none, 1 for A, 2 for B
};

struct s_program { // a program
	struct s_red_line lines[MAX_SIZE_SRC]; 
	int size; // number of lines
};

// -- global vars

char filename_prog_A[1024];
char filename_prog_B[1024];
short debug_level;
short display; // 1 true or 0 false
struct cell core[SIZE_CORE];
int cursor_A;
int cursor_B;
int screen_width;
int screen_height; 
struct s_program prog_A; // araray of struct s_red_line
struct s_program prog_B; // araray of struct s_red_line


