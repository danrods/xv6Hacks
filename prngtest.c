#include "types.h"
#include "user.h"

#define RUN 100



int 
main(int argc, char *argv[]) {
	int i;
	for (i = 0; i < RUN; ++i)
	{
		uint index = prng(20);
		printf(1, "%d : %d \n", i, index);
	}

	exit();
}
