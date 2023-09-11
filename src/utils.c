#include <string.h>
#include <mysql/mysql.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include "header.h"

int checkUsernameExists(const char *userName)
{
    MYSQL *conn;
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

struct termios originalTerm;

void disableEcho()
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void enableEcho() {
    // Restore the original terminal attributes
    tcsetattr(STDIN_FILENO, TCSANOW, &originalTerm);
    system("clear");
}
