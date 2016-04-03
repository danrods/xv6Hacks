#include "types.h"
#include "user.h"

#define RANNUMS[0xFFFFFFFF]
#define RUN 0x48964

int 
main(int argc, char *argv[]) {
	
	for (int i = 0; i < RUN; ++i)
	{
		uint index = getSeeds(*ptable);
		RANNUMS[index] += 1;
	}


	for (int i = 0; i < RUN; ++i)
	{
		cprintf("%d : %d", i, RANNUMS[i]);
	}

	exit();
}
