#include "user.h"

#define GOODNICE = 15;
#define BADNICE  = -80;

void
main(void) {
	nicetestpositive();
	nicetestnegative();

	exit();
}

// nice positive test
void
nicetestpositive() {

	nice(GOODNICE);

	if(*proc->nice >= 100 || *proc->nice < 140) {
		cprintf("Nice function works!\n");
		exit();
	}
	else {
		cprintf("Nice test fails!\n");
		exit();
	}
}

// nice negative test
void nicetestnegative() {
	nice(BADNICE);

	if(*proc->nice >= 100 || *proc->nice < 140) {
		cprintf("Nice function works!\n");
		exit();
	}
	else {
		cprintf("Nice test fails!\n");
		exit();
	}
}