// include directives
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// preprocessor constants
#define DATA_PATH "./data/data.txt"

#ifdef _WIN32 // "_WIN32" is implicitly defined in most Windows compilers
#undef DATA_PATH
#define DATA_PATH "%appdata%\\roaming\\password_manager\\data\\data.txt" /* The %s don't need to be escaped because it's not going to be printed */
#endif

#ifdef __unix__ // "__unix__" is implicitly defined in most Unix/Linux compilers
#undef DATA_PATH
#define DATA_PATH "~/.password_manager/data.txt"
#endif

#ifdef PORTABLE
#undef DATA_PATH
#define DATA_PATH "./data/data.txt"
#endif

#ifdef DEBUG
#undef DATA_PATH
#define DATA_PATH "./data/data.txt"
#endif

#define MAX_PROFILE_NAME_LENGTH 256
#define MAX_PASSPHRASE_LENGTH 256
#define MAX_PROFILES 256
#define MAX_SITES_PER_PROFILE 1024
#define MAX_ACCOUNTS_PER_SITE 256


// struct definitions


// function prototypes
void extract_profile_offsets(int to[MAX_PROFILES]);
int list_profiles();
void extract_profile_names(char to[MAX_PROFILES][MAX_PROFILE_NAME_LENGTH]);
void create_profile();
int list_websites(int of_user);
int list_accounts(int of_user, int for_site);

// global variables
FILE* data_ptr;
FILE* data_txt_ptr;
int profiles_size;
// int* profiles_offsets;

// compile the program with the "gcc -o password_manager password_manager.c -D DEBUG" to enable debug mode 
// (use "#ifdef DEBUG" directives to print extra information/enable special inputs to assist in testing)

int main(int argc, char** argv)
{
	data_ptr = fopen(DATA_PATH, "a+b");

	printf("=========================================\n");
	printf("|           [password manager]          |\n");
	printf("| by Cole DeVlaeminck and Lucas England |\n");
	#ifdef DEBUG
	printf("|                                       |\n");
	printf("|           DEBUG MODE ACTIVE           |\n");
	#endif
	printf("=========================================\n\n");
	
	int sel;

	// struct flex_int_array profiles_offsets = extract_profile_offsets();
	// for (int i = 0; i < profiles_offsets.size; ++i)
	// {
	// 	printf("%d: \n", profiles_offsets.data[i]);
	// }

	sel = list_profiles();
	while (sel == 0)
	{
		create_profile();
		sel = list_profiles();
	}
	
	fclose(data_ptr);
	return 0;
}

// function definitions
void extract_profile_offsets(int to[MAX_PROFILES])
{
	int profile_tally = 0;
	rewind(data_ptr);
	for (int i = 0; i < MAX_PROFILES; ++i)
	{
		char* line;
		fscanf(data_ptr, "%s", line);
		if (line[0] != '\t') 
		{
			++profile_tally;
		}
	}
	return;
}

void extract_profile_names(char to[MAX_PROFILES][MAX_PROFILE_NAME_LENGTH]) 
{
	return;
}

int list_profiles() // lists out the various profiles and prompts the user to select one, returning the result.
{
	int sel;
	printf("Select a profile by typing the number to the left of that profile:\n");

	printf("    0: [create new profile]\n");
	printf("   -1: [quit]\n> ");
	scanf("%d", &sel);
	return sel;
}

void create_profile()
{
	printf("Profile name (max %d characters):\n> ", MAX_PROFILE_NAME_LENGTH - 1);
	char profile_name[MAX_PROFILE_NAME_LENGTH];
	scanf("%s", profile_name);
	printf("Profile passphrase (max %d characters):\n> ", MAX_PASSPHRASE_LENGTH - 1);
	char profile_pass[MAX_PASSPHRASE_LENGTH];
	scanf("%s", profile_pass);
	fseek(data_ptr, 0, SEEK_END);
	char profile_id[MAX_PROFILE_NAME_LENGTH + MAX_PASSPHRASE_LENGTH + 3];
	fprintf(data_ptr, "%s %s\n", profile_name, profile_pass);
}

int list_websites(int of_user)
{
	int sel;

	return sel;
}

int list_accounts(int of_user, int for_site)
{
	int sel;

	return sel;
}
