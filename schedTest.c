#include "types.h"
#include "user.h"


void viewPrints(void);
void viewProcessDump(void);

int
main(void)
{

  viewPrints();
  viewProcessDump();

  exit();
  return 0;
}

void 
viewPrints(void){

	int pid;

	int i,
		niceval[4] = {19, 5, 0, -12};

	for(i = 0; i < 4; i++){
		if((pid = fork()) < 0){// Error
			printf(1, "Error forking!\n");
		}
		else if(pid == 0){ //Child
		  nice(niceval[i]);
		  	//printProcess();
		  	int j;
	 		for(j=0; j < 50; j++){
	 			printf(2, "CHILD : Nice Value ==> %d\n", niceval[i]);
	 		}
	 		exit();
		}

	}


		while(wait() >= 0)
			;

		printf(2, "\n\n\n\n");

}

void 
viewProcessDump(void){

	int pid;

	int i,
		niceval[4] = {19, 5, 0, -12};

	for(i = 0; i < 4; i++){
		if((pid = fork()) < 0){// Error
			printf(1, "Error forking!\n");
		}
		else if(pid == 0){ //Child
		  	nice(niceval[i]);
		  	printf(2, "-----Before Child %s-----\n", niceval[i]);
		  	printProcess();

		  	int j;
	 		for(j=0; j < 50; j++)
	 			;

	 		printf(2, "-----After Child %s------\n", niceval[i]);
		  	printProcess();

	 		exit();
		}

	}


		while(wait() >= 0)
			;


		printf(2, "\n\n\n\n");

}


