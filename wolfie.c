#include "user.h"

int 
main(int argc, char *argv[]) {
	void buf[4096];
	uint size = 4096;

	wolfie (&buf, size);

	printf("%s\n", (char *)buf);

}
