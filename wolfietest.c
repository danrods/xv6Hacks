#include "types.h"
#include "user.h"
#include "stat.h"

#define MAX_SIZE 6000


int 
main(int argc, char *argv[]) {
	void* buffer = malloc(MAX_SIZE);
	if(wolfie (buf, MAX_SIZE) > 0)  printf(1,"%s\n", buffer);

	free(buffer);
	exit();
}
