#include "header.h"
#include <mysql/mysql.h>
#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <ctype.h>

// this file includes all the account Operations

// FIXME: FIND AND ELIMINATE ANY MEM LEAKS

// TODO: MAKE DATE CUSTOMIZABLE

/// @brief Main TUI and DB funcs to create a new account
/// @param u Current User
void CreateNewAcc(struct User u)
{
    char AccID[16];
    char *username = u.name;
    char *pass = u.password;
    char *date = getTodaysDateAsString();
    char country[100];
    char PhoneNo[100];
    char Balance[100];
    char AccType[100];

    initscr();
    clear();
    curs_set(2);
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    // title display
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(maxY / 8, (maxX / 2 - strlen("R01 BANK NEW ACCOUNT FORM") / 2), "R01 BANK NEW ACCOUNT FORM");
    refresh();
    attroff(COLOR_PAIR(1) | A_BOLD);
    // AccountID
    do
    {
        mvprintw(6, maxX / 4, "Write A AccountID: ");
        refresh();
        attroff(COLOR_PAIR(2) | A_BOLD);
        scanw("%s", AccID);
        trim(AccID);
        if (checkAccIDExist(AccID) == -1)
        {
            mvprintw(6, maxX / 4, "ERROR ACCESSING ACC DATABASE FOR ID CHECK");
            mainMenu(u);
        }

    } while (strlen(AccID) >= 16 || checkAccIDExist(AccID) == 289);

    // Country
    do
    {
        mvprintw(6, maxX / 4, "What country are you in right now: ");
        refresh();
        attroff(COLOR_PAIR(2) | A_BOLD);
        scanw("%s", country);
        trim(country);
    } while (strlen(country) == 0);
    // PhoneNo
    do
    {
        mvprintw(8, maxX / 4, "Enter Your Phone Number: ");
        refresh();
        attroff(COLOR_PAIR(2) | A_BOLD);
        scanw("%s", PhoneNo);
        trim(PhoneNo);
    } while (strlen(PhoneNo) != 9);
    // Balance
    do
    {
        mvprintw(10, maxX / 4, "Enter The Balance: ");
        refresh();
        attroff(COLOR_PAIR(2) | A_BOLD);
        scanw("%s", Balance);
        trim(Balance);
    } while (strlen(Balance) == 0 || !HasOnlyDigits(Balance) || strlen(Balance) > 8);
    // Account type
    do
    {
        mvprintw(12, maxX / 4, "Enter The Account type (please read TOS): ");
        refresh();
        attroff(COLOR_PAIR(2) | A_BOLD);
        scanw("%s", AccType);
        trim(AccType);
    } while (strlen(AccType) == 0 || !IsValidAccountType(AccType));

    // if all is good, execute a query to insert a new account table
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW *row;

    //* ================================================================================
    //* execute insertion query

    conn = mysql_init(NULL);

    if (mysql_real_connect(conn, "localhost", "atmuser", "Abdoo@2004", "atm", 0, NULL, 0) == NULL)
    {
        mysql_close(conn);
        errprint("Error connecting to database");
    }

    int userID = GetUserIdFromUsername(u.name);
    if (userID == -1)
    {
        mysql_close(conn);
        errprint("ERROR: UNABLE TO GET USERID FROM USER DATABASE");
    }

    char query[1000];
    snprintf(query,
             sizeof(query),
             "INSERT INTO Accounts (AccountID, UserName, UserID, CreationDate, Country, PhoneNo, Balance, AccountType) VALUES (%d, '%s', %d, '%s', '%s', '%s', %d, '%s')",
             atoi(AccID), u.name, userID, date, country, PhoneNo, atoi(Balance), AccType);

    if (mysql_query(conn, query) != 0)
    {
        char emsg[1000];
        sprintf(emsg, "Error inserting Account: %s\n", mysql_error(conn));
        errprint(emsg);
        mysql_close(conn);
        endwin();
    }
    else
    {
        attron(COLOR_PAIR(2) | A_BOLD);
        curs_set(0);
        clear();
        mvprintw(maxY / 2,
                 (maxX / 2 - strlen("NEW ACCOUNT SUCCESSFULLY CREATED") / 2),
                 "NEW ACCOUNT SUCCESSFULLY CREATED");
        attroff(COLOR_PAIR(2) | A_BOLD);
        int c = getch();
        mysql_close(conn);
        curs_set(1);
        endwin();
        mainMenu(u);
    }
}

/// @brief Outputs list of all accounts
/// @param u current User
void checkAllAccounts(struct User u)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    //* ================================================================================
    //* execute Selection query

    conn = mysql_init(NULL);

    if (!mysql_real_connect(conn, "localhost", "atmuser", "Abdoo@2004", "atm", 0, NULL, 0))
    {
        errprint("Error connecting to database");
        mysql_close(conn);
    }

    char getAccountsQuery[1000];
    snprintf(getAccountsQuery, sizeof(getAccountsQuery),
             "SELECT * FROM Accounts WHERE UserName = '%s'",
             u.name);

    if (mysql_query(conn, getAccountsQuery))
    {
        char emsg[1000];
        snprintf(emsg, sizeof(emsg), "Error fetching Users '%s'", mysql_error(conn));
        errprint(emsg);
        mysql_close(conn);
    }

    res = mysql_use_result(conn);

    //* ================================================================================
    //* Display results

    AccountRecord Accounts[1000];
    int count = 0;

    initscr();
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    clear();

    while ((row = mysql_fetch_row(res)) != NULL)
    {
        AccountRecord curAcc = Accounts[count];
        curAcc.Accid = atoi(row[0]);
        strcpy(curAcc.username, row[1]);
        curAcc.userId = atoi(row[2]);
        strcpy(curAcc.creationdate, row[3]);
        strcpy(curAcc.country, row[4]);
        strcpy(curAcc.phoneno, row[5]);
        curAcc.balance = atoi(row[6]);
        strcpy(curAcc.accountType, row[7]);
        Accounts[count] = curAcc;
        count++;
    }

    attron(COLOR_PAIR(4) | A_BOLD);
    printw("\n\tCreation Date | Country of Creation | Associated Phone Number | Account Balance | Account type\n\n\n");
    attroff(COLOR_PAIR(4) | A_BOLD);
    refresh();

    attron(COLOR_PAIR(1) | A_BOLD);
    for (int i = 0; i < count; i++)
    {
        printw("\t%s | %s | %s | %.2f | %s\n\n",
               Accounts[i].creationdate,
               Accounts[i].country,
               Accounts[i].phoneno,
               Accounts[i].balance,
               Accounts[i].accountType);
        refresh();
        int a = getch();
    }
    attroff(COLOR_PAIR(1) | A_BOLD);

    mysql_free_result(res);
    mysql_close(conn);
    int c = getch();
    endwin();
    mainMenu(u);
}

/// @brief Updates specified Account
/// @param u Current User
void UpdateAccInfo(struct User u)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    //* ========================================
    //* Taking User Input

    char dID[1000], choice[10];
    int ch = 0;
    char Ndata[1000];

    initscr();
    curs_set(1);
    clear();
    printw("\n\t Enter the Account ID to the desired Account: ");
    refresh();
    scanw("%s", dID);
    do
    {
        printw("\n\t What field do you desire to change? ([1] Phone Number, [2] Country): ");
        scanw("%s", choice);

        if (strcmp(choice, "1") == 0)
        {
            ch = 1;
            do
            {
                printw("\n\t\tEnter Your New Phone Number: ");
                refresh();
                scanw("%s", Ndata);
                trim(Ndata);
            } while (strlen(Ndata) != 9);
        }
        else if (strcmp(choice, "2") == 0)
        {
            ch = 2;
            do
            {
                printw("Enter a New country: ");
                refresh();
                scanw("%s", Ndata);
                trim(Ndata);
            } while (strlen(Ndata) == 0);
        }
    } while (strcmp(choice, "1") != 0 && strcmp(choice, "2") != 0);

    //*==================================
    //* execute modification

    printw("\n\n\tEXECUTING QUERY...");
    refresh();

    conn = mysql_init(NULL);
    if (mysql_real_connect(conn, "localhost", "atmuser", "Abdoo@2004", "atm", 0, NULL, 0) == NULL)
    {
        errprint("Cannot connect to database");
        mysql_close(conn);
    }

    char query[1500];
    snprintf(query,
             sizeof(query),
             "SELECT * FROM Accounts WHERE UserName = '%s'",
             u.name);

    if (mysql_query(conn, query) != 0)
    {
        char emsg[1000];
        sprintf(emsg, "Error Getting Accounts: %s\n", mysql_error(conn));
        errprint(emsg);
        mysql_close(conn);
        endwin();
    }

    res = mysql_store_result(conn);
    if (!res)
    {
        errprint("Failed to get query result");
        mysql_close(conn);
    }

    int numRows = mysql_num_rows(res);
    if (atoi(dID) > numRows)
    {
        errprint("ID number out of range of available Account records for your user");
        mysql_free_result(res);
        mysql_close(conn);
    }

    AccountRecord *accountArray = (AccountRecord *)malloc(numRows * sizeof(AccountRecord));
    int rownum = 0;
    while ((row = mysql_fetch_row(res)) != NULL && rownum <= numRows)
    {
        accountArray[rownum].Accid = atoi(row[0]);
        strncpy(accountArray[rownum].username, row[1], sizeof(accountArray[rownum].username));
        accountArray[rownum].userId = atoi(row[2]);
        strncpy(accountArray[rownum].creationdate, row[3], sizeof(accountArray[rownum].creationdate));
        strncpy(accountArray[rownum].country, row[4], sizeof(accountArray[rownum].country));
        strncpy(accountArray[rownum].phoneno, row[5], sizeof(accountArray[rownum].phoneno));
        accountArray[rownum].balance = atof(row[6]);
        strncpy(accountArray[rownum].accountType, row[7], sizeof(accountArray[rownum].accountType));
        rownum++;
    }

    mysql_free_result(res);

    char updateQuery[1500];
    if (ch == 1)
    {

        snprintf(updateQuery, sizeof(updateQuery), "UPDATE Accounts SET PhoneNo = '%s' WHERE AccountID = %d",
                 Ndata, accountArray[atoi(dID)].Accid);
    }
    else if (ch == 2)
    {
        snprintf(updateQuery, sizeof(updateQuery), "UPDATE Accounts SET Country = '%s' WHERE AccountID = %d",
                 Ndata, accountArray[atoi(dID)].Accid);
    }

    if (mysql_query(conn, updateQuery))
    {
        errprint("Error updating the row\n");
    }
    else
    {
        clear();
        printw("Row updated successfully.\n");
        refresh();
    }

    mysql_close(conn);
    free(accountArray);
    int c = getch();
    endwin();
    mainMenu(u);
}

/// @brief Function to check details of a Specific Account
/// @brief Implements DB
/// @param u Current User
void checkSpecificAcc(struct User u)
{
    MYSQL *conn = mysql_init(NULL);
    MYSQL_RES *res;
    MYSQL_ROW row;
    char dID[10000];

    initscr();
    curs_set(1);
    clear();
    printw("\n\t Enter the Account ID to the desired Account: ");
    refresh();
    scanw("%s", dID);

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

    // map to struct
    dAccount->Accid = atoi(row[0]);
    strncpy(dAccount->creationdate, row[3], sizeof(dAccount->creationdate));
    strncpy(dAccount->country, row[4], sizeof(dAccount->country));
    strncpy(dAccount->phoneno, row[5], sizeof(dAccount->phoneno));
    dAccount->balance = atoi(row[6]);
    strncpy(dAccount->accountType, row[7], sizeof(dAccount->accountType));

    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    clear();

    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(2, 5, "Account Number: %d", dAccount->Accid);
    mvprintw(3, 5, "Country Of creation: %s", dAccount->country);
    mvprintw(4, 5, "Creation Date: %s", dAccount->creationdate);
    mvprintw(5, 5, "Associated Phone No: %s", dAccount->phoneno);
    char strBal[100];
    snprintf(strBal, sizeof(strBal), "%f", dAccount->balance);
    mvprintw(6, 5, "Current Account Balance: %s", strBal);
    attroff(COLOR_PAIR(1) | A_BOLD);
    attron(COLOR_PAIR(4) | A_BOLD);
    mvprintw(8, 5, "%s", displayAccountInformation(dAccount->creationdate, dAccount->accountType, dAccount->balance));
    attroff(COLOR_PAIR(4) | A_BOLD);
    getch();
    mysql_close(conn);
    endwin();
    free(dAccount);
    mainMenu(u);
}

/// @brief Func to delete Account. All Associated Transactional records are deleted with it
/// @param u Current User
void DelAcc(struct User u)
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

    int isAccHere = checkAccIDExist(dID);

    if (isAccHere == 0)
    {
        errprint("Account Doesn't Exist");
        mainMenu(u);
    }

    if (mysql_real_connect(conn, "localhost", "atmuser", "Abdoo@2004", "atm", 0, NULL, 0) == NULL)
    {
        errprint("Cannot connect to database");
        mysql_close(conn);
    }

    // del all transctions related to account
    char query[1500];
    snprintf(query,
             sizeof(query),
             "DELETE FROM Transactions WHERE AccountID = %d",
             atoi(dID));

    if (mysql_query(conn, query))
    {
        errprint("error executing Transaction Deletion query");
        errprint((char *)mysql_error(conn));
        mysql_close(conn);
        endwin();
    }

    // del Account from DB

    char query2[1500];
    snprintf(query2,
             sizeof(query2),
             "DELETE FROM Accounts WHERE AccountID = %d",
             atoi(dID));

    if (mysql_query(conn, query2))
    {
        errprint("error executing main Account Deletion query");
        errprint((char *)mysql_error(conn));
        mysql_close(conn);
        endwin();
    }

    clear();
    attroff(COLOR_PAIR(2) | A_BOLD);
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(maxY / 2, (maxX / 2 - strlen("Deletion Operation Successful!") / 2), "Deletion Operation Successful!");
    attroff(COLOR_PAIR(1) | A_BOLD);

    // success, free all mem Allocs and go back to main menu
    mysql_free_result(res);
    mysql_close(conn);
    getch();
    endwin();
    mainMenu(u);
}

// TODO: Finish Transfer Acc

/// @brief Func to transfer Account Ownership. All Associated Transactional records are Updated with it.
/// @brief Also uses child processes to notify other running sessions of the event
/// @param u Current User
void TransferAcc(struct User u) {}