#include <stdio.h>
#include <string.h>
#include <stdlib.h> 

struct s_test {
	int a;
	int b;
};


int main (int argc, char *argv[] ) {
	int a=-1;
	printf("%d %d\n", a, a%4096);


	char s[1024];
	strcpy(s,"coucodqwelkqjwelu");
	printf("%d\n", strlen(s));

	a=6 % 1;
	printf("%d\n", a);
}
