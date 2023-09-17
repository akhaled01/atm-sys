#include "header.h"
#include <mysql/mysql.h>
#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <ctype.h>

void CreateNewUser()
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char userName[255];
    char userPwd[255];

    // get user input
    initscr();
    clear();
    printw("\n\tEnter UserName: ");
    refresh();
    scanw("%s", userName);

    noecho();
    refresh();
    printw("\n\tEnter Password: ");
    refresh();
    scanw("%s", userPwd);
    echo();

    // check if username exists
    int ucheck = checkUsernameExists(userName);
    if (ucheck != 0)
    {
        switch (ucheck)
        {
        case 2:
            fprintf(stderr, "Error executing username check query\n");
            return;
        case 3:
            fprintf(stderr, "Error connecting to user database\n");
            return;
        case 1:
            fprintf(stderr, "Username Already Exists\n");
            return;
        }
    }

    // execute insertion query
    conn = mysql_init(NULL);

    if (mysql_real_connect(conn, "localhost", "atmuser", "Abdoo@2004", "atm", 0, NULL, 0) == NULL)
    {
        printw("Error connecting to database: %s\n", mysql_error(conn));
        refresh();
        endwin();
        return;
    }

    char query[1000];
    snprintf(query, sizeof(query), "INSERT INTO Users (UserName, UserPwd) VALUES ('%s', '%s')", userName, userPwd);

    if (mysql_query(conn, query) != 0)
    {
        fprintf(stderr, "Error executing query: %s\n", mysql_error(conn));
        mysql_close(conn);
        return;
    }

    printw("New user record created successfully.\n");
    refresh();
    endwin();

    mysql_close(conn);
    //* if all is done, log the new user in for transactions
    struct User nu;
    strcpy(nu.name, userName);
    strcpy(nu.password, userPwd);
    mainMenu(nu);
}

void CreateNewAcc(struct User u)
{
    char *username = u.name;
    char *pass = u.password;

    char *date = getTodaysDateAsString();
    char country[100];

    initscr();
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
        mvprintw(maxY / 6, maxX / 4, "What country are you in right now: ");
        refresh();
        attroff(COLOR_PAIR(2) | A_BOLD);
        scanw("%s", country);
        trim(country);
    } while (strlen(country) == 0);

    getch();
    endwin();
}

void checkAllAccounts(struct User u)
{
}
