#include "types.h"
#include "user.h"

int
main(void) {
	static const char DIRNAME = "newDir";
	static const char FILENAME = "newFile";


	printf("Test I: a new directory placement");
	printBStats();

	int file = mkdir(*DIRNAME);

	// Not created
	if (file == -1)
	{
		printf("ERROR: Failed to create the file");
		exit();
	}
	else {
		printBStats();
	}


	printf("Test II: a new file placement")
	

	exit();
}


