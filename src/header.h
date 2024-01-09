#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/// @brief Struct to represent Date
typedef struct
{
    int year;
    int month;
    int day;
} Date;

/// @brief struct type to represent a DB Account Record
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

typedef struct
{
    int TransactionID;
    char username[255];
    int userId;
    int AccID;
    double TransAmt;
} TransRecord;

/// @brief struct type to represent a DB Account Record
struct User
{
    char name[500];
    char password[500];
};

void loginMenu();
void CreateNewUser();
int checkUsernameExists(const char *userName);
bool HasOnlyDigits(const char *str);
bool IsValidAccountType(const char *str);
int GetUserIdFromUsername(const char *username);
void LogToFile(const char *filename, const char *text);
void trim(char *str);
char *displayAccountInformation(char *date, char *accountType, double amount);
void errprint(char *errmsg);
char *getTodaysDateAsString();
void CreateNewAcc(struct User u);
void checkAllAccounts(struct User u);
void checkSpecificAcc(struct User u);
void mainMenu(struct User u);
void UpdateAccInfo(struct User u);
void startMenu();
int checkAccIDExist(char *id);
void DelAcc(struct User u);
void TransferAcc(struct User u);
int Deposit(struct User u, AccountRecord *a, int Amt);
int Withdraw(struct User u, AccountRecord *a, int Amt);
void MakeTransaction(struct User u);
bool isValidDate(const char *dateString);
void mvprintenter(char *str);
double calculateInterestGains(const char *inputDate, double initialBalance);