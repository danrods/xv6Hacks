#include "types.h"
#include "user.h"

#define GOODNICE 15
#define BADNICE  -80
void nicetestpositive(void);
void nicetestnegative(void);

int
main(void) {
	nicetestpositive();
	nicetestnegative();

	exit();
}

// nice positive test
void
nicetestpositive() {
	int nice_value;

	nice_value = nice(GOODNICE);

	if(nice_value >= 100 || nice_value < 140) {
		printf(1, "POSITIVE: :) Nice function works!\n");
	}
	else {
		printf(1, "POSITIVE: :( Nice test fails!\n");
	}
}

// nice negative test
void 
nicetestnegative() {
	int nice_value;

	nice_value = nice(BADNICE);

	if(nice_value >= 100 || nice_value < 140) {
		printf(1, "NEGATIVE: :) Nice function works!\n");
	}
	else {
		printf(1, "NEGATIVE: :( Nice test fails!\n");
	}
}

