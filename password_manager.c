// header include directives
#include <stdio.h> // standard input/output library
#include <stdlib.h> // standard c library
#include <string.h> // standard string handling library
#include <sys/stat.h> 
#include <errno.h>

// constant define directives
#define DATA_PATH "./data"
#define PROFILE_NAMES_PATH "./data/profiles"

// #if !(defined(PORTABLE) || defined(DEBUG))
// 	#ifdef _WIN32 // "_WIN32" is implicitly defined in most Windows compilers
// 		#undef DATA_PATH
// 		#undef PROFILE_NAMES_PATH
// 		#define DATA_PATH "%appdata%\\roaming\\password_manager\\data" /* The % doesn't need to be escaped because it's not going to be printed */
// 		#define PROFILE_NAMES_PATH "%appdata%\\roaming\\password_manager\\data\\profiles"
// 	#endif

// 	#ifdef __unix__ // "__unix__" is implicitly defined in most Unix/Linux compilers
// 		#undef DATA_PATH
// 		#undef PROFILE_NAMES_PATH
// 		#define DATA_PATH "~/.password_manager/data"
// 		#define PROFILE_NAMES_PATH "~/.password_manager/data/profiles"
// 	#endif
// #endif

#define MAX_PROFILE_NAME_LENGTH 256
#define MAX_PASSPHRASE_LENGTH 256
#define MAX_PROFILES 256
#define MAX_SITES_PER_PROFILE 1024
#define MAX_ACCOUNTS_PER_SITE 256

// struct definitions

// function prototypes
int list_profiles();
int extract_profile_data(char to[MAX_PROFILES][MAX_PROFILE_NAME_LENGTH]);
void create_profile();
void select_profile(int profile);
int list_websites(int of_user);
int list_accounts(int of_user, int for_site);

// global variables
FILE* data_ptr;
int profiles_size;
// int profiles_offsets[MAX_PROFILES];

// compile the program with the "gcc -o password_manager password_manager.c -D DEBUG" to enable debug mode 
// (use "#ifdef DEBUG" directives to print extra information/enable special inputs to assist in testing)

int main(int argc, char** argv)
{
	// set up directories
	// struct stat st = {0};
	// #if ((defined(_WIN32)) && !(defined(PORTABLE) || defined(DEBUG)))
	// if (stat("%appdata%\\roaming\\password_manager", &st) == -1)
	// {
	// 	mkdir("%appdata%\\roaming\\password_manager", S_IRWXU);
	// }
	// #endif
	// #if ((defined(__unix__)) && !(defined(PORTABLE) || defined(DEBUG)))
	// if (stat("~/.password_manager", &st) == -1)
	// {
	// 	mkdir("~/.password_manager", S_IRWXU);
	// }
	// #endif

	data_ptr = fopen(DATA_PATH, "a+");

	printf("=========================================\n");
	printf("|                                       |\n");
	printf("|           [password manager]          |\n");
	printf("| by Cole DeVlaeminck and Lucas England |\n");
	#ifdef DEBUG
	printf("|                                       |\n");
	printf("|           DEBUG MODE ACTIVE           |\n");
	#endif
	printf("|                                       |\n");
	printf("=========================================\n\n");
	
	int sel = 0;

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
	if (sel > 0)
	{
		select_profile(sel);
	}
	
	fclose(data_ptr);
	return 0;
}

// function definitions
int extract_profile_data(char to[MAX_PROFILES][MAX_PROFILE_NAME_LENGTH]) 
{
	int profile_tally = 0;
	char line[MAX_PROFILE_NAME_LENGTH + MAX_PASSPHRASE_LENGTH + 1];
	rewind(data_ptr);
	while (fgets(line, sizeof(line), data_ptr))
	{
		if (line[0] != '\t')
		{
			strcpy(to[profile_tally], line);
			++profile_tally;
		}
	}
	return profile_tally;
}

int list_profiles() // lists out the various profiles and prompts the user to select one, returning the result.
{
	int sel;
	printf("Select a profile by typing the number to the left of that profile:\n");
	
	char pnames[MAX_PROFILES][MAX_PROFILE_NAME_LENGTH];
	int pcount = extract_profile_data(pnames);

	for (int i = 0; i < pcount; ++i)
	{
		char name[MAX_PROFILE_NAME_LENGTH]; 
		strcpy(name, strtok(pnames[i], " "));
		printf("    %d: %s\n", i + 1, name);
	}

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
	printf("Profile passphrase (max %d characters) (Make sure it's secure -- there's a reason it's called a passPHRASE here!):\n> ", MAX_PASSPHRASE_LENGTH - 1);
	char profile_pass[MAX_PASSPHRASE_LENGTH];
	scanf("%s", profile_pass);
	fseek(data_ptr, 0, SEEK_END);
	// char profile_id[MAX_PROFILE_NAME_LENGTH + MAX_PASSPHRASE_LENGTH + 3];
	fprintf(data_ptr, "%s %s\n", profile_name, profile_pass);
	
	// hacky and probably inadvisable method of forcing changes to actually write to disq
	fclose(data_ptr);
	data_ptr = fopen(DATA_PATH, "a+");
}

void select_profile(int profile)
{
	// int sel;

	// sel = list_websites(profile);
}

int list_websites(int of_user)
{
	int sel = 0;

	return sel;
}

int list_accounts(int of_user, int for_site)
{
	int sel = 0;

	return sel;
}
