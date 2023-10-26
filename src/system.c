#include "header.h"
#include <mysql/mysql.h>
#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <ctype.h>

//* FEATURE 1: REGISTER NEW USER
void CreateNewUser()
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char userName[255];
    char userPwd[255];

    // *get user input
    initscr();
    clear();
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    int maxX, maxY;
    getmaxyx(stdscr, maxY, maxX);
    attron(COLOR_PAIR(4) | A_BOLD);
    mvprintw(3, (maxX / 2 - strlen("THANKS FOR CHOOSING OUR BANK") / 2), "THANKS FOR CHOOSING OUR BANK");
    refresh();
    do
    {
        mvprintw(6, (maxX / 2 - strlen("THANKS FOR CHOOSING OUR BANK") / 2), "Write a Username: ");
        refresh();
        scanw("%s", userName);
        trim(userName);
    } while (strcmp(userName, "") == 0);

    do
    {
        refresh();
        printw("\n\tEnter a secure password: ");
        refresh();
        noecho();
        scanw("%s", userPwd);
        echo();
        trim(userPwd);
    } while (strcmp(userPwd, "") == 0);

    //* ================================================================================
    //* check if username exists
    int ucheck = checkUsernameExists(userName);
    if (ucheck != 0)
    {
        switch (ucheck)
        {
        case 2:
            errprint("Error executing username check query");
            mysql_close(conn);
            return;
        case 3:
            errprint("connecting to user database");
            mysql_close(conn);
            return;
        case 1:
            errprint("User Already Exists");
            mysql_close(conn);
            return;
        }
    }

    //* ================================================================================
    //* execute insertion query

    conn = mysql_init(NULL);

    if (mysql_real_connect(conn, "localhost", "atmuser", "Abdoo@2004", "atm", 0, NULL, 0) == NULL)
    {
        // printw("Error connecting to database: %s\n", mysql_error(conn));
        mysql_close(conn);
        errprint("Error connecting to database");
    }

    char query[1000];
    snprintf(query, sizeof(query), "INSERT INTO Users (UserName, UserPwd) VALUES ('%s', '%s')", userName, userPwd);

    if (mysql_query(conn, query) != 0)
    {
        mysql_close(conn);
        errprint("Error inserting user");
    }

    endwin();

    mysql_close(conn);
    //* ==================================================
    //* if all is done,
    struct User nu;
    strcpy(nu.name, userName);
    strcpy(nu.password, userPwd);
    mainMenu(nu);
}

//* FEATURE 2: REGISTER NEW ACCOUNT

void CreateNewAcc(struct User u)
{
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
             "INSERT INTO Accounts (UserName, UserID, CreationDate, Country, PhoneNo, Balance, AccountType) VALUES ('%s', %d, '%s', '%s', '%s', %d, '%s')",
             u.name, userID, date, country, PhoneNo, atoi(Balance), AccType);

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
