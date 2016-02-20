#include "types.h"
#include "user.h"
#include "stat.h"

int 
main(int argc, char *argv[]) {
	char buffer[4096];
	uint size = 4096;
	void* buf = (void*) &buffer;

	wolfie (buf, size);

	cprintf("%s\n", buffer);

	return 0;
}
