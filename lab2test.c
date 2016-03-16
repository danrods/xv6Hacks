#include "user.h"

char* BADADD = (char*) 0xFFFFFFFF;

int
main (int agrc, char *argv[]) {
	printf(1,"%s\n", BADADD);

	exit();
}
