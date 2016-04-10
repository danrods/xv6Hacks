#include "types.h"
#include "user.h"

#define RUN 100



int 
main(int argc, char *argv[]) {
	int table[20] = {0};

	for (int i = 0; i < RUN; ++i)
	{
		uint index = prng(20);
		table[index] += table[index];
	}

	for (int i = 0; i < 20; ++i)
	{
		printf(1, "%d\n", table[i]);
	}

	exit();
}
