#include "header.h"
#include <mysql/mysql.h>
#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <ctype.h>

/// @brief MAIN TUI FOR USER REGISTERATION
void CreateNewUser()
{
    MYSQL *conn;
    // MYSQL_RES res;
    // MYSQL_ROW row;
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
    do
    {
        mvprintenter("Write a Username: ");
        refresh();
        scanw("%s", userName);
        trim(userName);
        werase(stdscr);
    } while (strcmp(userName, "") == 0);

    do
    {
        werase(stdscr);
        refresh();
        noecho();
        mvprintenter("Enter a secure password: ");
        refresh();
        scanw("%s", userPwd);
        trim(userPwd);
        werase(stdscr);
        refresh();
        echo();
    } while (strcmp(userPwd, "") == 0);

    attroff(COLOR_PAIR(4) | A_BOLD);

    //* ================================================================================
    //* check if username exists
    int ucheck = checkUsernameExists(userName);
    if (ucheck != 0)
    {
        switch (ucheck)
        {
        case 2:
            errprint("Error executing username check query");
            // mysql_close(conn);
            return;
        case 3:
            errprint("error connecting to user database");
            // mysql_close(conn);
            endwin();
            return;
        case 1:
            errprint("User Already Exists");
            // mysql_close(conn);
            endwin();
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

    mysql_close(conn);
    endwin();

    //* ==================================================
    //* if all is done,
    struct User nu;
    strcpy(nu.name, userName);
    strcpy(nu.password, userPwd);
    mainMenu(nu);
}
