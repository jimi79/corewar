#include <stdio.h>
#include <string.h>
#include <stdlib.h> 

struct s_test {
	int a;
	int b;
};

int test(struct s_test* blah)
{
	printf("%d %d\n", blah[2].a, blah[2].b);
}

int main (int argc, char *argv[] ) {
	struct s_test blah[100];
	for (int i=0;i<100;i++) {
		blah[i].a=i;
		blah[i].b=i*2;
	}
	test(blah); 
}
