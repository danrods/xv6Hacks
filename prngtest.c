#include "types.h"
#include "user.h"

#define RUN 100



int 
main(int argc, char *argv[]) {
	int table[20] = {0};
	int i;
	for (i = 0; i < RUN; ++i)
	{
		uint index = prng(20);
		table[index] = ++table[index];
	}

	int j;
	for (j = 0; j < 20; ++j)
	{
		printf(1, "%d\n", table[j]);
	}

	exit();
}
