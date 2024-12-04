// a promising-looking encryption library
// https://github.com/jedisct1/libsodium?tab=readme-ov-file

//-I cygdrive\c\Users\Cdevl\Projects\PasswordManagerGitPull\vcpkg-master\installed\x64-windows\include


// header include directives
#include <stdio.h> // standard input/output library
#include <stdlib.h> // standard c library
#include <string.h> // standard string handling library
#include <sys/stat.h> 
#include <errno.h> 
#include <termios.h>
#include <sodium.h>

// constant define directives

// filepaths
#define DATA_PATH "./data"
#define TEMP_DATA_PATH "./temp"
// #define PROFILE_NAMES_PATH "./data/profiles"
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

// array sizes
#define MAX_PROFILE_NAME_LENGTH 256
#define MAX_PASSPHRASE_LENGTH 256
#define MAX_PROFILES 256
#define MAX_SITES_PER_PROFILE 1024
#define MAX_SITE_NAME_LENGTH 256
#define MAX_SITE_URL_LENGTH 2048 // ~2000 seems to be something of an unofficial soft limit for url lengths
#define MAX_ACCOUNTS_PER_SITE 256
#define MAX_ACCOUNT_USERNAME_LENGTH 256
#define MAX_ACCOUNT_PASSWORD_LENGTH 256
#define GENERATED_PASSWORD_LENGTH 32
#define MAX_LINE_LENGTH 4096
#define MAX_FILE_LINES (MAX_PROFILES + (MAX_PROFILES * MAX_SITES_PER_PROFILE) + (MAX_PROFILES * MAX_SITES_PER_PROFILE * MAX_ACCOUNTS_PER_SITE))

#define MAX_PASSPHRASE_OPS 3
#define MAX_PASSPHRASE_MEM 256

// struct definitions

// function prototypes
// profiles
int list_profiles();
int extract_profile_data(char to[MAX_PROFILES][MAX_LINE_LENGTH]);
void create_profile();
void delete_profile(int profile);
void select_profile(int profile);

// sites
int list_sites(int of_user);
int extract_site_data(int of_user, char to[MAX_SITES_PER_PROFILE][MAX_LINE_LENGTH]);
void add_site(int to_user);
void rename_profile(int of_user);
void change_passphrase(int of_user);
void delete_site(int of_user);
void select_site(int of_user, int site);

// accounts
int list_accounts(int of_user, int for_site);
int extract_account_data(int of_user, int for_site, char to[MAX_ACCOUNTS_PER_SITE][MAX_LINE_LENGTH]);
void create_account(int for_user, int for_site);
void remove_account(int of_user, int for_site, int account);
void select_account(int of_user, int for_site, int account);
int list_account_settings(int of_user, int for_site, int account);

// utility functions
void insert_line_before(char new_line[MAX_LINE_LENGTH], char before[MAX_LINE_LENGTH]);
void insert_line_between(char new_line[MAX_LINE_LENGTH], char after[MAX_LINE_LENGTH], char before[MAX_LINE_LENGTH]);
void erase_between(char after[MAX_LINE_LENGTH], char before[MAX_LINE_LENGTH]);
void replace_line(char with[MAX_LINE_LENGTH], char which[MAX_LINE_LENGTH]);
void generate_password(char out[GENERATED_PASSWORD_LENGTH]);
void encrypt(char *string, int len);
void decrypt(char *string, int len);
void hide_echo();
void unhide_echo();

// global variables
FILE* data_ptr;
int profile_count;
struct termios term;
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

	// initilization stuff
	data_ptr = fopen(DATA_PATH, "a+");
	tcgetattr(fileno(stdin), &term);
	if (sodium_init() == -1)
	{
		printf("ERROR: libsodium could not be initialized\n");
		return 1;
	}

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
	while (sel != -1)
	{
		switch (sel)
		{
		case 0:
			create_profile();
			break;
		
		default:
			select_profile(sel - 1);
			break;
		}
		sel = list_profiles();
	}
	
	fclose(data_ptr);
	return 0;
}

// function definitions
int list_profiles() // lists out the various profiles and prompts the user to select one, returning the result.
{
	int sel;
	printf("\nSelect a profile by typing the number to the left of that profile:\n");
	
	char pnames[MAX_PROFILES][MAX_LINE_LENGTH];
	int pcount = extract_profile_data(pnames);

	for (int i = 0; i < pcount; ++i)
	{
		char name[MAX_PROFILE_NAME_LENGTH]; 
		strcpy(name, strtok(pnames[i], " "));
		printf("%5d: %s\n", i + 1, name);
	}

	printf("    0: [create new profile]\n");
	printf("   -1: [quit]\n> ");
	scanf("%d", &sel);
	return sel;
}

int extract_profile_data(char to[MAX_PROFILES][MAX_LINE_LENGTH]) 
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

void create_profile()
{
	char profile_name[MAX_PROFILE_NAME_LENGTH];
	// printf("[Type \"c\" at any point to cancel.]\n");
	printf("\nProfile name (or \"c\" to cancel) (max %d characters):\n> ", MAX_PROFILE_NAME_LENGTH - 1);
	scanf("%s", profile_name);

	// TODO prevent duplicate profile names
	// in progress
	if (strcmp(profile_name, "c") == 0)
	{
		return;
	}
	char profiles[MAX_PROFILES][MAX_LINE_LENGTH];
	int profiles_count = extract_profile_data(profiles);
	for (int i = 0; i < profiles_count; ++i)
	{
		if (strcmp(profile_name, strtok(profiles[i], " ")) == 0)
		{
			printf("Profile already exists!\n\n");
			return;
		}
	}

	hide_echo();

	char profile_pass[MAX_PASSPHRASE_LENGTH];
	printf("Profile passphrase (or \"c\" to cancel) (max %d characters) \n", MAX_PASSPHRASE_LENGTH - 1);
	printf("(Make sure it's secure -- there's a reason it's called a passPHRASE here!):\n> ");
	scanf("%s", profile_pass);

	if (strcmp(profile_pass, "c") == 0)
	{
		unhide_echo();
		return;
	}

	char profile_cpass[MAX_PASSPHRASE_LENGTH];
	printf("\nConfirm passphrase (or \"c\" to cancel):\n> ");
	scanf("%s", profile_cpass);

	if (strcmp(profile_cpass, "c") == 0)
	{
		unhide_echo();
		return;
	}

	while (strcmp(profile_pass, profile_cpass) != 0)
	{
		#ifdef DEBUG
		printf("\n[DEBUG] Pass: %s, CPass: %s, strcmp Verdict: %d", profile_pass, profile_cpass, strcmp(profile_pass, profile_cpass));
		#endif

		printf("\nPASSPHRASES DO NOT MATCH\n");
		printf("Profile passphrase (or \"c\" to cancel) (max %d characters):\n> ", MAX_PASSPHRASE_LENGTH - 1);
		scanf("%s", profile_pass);

		if (strcmp(profile_pass, "c") == 0)
		{
			unhide_echo();
			return;
		}

		printf("\nConfirm passphrase (or \"c\" to cancel):\n> ");
		scanf("%s", profile_cpass);

		if (strcmp(profile_cpass, "c") == 0)
		{
			unhide_echo();
			return;
		}
	}

	unhide_echo();

	fseek(data_ptr, 0, SEEK_END);
	// char profile_id[MAX_PROFILE_NAME_LENGTH + MAX_PASSPHRASE_LENGTH + 3];
	char hashed[crypto_pwhash_STRBYTES];
	int err = crypto_pwhash_str(hashed, profile_pass, strlen(profile_pass), crypto_pwhash_OPSLIMIT_MODERATE, crypto_pwhash_MEMLIMIT_MODERATE);
	if (err)
	{
		printf("\nERROR: could not encrypt password\n");
		#ifdef DEBUG
		printf("passphrase: %s\nlength: %d\nhash: %s\n", profile_pass, (int) strlen(profile_pass), hashed);
		#endif
		return;
	}
	fprintf(data_ptr, "%s %s\n", profile_name, hashed);
	
	// hacky and probably inadvisable method of forcing changes to actually write to disq
	fclose(data_ptr);
	data_ptr = fopen(DATA_PATH, "a+");
}

void select_profile(int profile)
{
	// password
	hide_echo();

	char pnames[MAX_PROFILES][MAX_LINE_LENGTH];
	profile_count = extract_profile_data(pnames);

	// get profile name
	char profile_name[MAX_PROFILE_NAME_LENGTH];
	strcpy(profile_name, strtok(pnames[profile], " "));
	// get correct password
	char profile_cpass[MAX_PASSPHRASE_LENGTH];
	strcpy(profile_cpass, strtok(NULL, "\n"));

	char profile_pass[MAX_PASSPHRASE_LENGTH];
	printf("Passphrase for %s (or \"c\" to cancel):\n> ", profile_name);
	scanf("%s", profile_pass);

	if (strcmp(profile_pass, "c") == 0)
	{
		unhide_echo();
		return;
	}

	while (crypto_pwhash_str_verify(profile_pass, profile_cpass, strlen(profile_cpass)) != 0)
	{
		// #ifdef DEBUG
		// printf("\n[DEBUG] Pass: %s, CPass: %s, strcmp Verdict: %d", profile_pass, profile_cpass, strcmp(profile_pass, profile_cpass));
		// #endif

		printf("\nPASSPHRASES DO NOT MATCH\n");
		printf("Profile passphrase (or \"c\" to cancel) (max %d characters):\n> ", MAX_PASSPHRASE_LENGTH - 1);
		scanf("%s", profile_pass);

		if (strcmp(profile_pass, "c") == 0)
		{
			unhide_echo();
			return;
		}
	}

	unhide_echo();
	
	// sites loop
	int sel = list_sites(profile);
	while (sel != -1)
	{
		switch (sel)
		{
		case 0:
			add_site(profile);
			break;
		
		case -2:
			rename_profile(profile);
			break;

		case -3:
			change_passphrase(profile);
			break;

		case -4:
			delete_profile(profile);
			break;

		case -1:
			return;

		default:
			select_site(profile, sel - 1);
			break;
		}
		sel = list_sites(profile);
	}
}

int list_sites(int of_user)
{
	int sel;
	printf("\nSelect a site by typing the number to the left of that site:\n");
	
	char snames[MAX_SITES_PER_PROFILE][MAX_LINE_LENGTH];
	int scount = extract_site_data(of_user, snames);

	for (int i = 0; i < scount; ++i)
	{
		char name[MAX_SITE_NAME_LENGTH], url[MAX_SITE_URL_LENGTH];
		// url is stored before name in file so i can use spaces as a delimiter 
		// while also allowing spaces in the site name
		strcpy(url, strtok(snames[i], " "));
		strcpy(name, strtok(NULL, "\n"));
		printf("%5d: %s (%s)\n", i + 1, name, url);
	}

	printf("    0: [add new site]\n");
	printf("   -2: [rename profile]\n");
	printf("   -3: [change passphrase]\n");
	printf("   -4: [Delete profile]\n");
	printf("   -1: [return]\n> ");
	scanf("%d", &sel);
	return sel;
}

int extract_site_data(int of_user, char to[MAX_SITES_PER_PROFILE][MAX_LINE_LENGTH])
{
	int profile_tally = 0, site_tally = 0;
	char line[MAX_LINE_LENGTH];
	rewind(data_ptr);
	while (fgets(line, sizeof(line), data_ptr) && profile_tally < of_user)
	{
		if (line[0] != '\t')
		{
			++profile_tally;
		}
	}
	while (fgets(line, sizeof(line), data_ptr) && line[0] == '\t')
	{
		if (line[1] != '\t')
		{
			strcpy(to[site_tally], line + 1);
			++site_tally;
		}
	}
	return site_tally;
}

void add_site(int to_user)
{
	char site_name[MAX_SITE_NAME_LENGTH];
	// printf("[Type \"c\" at any point to cancel.]\n");
	printf("\nWebsite name (or \"c\" to cancel) (max %d characters):\n> ", MAX_SITE_NAME_LENGTH - 1);
	scanf("%s", site_name);

	// TODO prevent duplicate site names (per profile)
	if (strcmp(site_name, "c") == 0)
	{
		return;
	}

	char site_url[MAX_SITE_URL_LENGTH];
	printf("\nWebsite URL (or \"c\" to cancel) (max %d characters):\n> ", MAX_SITE_URL_LENGTH - 1);
	scanf("%s", site_url);

	if (strcmp(site_url, "c") == 0)
	{
		return;
	}

	char pnames[MAX_PROFILES][MAX_LINE_LENGTH];
	int pcount = extract_profile_data(pnames);
	#ifdef DEBUG
	for (int i = 0; i < pcount; ++i)
	{
		printf("profile #%d: %s", i, pnames[i]);
	}
	#endif
	if (to_user == pcount - 1)
	{
		fprintf(data_ptr, "\t%s %s\n", site_url, site_name);
	}
	else
	{
		// char line[MAX_LINE_LENGTH] = "\t";
		// strcat(line, site_url);
		// strcat(line, " ");
		// strcat(line, site_name);
		// strcat(line, "\n");
		char line[MAX_LINE_LENGTH] = "";
		sprintf(line, "\t%s %s\n", site_url, site_name);
		#ifdef DEBUG
		printf("printing line before %s\n", pnames[pcount - 1]);
		#endif
		insert_line_before(line, pnames[pcount - 1]);
	}
}

void rename_profile(int of_user)
{
	char new_name[MAX_PROFILE_NAME_LENGTH];
	printf("New profile name (or \"c\" to cancel) (max %d characters):\n> ", MAX_PROFILE_NAME_LENGTH - 1);
	scanf("%s", new_name);

	if (strcmp(new_name, "c") == 0)
	{
		return;
	}

	// prevent duplicate names
	char pnames[MAX_PROFILES][MAX_LINE_LENGTH];
	int pcount = extract_profile_data(pnames);
	for (int i = 0; i < pcount; ++i)
	{
		if (strcmp(new_name, strtok(pnames[i], " ")) == 0)
		{
			printf("Profile already exists!\n\n");
			return;
		}
	}

	extract_profile_data(pnames);

	char old_line[MAX_LINE_LENGTH];
	strcpy(old_line, pnames[of_user]);
	char passphrase[MAX_PASSPHRASE_LENGTH] = "";
	// skip profile name
	strcpy(passphrase, strtok(pnames[of_user], " "));
	// get correct passphrase
	strcpy(passphrase, strtok(NULL, "\n"));

	char new_line[MAX_LINE_LENGTH] = "";
	sprintf(new_line, "%s %s\n", new_name, passphrase);
	replace_line(new_line, old_line);
}

void change_passphrase(int of_user)
{
	hide_echo();

	char new_pass[MAX_PASSPHRASE_LENGTH];
	printf("Profile passphrase (or \"c\" to cancel) (max %d characters) \n", MAX_PASSPHRASE_LENGTH - 1);
	printf("(Make sure it's secure -- there's a reason it's called a passPHRASE here!):\n> ");
	scanf("%s", new_pass);

	if (strcmp(new_pass, "c") == 0)
	{
		unhide_echo();
		return;
	}

	char profile_cpass[MAX_PASSPHRASE_LENGTH];
	printf("\nConfirm passphrase (or \"c\" to cancel):\n> ");
	scanf("%s", profile_cpass);

	if (strcmp(profile_cpass, "c") == 0)
	{
		unhide_echo();
		return;
	}

	while (strcmp(new_pass, profile_cpass) != 0)
	{
		#ifdef DEBUG
		printf("\n[DEBUG] Pass: %s, CPass: %s, strcmp Verdict: %d", new_pass, profile_cpass, strcmp(new_pass, profile_cpass));
		#endif

		printf("\nPASSPHRASES DO NOT MATCH\n");
		printf("Profile passphrase (or \"c\" to cancel) (max %d characters):\n> ", MAX_PASSPHRASE_LENGTH - 1);
		scanf("%s", new_pass);

		if (strcmp(new_pass, "c") == 0)
		{
			unhide_echo();
			return;
		}

		printf("\nConfirm passphrase (or \"c\" to cancel):\n> ");
		scanf("%s", profile_cpass);

		if (strcmp(profile_cpass, "c") == 0)
		{
			unhide_echo();
			return;
		}
	}

	unhide_echo();

	char pnames[MAX_PROFILES][MAX_LINE_LENGTH];
	int pcount = extract_profile_data(pnames);

	char old_line[MAX_LINE_LENGTH];
	strcpy(old_line, pnames[of_user]);
	char name[MAX_PROFILE_NAME_LENGTH] = "";
	// get profile name
	strcpy(name, strtok(pnames[of_user], " "));

	char new_line[MAX_LINE_LENGTH] = "";
	sprintf(new_line, "%s %s\n", name, new_pass);
	replace_line(new_line, old_line);
}

void delete_profile(int profile)
{
	
}

void delete_site(int of_user)
{
	
}

void select_site(int of_user, int site)
{
	int sel = list_accounts(of_user, site);
	while (sel != -1)
	{
		switch (sel)
		{
		case 0:
			create_account(of_user, site);
			break;

		case -4:
			// TODO implement deleting sites
			break;

		case -3:
			// TODO implement changing site URL
			break;
		
		case -2:
			// TODO implement renaming sites
			break;

		case -1:
			return;

		default:
			select_account(of_user, site, sel - 1);
			break;
		}
		sel = list_accounts(of_user, site);
	}
}

int list_accounts(int of_user, int for_site)
{
	int sel = 0;
	printf("\nSelect an account entry by typing the number to the left of that entry:\n");
	
	char snames[MAX_ACCOUNTS_PER_SITE][MAX_LINE_LENGTH];
	int scount = extract_account_data(of_user, for_site, snames);

	for (int i = 0; i < scount; ++i)
	{
		char username[MAX_ACCOUNT_USERNAME_LENGTH], password[MAX_ACCOUNT_PASSWORD_LENGTH];
		// decrypt(snames[i] + 2, strlen(snames[i]));
		strcpy(username, strtok(snames[i], " "));
		strcpy(password, strtok(NULL, "\n"));
		printf("%5d: %s | %s\n", i + 1, username, password);
	}

	printf("    0: [add new account]\n");
	printf("   -2: [change site name]\n");
	printf("   -3: [change site url]\n");
	printf("   -4: [delete site]\n");
	printf("   -1: [return]\n> ");
	scanf("%d", &sel);
	return sel;
}

int extract_account_data(int of_user, int for_site, char to[MAX_ACCOUNTS_PER_SITE][MAX_LINE_LENGTH])
{
	int profile_tally = 0, site_tally = 0, account_tally = 0;
	char line[MAX_LINE_LENGTH];
	rewind(data_ptr);
	while (fgets(line, sizeof(line), data_ptr) && profile_tally < of_user)
	{
		if (line[0] != '\t')
		{
			++profile_tally;
		}
	}
	while (fgets(line, sizeof(line), data_ptr) && site_tally < for_site)
	{
		if (line[1] != '\t')
		{
			++site_tally;
		}
	}
	while (fgets(line, sizeof(line), data_ptr) && line[1] == '\t')
	{
		decrypt(line + 2, strlen(line + 2));
		strcpy(to[account_tally], line + 2);
		++account_tally;
	}
	return account_tally;
}

void create_account(int for_user, int for_site)
{
	char account_name[MAX_ACCOUNT_USERNAME_LENGTH];
	// printf("[Type \"c\" at any point to cancel.]\n");
	printf("\nAccount Username (or \"c\" to cancel) (max %d characters):\n> ", MAX_ACCOUNT_USERNAME_LENGTH - 1);
	scanf("%s", account_name);

	// TODO prevent duplicate account names (per site)
	if (strcmp(account_name, "c") == 0)
	{
		return;
	}

	char account_pass[MAX_SITE_URL_LENGTH];
	char choice;
	printf("\nWould you like to automatically generate a secure password? (y/n/c):\n> ");
	scanf(" %c", &choice);
	switch (choice)
	{
	case 'y':
		generate_password(account_pass);
		break;
	
	case 'n':
		printf("\nAccount password (or \"c\" to cancel) (max %d characters):\n> ", MAX_SITE_URL_LENGTH - 1);
		scanf("%s", account_pass);

		if (strcmp(account_pass, "c") == 0)
		{
			return;
		}
		break;
	
	default:
	case 'c':
		return;
	
	}

	char pnames[MAX_PROFILES][MAX_LINE_LENGTH];
	int pcount = extract_profile_data(pnames);
	#ifdef DEBUG
	for (int i = 0; i < pcount; ++i)
	{
		printf("profile #%d: %s", i, pnames[i]);
	}
	#endif
	char line[MAX_LINE_LENGTH] = "";
	sprintf(line, "\t\t%s %s\n", account_name, account_pass);
	encrypt(line, strlen(line));

	char snames[MAX_SITES_PER_PROFILE][MAX_LINE_LENGTH];
	int scount = extract_site_data(for_user, snames);

	if (for_user == pcount - 1 && for_site == scount - 1)
	{
		fprintf(data_ptr, "%s", line);
	}

	else
	{
		#ifdef DEBUG
		printf("printing line before %s\n", pnames[pcount - 1]);
		#endif
		insert_line_before(line, snames[scount - 1]);
	}
}

void remove_account(int of_user, int for_site, int account)
{

}

void generate_password(char out[GENERATED_PASSWORD_LENGTH])
{
	char pass[GENERATED_PASSWORD_LENGTH] = "placeholder";

	strcpy(out, pass);
}

void select_account(int of_user, int for_site, int account)
{
	int sel = list_account_settings(of_user, for_site, account);
	while (sel != -1)
	{
		switch (sel)
		{
		case 0:
			break;
		
		case 1:
			// TODO implement renaming sites
			break;

		case -1:
			return;

		default:
			break;
		}
		sel = list_account_settings(of_user, for_site, account);
	}	
}

int list_account_settings(int of_user, int for_site, int account)
{
	int sel = 0;
	printf("Options:\n");

	printf("    0: [change username]\n");
	printf("    1: [change password]\n");
	printf("    2: [regenerate password]\n");
	printf("   -1: [return]\n> ");
	scanf("%d", &sel);
	return sel;
}

void insert_line_before(char new_line[MAX_LINE_LENGTH], char before[MAX_LINE_LENGTH])
{
	// char new_file[MAX_FILE_LINES][MAX_LINE_LENGTH];
	FILE* temp_data_ptr = fopen(TEMP_DATA_PATH, "w");
	int lines_tally = 0;
	char current_line[MAX_LINE_LENGTH];
	rewind(data_ptr);
	// insert lines until "before" is found
	while (fgets(current_line, sizeof(current_line), data_ptr) && strcmp(current_line, before))
	{
		// strcpy(new_file[lines_tally], current_line);
		fprintf(temp_data_ptr, "%s", current_line);
		#ifdef DEBUG
		printf("copying line %d: \"%s\"\n", lines_tally, current_line);
		#endif
		++lines_tally;
	}
	// insert new line
	// strcpy(new_file[lines_tally], new_line);
	fprintf(temp_data_ptr, "%s", new_line);
	++lines_tally;
	// don't forget the line we wanted to insert our new line before
	// strcpy(new_file[lines_tally], current_line);
	fprintf(temp_data_ptr, "%s", current_line);
	// insert the rest of the file
	while (fgets(current_line, sizeof(current_line), data_ptr))
	{
		// strcpy(new_file[lines_tally], current_line);
		fprintf(temp_data_ptr, "%s", current_line);
		++lines_tally;
	}
	// put string array into file
	// rewind(data_ptr);
	// for (int i = 0; i < lines_tally; ++i)
	// {
	// 	fprintf(data_ptr, "%s", new_file[i]);
	// }
	fclose(temp_data_ptr);
	fclose(data_ptr);
	// remove(DATA_PATH);
	rename(TEMP_DATA_PATH, DATA_PATH);
	data_ptr = fopen(DATA_PATH, "a+");
}

void insert_line_between(char new_line[MAX_LINE_LENGTH], char after[MAX_LINE_LENGTH], char before[MAX_LINE_LENGTH])
{
	// char new_file[MAX_FILE_LINES][MAX_LINE_LENGTH];
	FILE* temp_data_ptr = fopen(TEMP_DATA_PATH, "w");
	int lines_tally = 0;
	char current_line[MAX_LINE_LENGTH];
	rewind(data_ptr);
	// insert lines until "after" is found
	while (fgets(current_line, sizeof(current_line), data_ptr) && strcmp(current_line, after))
	{
		// strcpy(new_file[lines_tally], current_line);
		fprintf(temp_data_ptr, "%s", current_line);
		#ifdef DEBUG
		printf("copying line %d: \"%s\"\n", lines_tally, current_line);
		#endif
		++lines_tally;
	}
	fprintf(temp_data_ptr, "%s", current_line);
	++lines_tally;
	// insert lines until "before" is found
	while (fgets(current_line, sizeof(current_line), data_ptr) && strcmp(current_line, before))
	{
		// strcpy(new_file[lines_tally], current_line);
		fprintf(temp_data_ptr, "%s", current_line);
		#ifdef DEBUG
		printf("copying line %d: \"%s\"\n", lines_tally, current_line);
		#endif
		++lines_tally;
	}
	// insert new line
	// strcpy(new_file[lines_tally], new_line);
	fprintf(temp_data_ptr, "%s", new_line);
	++lines_tally;
	// don't forget the line we wanted to insert our new line before
	// strcpy(new_file[lines_tally], current_line);
	fprintf(temp_data_ptr, "%s", current_line);
	// insert the rest of the file
	while (fgets(current_line, sizeof(current_line), data_ptr))
	{
		// strcpy(new_file[lines_tally], current_line);
		fprintf(temp_data_ptr, "%s", current_line);
		++lines_tally;
	}
	// put string array into file
	// rewind(data_ptr);
	// for (int i = 0; i < lines_tally; ++i)
	// {
	// 	fprintf(data_ptr, "%s", new_file[i]);
	// }
	fclose(temp_data_ptr);
	fclose(data_ptr);
	// remove(DATA_PATH);
	rename(TEMP_DATA_PATH, DATA_PATH);
	data_ptr = fopen(DATA_PATH, "a+");
}

void replace_line(char with[MAX_LINE_LENGTH], char which[MAX_LINE_LENGTH])
{
	// char new_file[MAX_FILE_LINES][MAX_LINE_LENGTH];
	FILE* temp_data_ptr = fopen(TEMP_DATA_PATH, "w");
	int lines_tally = 0;
	char current_line[MAX_LINE_LENGTH];
	rewind(data_ptr);
	// insert lines until "before" is found
	while (fgets(current_line, sizeof(current_line), data_ptr) && strcmp(current_line, which))
	{
		// strcpy(new_file[lines_tally], current_line);
		fprintf(temp_data_ptr, "%s", current_line);
		#ifdef DEBUG
		printf("copying line %d: \"%s\"\ncurrent_line: \"%s\", which: \"%s\"", lines_tally, current_line, current_line, which);
		#endif
		++lines_tally;
	}
	// insert new line
	// strcpy(new_file[lines_tally], new_line);
	fprintf(temp_data_ptr, "%s", with);
	++lines_tally;
	// strcpy(new_file[lines_tally], current_line);
	// fprintf(temp_data_ptr, "%s", current_line);
	// insert the rest of the file
	while (fgets(current_line, sizeof(current_line), data_ptr))
	{
		// strcpy(new_file[lines_tally], current_line);
		fprintf(temp_data_ptr, "%s", current_line);
		++lines_tally;
	}
	// put string array into file
	// rewind(data_ptr);
	// for (int i = 0; i < lines_tally; ++i)
	// {
	// 	fprintf(data_ptr, "%s", new_file[i]);
	// }
	fclose(temp_data_ptr);
	fclose(data_ptr);
	// remove(DATA_PATH);
	rename(TEMP_DATA_PATH, DATA_PATH);
	data_ptr = fopen(DATA_PATH, "a+");
}

void erase_between(char after[MAX_LINE_LENGTH], char before[MAX_LINE_LENGTH])
{
	FILE* temp_data_ptr = fopen(TEMP_DATA_PATH, "w");
	int lines_tally = 0;
	char current_line[MAX_LINE_LENGTH];
	rewind(data_ptr);
	
	// insert lines until "after" is found
	while (fgets(current_line, sizeof(current_line), data_ptr) && strcmp(current_line, after))
	{
		fprintf(temp_data_ptr, "%s", current_line);
		#ifdef DEBUG
		printf("copying line %d: \"%s\"\n", lines_tally, current_line);
		#endif
		++lines_tally;
	}
	fprintf(temp_data_ptr, "%s", current_line);
	++lines_tally;

	// insert lines until "before" is found
	while (fgets(current_line, sizeof(current_line), data_ptr) && strcmp(current_line, before))
	{
		/*fprintf(temp_data_ptr, "%s", current_line);
		#ifdef DEBUG
		printf("copying line %d: \"%s\"\n", lines_tally, current_line);
		#endif*/
		++lines_tally;
	}
	fprintf(temp_data_ptr, "%s", current_line);
	++lines_tally;

	//Insert the file
	while (fgets(current_line, sizeof(current_line), data_ptr))
	{
		fprintf(temp_data_ptr, "%s", current_line);
		++lines_tally;
	}
	
	fclose(temp_data_ptr);
	fclose(data_ptr);
	
	rename(TEMP_DATA_PATH, DATA_PATH);
	data_ptr = fopen(DATA_PATH, "a+");
}

// TODO implement encryption/decryption
void encrypt(char *string, int len)
{
	#ifdef DEBUG
	printf("TODO: ENCRYPTION HAS NOT BEEN IMPLEMENTED YET\n");
	#endif
}

void decrypt(char *string, int len)
{
	#ifdef DEBUG
	printf("TODO: DECRYPTION HAS NOT BEEN IMPLEMENTED YET\n");
	#endif
}

// Use this function to hide the user's input while they're typing a password.
void hide_echo()
{
	term.c_lflag &= ~ECHO;
	tcsetattr(fileno(stdin), 0, &term);
}

// Use this function to re-enable displaying the user's input.
void unhide_echo()
{
	term.c_lflag |= ECHO;
	tcsetattr(fileno(stdin), 0, &term);
	printf("\n");
}

void generateRandomPassword(){
    const char charset[] = "abcdefghijklmnopqrstuvwxyz" 
                           "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "0123456789"
                           "!@#$%^&*()_+";
	int length = 32;
	char password[length + 1];

	for(int i=0; i<length; i++)
	{
    	int key = rand() % (sizeof(charset) - 1);
    	password[i] = charset[key];
	}
	password[length] = '\0';

printf("Generated Password: %s", password);
}
