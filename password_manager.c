// include directives
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// preprocessor constants
#define DATA_PATH "./data.bin"
#ifdef _WIN32 // "_WIN32" is implicitly defined in most Windows compilers
#undef DATA_PATH
#define DATA_PATH "%appdata%\\roaming\\password_manager\\data.bin"
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

#define MAX_PROFILE_NAME_LENGTH 256
#define MAX_PASSPHRASE_LENGTH 256
#define MAX_PROFILES 256
#define MAX_SITES_PER_PROFILE 1024
#define MAX_ACCOUNTS_PER_SITE 256


// struct definitions
struct flex_int_array
{
	int* data;
	int size;
};


// function prototypes
int* extract_profile_offsets();
int list_profiles();
char** extract_profile_names();
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
	#ifdef DEBUG
	data_txt_ptr = fopen(DEBUG_DATA_PATH, "a+");
	#endif

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
	if (sel == 0)
	{
		create_profile();
	}
	
	fclose(data_ptr);
	#ifdef DEBUG
	fclose(data_txt_ptr);
	#endif
	return 0;
}

// function definitions
int* extract_profile_offsets()
{
	int out[MAX_PROFILES];
	int profile_tally = 0;
	rewind(data_ptr);
	for (int i = 0; i < 0; ++i)
	{
		char* line;
		fscanf(data_ptr, "%s", line);
		if (line[0] != '\t') 
		{
			++profile_tally;
		}
	}
	return out;
}

char** extract_profile_names() 
{
	char out[MAX_PROFILES][MAX_PROFILE_NAME_LENGTH];

	return out;
}

int list_profiles() // lists out the various profiles and prompts the user to select one, returning the result.
{
	int sel;
	printf("Select a profile by typing the number to the left of that profile:\n");
	printf("    0: [create new profile]\n> ");
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
