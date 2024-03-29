#include "header.h"
#include <mysql/mysql.h>
#include <ncurses.h>

/// @brief MAIN LOGIN TUI
void loginMenu()
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char userName[255];
    char userPwd[255];

    // get user input
    initscr();
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    // title display
    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintenter("Enter Username: ");
    scanw("%s", userName);
    werase(stdscr);
    refresh();
    noecho();
    mvprintenter("Enter Password: ");
    refresh();
    scanw("%s", userPwd);
    echo();

    attroff(COLOR_PAIR(2) | A_BOLD);

    conn = mysql_init(NULL);

    if (conn == NULL)
    {
        clear();
        attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(0, 0, "Error connecting to database, press any key to exit");
        attroff(COLOR_PAIR(3) | A_BOLD);
        getch();
        endwin();
        return;
    }

    if (mysql_real_connect(conn, "localhost", "atmuser", "Abdoo@2004", "atm", 0, NULL, 0) == NULL)
    {
        clear();
        attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(0, 0, "Error connecting to database, press any key to exit");
        refresh();
        attroff(COLOR_PAIR(3) | A_BOLD);
        getch();
        endwin();
        return;
    }

    char query[1000];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM Users WHERE UserName='%s' AND UserPwd='%s'", userName, userPwd);

    if (mysql_query(conn, query) != 0)
    {
        clear();
        attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(0, 0, "Error executing query, press any key to exit");
        refresh();
        attroff(COLOR_PAIR(3) | A_BOLD);
        getch();
        endwin();
        return;
    }

    res = mysql_store_result(conn);
    if (res == NULL)
    {
        clear();
        attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(0, 0, "Error getting result from database, press any key to exit");
        refresh();
        attroff(COLOR_PAIR(3) | A_BOLD);
        int c = getch();
        endwin();
        return;
    }

    row = mysql_fetch_row(res);
    if (row == NULL)
    {
        clear();
        attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(0, 0, "Error fetching row, press any key to exit");
        refresh();
        attroff(COLOR_PAIR(3) | A_BOLD);
        int c = getch();
        endwin();
        return;
    }

    unsigned long *lengths = mysql_fetch_lengths(res);
    unsigned long num_records = atoi(row[0]); // Assuming the count is returned as the first column

    if (num_records > 0)
    {
        mysql_free_result(res);
        mysql_close(conn);
        endwin();
        struct User lu;
        strcpy(lu.name, userName);
        strcpy(lu.password, userPwd);
        mainMenu(lu);
    }
    else
    {
        clear();
        attron(COLOR_PAIR(3) | A_BOLD);
        errprint("Login Failed");
        refresh();
        attroff(COLOR_PAIR(3) | A_BOLD);
        endwin();
        return;
    }
}
