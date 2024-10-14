// include directives
#include <stdio.h>
#include <stdlib.h>

// preprocessor constants
#define DATA_PATH "./data.bin"
#ifdef _WIN32 // "_WIN32" is implicitly defined in most Windows compilers
#undef DATA_PATH
#define DATA_PATH "%appdata%/roaming/password_manager/data.bin"
#endif
#ifdef __unix__ // "__unix__" is implicitly defined in most Unix/Linux compilers
#undef DATA_PATH
#define DATA_PATH "~/.password_manager/data.bin"
#endif
#ifdef PORTABLE
#undef DATA_PATH
#define DATA_PATH "data.bin"
#endif

#ifdef DEBUG
#undef DATA_PATH
#define DATA_PATH "data.bin"
#define DEBUG_DATA_PATH "data_text.txt"
#endif

// function prototypes
int list_profiles();
int list_websites(int of);
int list_accounts(int of_user, int for_site);

// compile the program with the "gcc -o password_manager password_manager.c -D DEBUG" to enable debug mode 
// (use "#ifdef DEBUG" directives to print extra information/enable special inputs to assist in testing)

int main(int argc, char** argv)
{
	FILE* data_ptr, data_txt_ptr; 
	data_ptr = fopen(DATA_PATH, "a+b");
	#ifdef DEBUG
	data_txt_ptr = fopen(DATA_PATH, "a+");
	#endif

	#ifdef DEBUG
	printf("===== DEBUG MODE ACTIVE =====\n");
	#endif
	

	fclose(data_ptr);
	#ifdef DEBUG
	fclose(data_txt_ptr);
	#endif
	return 0;
}

// function definitions
int list_profiles() // lists out the various profiles and prompts the user to select one, returning the result.
{
	int sel;

	return sel;
}

int list_websites(int of)
{
	int sel;

	return sel;
}

int list_accounts(int of_user, int for_site)
{
	int sel;

	return sel;
}
