#include "var_red.h" // -- functions

int adr(int val);
int copy_cell(int from, int to);
int copy_line(struct s_program* program, int a, int b);
int display_cell(int idx);
int display_core_dump();
int display_full_core();
int execute(int idx, int owner);
int get_term_size();
int get_val(int method, int address);
int init_line(struct s_red_line* line, int type, int mod_A, int mod_B, int adr_A, int adr_B);
int install_program(struct s_program* prog, int to, int owner);
int locate_cell(int idx);
int locate_log(int shift);
int pause_locate(int cursor);
int print_listing(struct s_program* prog);
int print_red_line(struct s_red_line* s);
int print_short_type(struct s_red_line* s);
int randomize();
int load_prog(char filename[MAX_SIZE_SRC], struct s_program* prog);
int run_fight();
int init_core();
int get_random(int* cursor_A, int* cursor_B, struct s_program* prog_A, struct s_program* prog_B);
int mutate_change(struct s_program* program, int force_append, int big_mutate);
int mutate_duplicate_location(struct s_program* program, int a, int b, int pos);
int mutate_duplicate(struct s_program* program);
int mutate_remove(struct s_program* program);
