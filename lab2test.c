#include "types.h"
#include "user.h"



int
main (int agrc, char *argv[]) {
	printf(1, "Trying Lab 2 Test\n");

	char* GOODADD = (char*) malloc(10);
	strcpy(GOODADD,"Hello");
	printf(1, "%s\n", GOODADD);
	free(GOODADD);
	
	char* BADADD = (char*) 0xFFF00FFF;
	printf(1,"%s\n", BADADD);

	exit();
}
