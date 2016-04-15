#include "types.h"
#include "user.h"


int
main(void)
{
	int pid;

	int i,
		niceval[4] = {19, 5, 0, -12};

for(i = 0; i < 4; i++){
	if((pid = fork()) < 0){// Error
		printf(1, "Error forking!\n");
	}
	else if(pid == 0){ //Child
	  nice(niceval[i]);
	  	printProcess();
	  	int j;
 		for(j=0; j < 50; j++){
 			printf(2, "CHILD : Nice Value ==> %d\n", niceval[i]);
 		}
 		exit();
	}

}


	while(wait() >= 0)
		;


  exit();
  return 0;
}


