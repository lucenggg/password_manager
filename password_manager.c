// include directives
#include <stdio.h>

// function prototypes


// compile the program with the "gcc -o password_manager password_manager.c -D DEBUG" to enable debug mode 
// (use "#ifdef DEBUG" directives to print extra information/enable special inputs to assist in testing)

int main()
{
	#ifdef DEBUG
	printf("===== DEBUG MODE ACTIVE =====\n");
	#endif
	
	return 0;
}

// function definitions

