#include "types.h"
#include "user.h"


void viewPrints(void);
void viewProcessDump(void);
void CPUBoundProc(void);
int computeFib(int i, int n);
int computePrime(int index, int n);
void doCalculation(int index, int n);

int
main(void)
{

  CPUBoundProc();
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
		niceval[4] = {19, 10, 0, -12};

	for(i = 0; i < 4; i++){
		if((pid = fork()) < 0){// Error
			printf(1, "Error forking!\n");
		}
		else if(pid == 0){ //Child
		  	nice(niceval[i]);
		  	printf(2, "-----Before Child %d-----\n", niceval[i]);
		  	printProcess();

		  	int j;
	 		for(j=0; j < 50; j++)
	 			;

	 		printf(2, "-----After Child %d------\n", niceval[i]);
		  	printProcess();

	 		exit();
	}

}


		while(wait() >= 0)
			;


		printf(2, "\n\n\n\n");

}

void
CPUBoundProc(void){

	int niceval[4] = {19, 10, 0, -12};

	int i, pid, n = 100;
	for(i=0; i < 4; i++){

		if((pid = fork()) < 0){
			printf(2, "Error forking!\n");
			exit();
		}
		else if(pid == 0){
			nice(niceval[i]);
			doCalculation(niceval[i], n);
			exit();
		}
	}

	while(wait() >= 0)
			;


		printf(2, "\n\n\n\n");

}


int computeFib(int i, int n){
		if(n%100 == 0){
			printf(1, "Computing Fib Process : %d, N=%d\n", i,n);
		}

		if(n > 1){
			return computeFib(i, n-2) + computeFib(i, n-1);
		}
		else return 1;

	}

int computePrime(int index, int n){

	if(n <= 1) return 0;
	else if(n <= 3) return 1;
	else if(n % 2 == 0 || n % 3 == 0) return 0;

	int i = 5;

	while((i * i) <= n){
		if(n % i == 0 || n % (i + 2) == 0) return 0;
		i = i + 6;
	}

	return 1;
}

void doCalculation(int index, int n){

	int i;
	int sum;
	for(i=0; i < n; i= i + 2){
		if(n%50 == 0){
			printf(1, "Proc with nice value %d running!: , N=%d\n", index,i);
			sum >>=20;
		}
		sum = n * sum;

	}
}


