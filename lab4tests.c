#include "types.h"
#include "user.h"

int
main(void) {
	static const char DIRNAME = "newDir";
	static const char FILENAME = "newFile";
	static const char LARGEFILE = "LargeFile";
	static const int SIZE = 128000;	 // 16KB

	// First Heuristic Test
	printf("Test I: a new directory placement");
	printBStats();

	int path = createDir(*DIRNAME);

	// Not created
	if (path == -1)
	{
		printf("ERROR: Failed to create the directory");
		exit();
	}
	else {
		printBStats();
	}

	// Second Heuristic Test
	printf("Test II: a new file placement");

	int file = createFile(*FILENAME);
	if (file == -1)
	{
		printf("ERROR: Failed to create the file");
		exit();
	}
	else 
	{
		printBStats();
	}


	// Third Heuristic Test
	printf("Test II: a new file placement");
	int bigFile = createFile(*FILENAME, SIZE);
	if (bigFile == -1)
	{
		printf("ERROR: Failed to create the big file");
		exit();
	}
	else {
		printBStats();
	}


	exit();
}


