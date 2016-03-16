#include "types.h"
#include "user.h"



int
main (int agrc, char *argv[]) {
	printf(1, "Trying Lab 2 Test\n");

	char* GOODADD = malloc(1, 10);

	printf(1, "%s\n", GOODADD);

	char* BADADD = (char*) 0xFFFFF0F0;
	printf(1,"%s\n", BADADD);

	exit();
}
