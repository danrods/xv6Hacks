#include "types.h"
#include "user.h"
#include "stat.h"

#define MAX_SIZE 6000


int 
main(int argc, char *argv[]) {
	void* buffer = malloc(MAX_SIZE); //Need to malloc because stack bug only allocates up to max one page
	if(wolfie (buffer, MAX_SIZE) > 0)  printf(1,"%s\n", buffer);

	free(buffer);
	exit();
}
