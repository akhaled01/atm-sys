#include <string.h>
#include <mysql/mysql.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include "header.h"
#include <time.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>

/// @brief Function to check if a username exists in the Users DB
/// @param userName Username to check against DB records
/// @return 0 on success, 3 OR 2 if DB conn ERR, 1 if Username found
int checkUsernameExists(const char *userName)
{
    MYSQL *conn = mysql_init(NULL);
    MYSQL_RES *res;
    MYSQL_ROW row;

    conn = mysql_init(NULL);

    if (mysql_real_connect(conn, "localhost", "atmuser", "Abdoo@2004", "atm", 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "Error connecting to database: %s\n", mysql_error(conn));
        return 3;
    }

    char query[1000];
    snprintf(query, sizeof(query), "SELECT * FROM Users WHERE UserName = '%s'", userName);

    if (mysql_query(conn, query) != 0)
    {
        mysql_close(conn);
        return 2;
    }

    res = mysql_use_result(conn);
    if ((row = mysql_fetch_row(res)) != NULL)
    {
        mysql_free_result(res);
        mysql_close(conn);
        return 1;
    }

    mysql_free_result(res);
    mysql_close(conn);
    return 0;
}

/// @brief Gets the current date from system
/// @return current date as a string
char *getTodaysDateAsString()
{
    time_t t = time(NULL);
    struct tm *now = localtime(&t);

    char *dateStr = (char *)malloc(sizeof(char) * 40); // "YYYY-MM-DD\0"
    snprintf(dateStr, 100, "%04d-%02d-%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);

    return dateStr;
}

/// @brief Trims string
/// @param str trimmed string
void trim(char *str)
{
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0) // All spaces?
        return;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    // Write new null terminator character
    end[1] = '\0';
}

/// @brief Prints a error message in ncurses
/// @param errmsg Error Message to print
void errprint(char *errmsg)
{
    clear();
    init_pair(9, COLOR_RED, COLOR_BLACK);
    attron(COLOR_PAIR(9) | A_BOLD);
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    char errmsgM[1000];
    snprintf(errmsgM, sizeof(errmsgM), "%s", errmsg);
    mvprintw(maxY / 2, (maxX / 2 - strlen(errmsgM) / 2), "%s", errmsgM);
    attroff(COLOR_PAIR(9) | A_BOLD);
    getch();
    endwin();
    return;
}

/// @brief Checks if string only has digits
/// @param str String to check
/// @return true if only digits, false otherwise
bool HasOnlyDigits(const char *str)
{
    bool dotFound = false;

    while (*str != '\0')
    {
        if (*str == '.')
        {
            if (dotFound)
            {
                return false; // More than one dot found
            }
            dotFound = true;
        }
        else if ((*str < '0' || *str > '9') && *str != '.')
        {
            return false; // Non-digit character found
        }
        str++;
    }

    return true;
}

/// @brief Func to check if Account Type is valid
/// @param str Account Type
/// @return true if valid, false otherwise
bool IsValidAccountType(const char *str)
{
    const char *validTypes[] = {"fixed01", "fixed02", "fixed03", "current", "savings", "saving"};
    const int numTypes = sizeof(validTypes) / sizeof(validTypes[0]);

    for (int i = 0; i < numTypes; i++)
    {
        if (strcasecmp(str, validTypes[i]) == 0)
        {
            return true;
        }
    }

    return false;
}

/// @brief Scans DB for userID from Username
/// @param username desired Uname
/// @return -1 if err or not found, userID otherwise
int GetUserIdFromUsername(const char *username)
{
    MYSQL *conn = mysql_init(NULL);

    if (conn == NULL)
    {
        return -1;
    }

    if (mysql_real_connect(conn, "localhost", "atmuser", "Abdoo@2004", "atm", 0, NULL, 0) == NULL)
    {
        mysql_close(conn);
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query), "SELECT UserID FROM Users WHERE UserName = '%s'", username);

    if (mysql_query(conn, query) != 0)
    {
        mysql_close(conn);
        return -1;
    }

    MYSQL_RES *result = mysql_store_result(conn);

    if (result == NULL)
    {
        mysql_close(conn);
        return -1;
    }

    int userId = -1;

    if (mysql_num_rows(result) > 0)
    {
        MYSQL_ROW row = mysql_fetch_row(result);
        userId = atoi(row[0]);
    }

    mysql_free_result(result);
    mysql_close(conn);

    return userId;
}

/// @brief Logs Errors To file
/// @param filename file to log to
/// @param text text to log
void LogToFile(const char *filename, const char *text)
{
    FILE *file = fopen(filename, "a"); // Open the file in "append" mode

    if (file == NULL)
    {
        file = fopen(filename, "w"); // Open the file in "write" mode to create it
        if (file == NULL)
        {
            printf("Failed to create file: %s\n", filename);
            return;
        }
    }

    fprintf(file, "%s\n", text); // Write the text to the file
    fclose(file);                // Close the file
}

/// @brief Checks if Account ID exists
/// @param id Desired ID
/// @return -1 if ERR or not found, 0 otherwise
int checkAccIDExist(char *id)
{
    MYSQL *conn = mysql_init(NULL);
    MYSQL_RES *res;
    MYSQL_ROW row;

    if (mysql_real_connect(conn, "localhost", "atmuser", "Abdoo@2004", "atm", 0, NULL, 0) == NULL)
    {
        mysql_close(conn);
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM Accounts WHERE AccountID = %d", atoi(id));

    if (mysql_query(conn, query) != 0)
    {
        errprint((char *)mysql_error(conn));
        mysql_close(conn);
        return -1;
    }

    res = mysql_store_result(conn);
    if (res == NULL)
    {
        fprintf(stderr, "mysql_store_result() failed\n");
        errprint((char *)mysql_error(conn));
        mysql_close(conn);
        return -1;
    }

    row = mysql_fetch_row(res);
    if (row == NULL)
    {
        fprintf(stderr, "mysql_fetch_row() failed\n");
        errprint((char *)mysql_error(conn));
        mysql_free_result(res);
        mysql_close(conn);
        return -1;
    }

    int count = atoi(row[0]);

    if (count >= 1)
    {
        return 289;
    }

    mysql_free_result(res);
    mysql_close(conn);

    return 0;
}

bool isValidDate(const char *dateString)
{
    // Check if the date string contains only digits and follows the format "YYYY-MM-DD"
    for (int i = 0; dateString[i] != '\0'; ++i)
    {
        if (!isdigit(dateString[i]) && (i != 4 && i != 7) && dateString[i] != '-')
        {
            return false;
        }
    }

    int year, month, day;
    if (sscanf(dateString, "%d-%d-%d", &year, &month, &day) == 3)
    {
        if (year >= 1000 && year <= 9999 && month >= 1 && month <= 12 && day >= 1)
        {
            switch (month)
            {
            case 1:
            case 3:
            case 5:
            case 7:
            case 8:
            case 10:
            case 12:
                return day <= 31;
            case 4:
            case 6:
            case 9:
            case 11:
                return day <= 30;
            case 2:
                if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
                {
                    return day <= 29;
                }
                else
                {
                    return day <= 28;
                }
            default:
                return false;
            }
        }
    }
    return false;
}

void mvprintenter(char *str)
{
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    mvprintw(maxY / 2, maxX / 2 - strlen(str) / 2, str);
}

// Function to calculate the interest gains
double calculateInterestGains(const char *inputDate, double initialBalance, const char *accType)
{
    // Parse the input date

    Date date;
    sscanf(inputDate, "%d-%d-%d", &date.year, &date.month, &date.day);

    // Get the current date (assuming today's date is 2024-01-09)
    Date currentDate = {2024, 1, 9};

    // Check if the input date is valid
    if (date.year < 0 || date.month < 1 || date.month > 12 || date.day < 1 || date.day > 31)
    {
        mvprintenter("Invalid date format\n");
        return 0.0;
    }

    // Calculate the number of days since the input date
    int daysSinceStart = (currentDate.year - date.year) * 365 +
                         (currentDate.month - date.month) * 30 +
                         (currentDate.day - date.day);

    // Assume a fixed annual interest rate (for simplicity)
    double annualInterestRate = 0.05; // 5%

    // Calculate the interest gains using simple interest formula
    double interestGains = initialBalance * annualInterestRate * daysSinceStart / 365.0;

    return interestGains;
}

/// @brief Method for main interest gainz (Thanks sameer)
/// @param inputDate date of account opening
/// @param initialBalance balance of account
/// @param accountType Account type
/// @return formatted string of account info
char *MainAccountInterestInfo(const char *inputDate, double initialBalance, const char *accountType)
{
    int depositMonth, depositDay, depositYear;
    sscanf(inputDate, "%d-%d-%d", &depositYear, &depositMonth, &depositDay);
    float interest_rate = 0.0;

    if (strcmp(accountType, "savings") == 0 || strcmp(accountType, "saving") == 0)
    {
        interest_rate = 0.07;
    }
    else if (strcmp(accountType, "fixed01") == 0)
    {
        interest_rate = 0.04;
    }
    else if (strcmp(accountType, "fixed02") == 0)
    {
        interest_rate = 0.05;
    }
    else if (strcmp(accountType, "fixed03") == 0)
    {
        interest_rate = 0.08;
    }
    else if (strcmp(accountType, "current") == 0)
    {
        return strdup("You will not get interests because the account is of type current.\n");
    }
    else
    {
        return strdup("Invalid account type.\n");
    }

    time_t currentTime = time(NULL);
    struct tm *localTime = localtime(&currentTime);
    int currentYear = localTime->tm_year + 1900;
    int currentMonth = localTime->tm_mon + 1;

    float totalInterest = 0.0;
    float interestForMonth = 0.0;
    while (depositYear < currentYear ||
           (depositYear == currentYear && depositMonth <= currentMonth))
    {
        interestForMonth = (initialBalance * interest_rate) / 12;
        totalInterest += interestForMonth;

        depositMonth++;
        if (depositMonth > 12)
        {
            depositMonth = 1;
            depositYear++;
        }
    }

    if (strcmp(accountType, "fixed01") == 0)
    {
        char *result;
        asprintf(&result, "You will gain $%.2f interest on %d/%d/%d\n", interestForMonth * 12, depositDay, depositMonth, depositYear + 1);
        return result;
    }
    else if (strcmp(accountType, "fixed02") == 0)
    {
        char *result;
        asprintf(&result, "You will gain $%.2f interest on %d/%d/%d\n", interestForMonth * 24, depositDay, depositMonth, depositYear + 2);
        return result;
    }
    else if (strcmp(accountType, "fixed03") == 0)
    {
        char *result;
        asprintf(&result, "You will gain $%.2f interest on %d/%d/%d\n", interestForMonth * 36, depositDay, depositMonth, depositYear + 3);
        return result;
    }
    else
    {
        char *result;
        asprintf(&result, "You will get $%.2f as interest on day %d of every month. Total interest acquired: $%.2f\n", interestForMonth, depositDay, totalInterest);
        return result;
    }
}

/// @brief Checks if user is authorized to perform an account Action
/// @param u 
/// @return 
bool checkAuth(struct User u, char *dID)
{
    MYSQL *conn = mysql_init(NULL);
    MYSQL_RES *res;
    MYSQL_ROW row;

    if (mysql_real_connect(conn, "localhost", "atmuser", "Abdoo@2004", "atm", 0, NULL, 0) == NULL)
    {
        mysql_close(conn);
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query), "SELECT UserName FROM Accounts WHERE AccountID = %d", atoi(dID));

    if (mysql_query(conn, query) != 0)
    {
        errprint((char *)mysql_error(conn));
        mysql_close(conn);
        return -1;
    }

    res = mysql_store_result(conn);
    if (res == NULL)
    {
        fprintf(stderr, "mysql_store_result() failed\n");
        errprint((char *)mysql_error(conn));
        mysql_close(conn);
        return -1;
    }

    row = mysql_fetch_row(res);
    if (row == NULL)
    {
        fprintf(stderr, "mysql_fetch_row() failed\n");
        errprint((char *)mysql_error(conn));
        mysql_free_result(res);
        mysql_close(conn);
        return -1;
    }

    char *uname = row[0];

    return strcmp(uname, u.name) == 0;  
}