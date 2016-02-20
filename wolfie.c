#include "types.h"
#include "user.h"
#include "stat.h"

#define MAX_SIZE 4096

int 
main(int argc, char *argv[]) {

	char buffer[MAX_SIZE];
	uint size = MAX_SIZE;
	void* buf = (void*) &buffer;

	printf(1, "Calling Wolfie");
	
	wolfie (buf, size);

	printf(1,"%s\n", buffer);

	return 0;
}
