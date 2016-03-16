#include "types.h"
#include "user.h"



int
main (int agrc, char *argv[]) {
	printf(1, "Trying Lab 2 Test\n");
	char* BADADD = (char*) 0xFFFFFFFF;
	printf(1,"%s\n", BADADD);

	exit();
}
