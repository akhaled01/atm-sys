#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Date
{
    int month, day, year;
};

// all fields for each record of an account
struct Record
{
    int id;
    int userId;
    char name[100];
    char country[100];
    int phone;
    char accountType[10];
    int accountNbr;
    double amount;
    struct Date deposit;
    struct Date withdraw;
};

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
void disableEcho();
void enableEcho();
void trim(char *str);
void startMenu();
char *getTodaysDateAsString();
// system function
void CreateNewAcc(struct User u);
void mainMenu(struct User u);
void checkAllAccounts(struct User u);