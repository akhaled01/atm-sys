#include "header.h"
#include <mysql/mysql.h>
#include <ncurses.h>
#include <string.h>
#include <ctype.h>

// this file deals with transactions in the application
// DONE BY AKHALED

/// @brief Function to change DB to deposit Money
/// @param u current User
/// @param a AccountRecord struct
/// @param Amt amount of transaction
/// @return 0 if successful, 1 otherwise
int Deposit(struct User u, AccountRecord *a, int Amt)
{
    MYSQL *conn = mysql_init(NULL);
    MYSQL_RES *res;
    MYSQL_ROW row;

    if (mysql_real_connect(conn, "localhost", "atmuser", "Abdoo@2004", "atm", 0, NULL, 0) == NULL)
    {
        errprint("Cannot connect to database");
        mysql_close(conn);
        return 1;
    }

    char query[1500];

    int mAmt = a->balance + Amt;

    snprintf(query, sizeof(query), "UPDATE Accounts SET Balance = %d WHERE AccountID = %d", mAmt, a->Accid);

    if (mysql_query(conn, query))
    {
        errprint("error executing deposit query");
        errprint((char *)mysql_error(conn));
        mysql_close(conn);
        endwin();
        return 1;
    }

    return 0;
}

/// @brief Function to make withdrawal. If amt larger than Balance, balance is set to 0
/// @param u current User
/// @param a AccountRecord struct
/// @param Amt amount of transaction
/// @return 0 if successful, 1 otherwise
int Withdraw(struct User u, AccountRecord *a, int Amt)
{
    MYSQL *conn = mysql_init(NULL);
    MYSQL_RES *res;
    MYSQL_ROW row;

    if (mysql_real_connect(conn, "localhost", "atmuser", "Abdoo@2004", "atm", 0, NULL, 0) == NULL)
    {
        errprint("Cannot connect to database");
        mysql_close(conn);
        return 1;
    }

    int mAmt = a->balance - Amt;

    if (mAmt < 0)
    {
        return 1;
    }

    char query[1500];

    snprintf(query, sizeof(query), "UPDATE Accounts SET Balance = %d WHERE AccountID = %d", mAmt, a->Accid);

    if (mysql_query(conn, query))
    {
        errprint("error executing withdraw query");
        errprint((char *)mysql_error(conn));
        mysql_close(conn);
        endwin();
        return 1;
    }

    return 0;
}

/// @brief Function to make Transaction. calls the TUI and invokes Withdrawal / deposit function
/// @brief depending on the user's choice. It also saves everything to Transaction Table in DB
/// @param u current User
void MakeTransaction(struct User u)
{
    MYSQL *conn = mysql_init(NULL);
    MYSQL_RES *res;
    MYSQL_ROW row;
    char dID[10000];

    initscr();
    curs_set(1);
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    clear();
    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(maxY / 2, (maxX / 2 - strlen(" Enter the Account ID to the desired Account: ") / 2), " Enter the Account ID to the desired Account: ");
    refresh();
    scanw("%s", dID);

    if (checkAuth(u, dID) == false)
    {
        errprint("You are not authorized to perform this action on this account");
        mainMenu(u);
    }

    if (mysql_real_connect(conn, "localhost", "atmuser", "Abdoo@2004", "atm", 0, NULL, 0) == NULL)
    {
        errprint("Cannot connect to database");
        mysql_close(conn);
    }

    char query[1500];
    snprintf(query,
             sizeof(query),
             "SELECT * FROM Accounts WHERE AccountID = %d",
             atoi(dID));

    if (mysql_query(conn, query))
    {
        errprint("error executing query");
        errprint((char *)mysql_error(conn));
        mysql_close(conn);
        endwin();
    }

    res = mysql_store_result(conn);
    if (!res)
    {
        errprint("Failed to get query result");
        errprint((char *)mysql_error(conn));
        mysql_close(conn);
        endwin();
    }

    row = mysql_fetch_row(res);
    AccountRecord *dAccount;
    dAccount = (AccountRecord *)malloc(sizeof(AccountRecord));

    dAccount->Accid = atoi(row[0]);
    strncpy(dAccount->creationdate, row[3], sizeof(dAccount->creationdate));
    strncpy(dAccount->country, row[4], sizeof(dAccount->country));
    strncpy(dAccount->phoneno, row[5], sizeof(dAccount->phoneno));
    dAccount->balance = atoi(row[6]);
    strncpy(dAccount->accountType, row[7], sizeof(dAccount->accountType));
    // ensure non-fixed account type before proceeding
    if (strcasecmp(dAccount->accountType, "fixed01") == 0 || strcasecmp(dAccount->accountType, "fixed02") == 0 || strcasecmp(dAccount->accountType, "fixed03") == 0)
    {
        errprint("ACCOUNTS OF FIXED TYPE AREN'T ALLOWED TO MAKE TRANSACTIONS");
        mysql_free_result(res);
        free(dAccount);
        mysql_close(conn);
        endwin();
        mainMenu(u);
    }

    clear();
    mvprintw(maxY / 2, (maxX / 2 - strlen("Enter Amount of transaction: ") / 2), "Enter Amount of transaction: ");
    refresh();
    char dAmt[1000];
    scanw("%s", dAmt);
    clear();
    curs_set(0);
    mvprintw(maxY / 2, (maxX / 2 - strlen("[1] Withdraw   [2] Deposit") / 2), "[1] Withdraw   [2] Deposit");
    refresh();

    char opt[1000];
    scanw("%s", opt);

    int mainAmt = atoi(dAmt);

    switch (atoi(opt))
    {
    case 1:
        if (Withdraw(u, dAccount, mainAmt))
        {
            errprint("WITHDRAWAL FAILED, check your Balance");
            free(dAccount);
            mysql_free_result(res);
            mysql_close(conn);
            endwin();
            mainMenu(u);
        }
        break;
    case 2:
        if (Deposit(u, dAccount, mainAmt))
        {
            errprint("DEPOSIT FAILED");
            mysql_free_result(res);
            free(dAccount);
            mysql_close(conn);
            endwin();
            mainMenu(u);
        }
        break;
    default:
        errprint("invalid option");
        free(dAccount);
        mysql_free_result(res);
        mysql_close(conn);
        endwin();
        mainMenu(u);
    }

    char query2[1500];

    snprintf(query2, sizeof(query2), "INSERT INTO Transactions (AccountID, TransactionAmt) VALUES (%d, %d)", dAccount->Accid, mainAmt);

    if (mysql_query(conn, query2))
    {
        errprint("ERROR SAVING TRANSACTION TO DB");
        free(dAccount);
        mysql_free_result(res);
        mysql_close(conn);
        mainMenu(u);
    }

    clear();
    attroff(COLOR_PAIR(2) | A_BOLD);
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(maxY / 2, (maxX / 2 - strlen("Transaction Successful!") / 2), "Transaction Successful!");
    attroff(COLOR_PAIR(1) | A_BOLD);

    // success, free all mem Allocs and go back to main menu
    free(dAccount);
    mysql_free_result(res);
    mysql_close(conn);
    getch();
    endwin();
    mainMenu(u);
}