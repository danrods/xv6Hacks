#include "types.h"
#include "user.h"

#define RUN 0x48964

int 
main(int argc, char *argv[]) {

	for (int i = 0; i < RUN; ++i)
	{
		uint index = prng();
		cprintf("%d : %d", i, index);
	}


	exit();
}
