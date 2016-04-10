#include "types.h"
#include "user.h"

#define RUN 0x48964

int 
main(int argc, char *argv[]) {
	int i;
	for (i = 0; i < RUN; ++i)
	{
		int index = prng();
		printf(1, "%d : %d", i, index);
	}

	exit();
}
