#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
struct Date
{
    int month, day, year;
};

// all fields for each record of an account
typedef struct
{
    int Accid;
    char username[255];
    int userId;
    char creationdate[255];
    char country[255];
    char phoneno[255];
    double balance;
    char accountType[255];
} AccountRecord;

//* struct for user to map from mysql database
struct User
{
    char name[500];
    char password[500];
};

// authentication functions
void loginMenu();
void CreateNewUser();
// utilities
int checkUsernameExists(const char *userName);
bool HasOnlyDigits(const char *str);
bool IsValidAccountType(const char *str);
int GetUserIdFromUsername(const char *username);
void LogToFile(const char *filename, const char *text);
void trim(char *str);
void errprint(char *errmsg);
char *getTodaysDateAsString();
// system function
void CreateNewAcc(struct User u);
void mainMenu(struct User u);
void checkAllAccounts(struct User u);
void UpdateAccInfo(struct User u);
void startMenu();
