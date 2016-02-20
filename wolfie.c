#include "user.h"
#include "types.h"
#include "stat.h"

int 
main(int argc, char *argv[]) {
	char buf[4096];
	uint size = 4096;

	wolfie ((void *)&buf, size);

	printf("%s\n", buf);

	return 0;
}
