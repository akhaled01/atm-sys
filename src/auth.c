#include <termios.h>
#include "header.h"
#include <mysql/mysql.h>

void loginMenu()
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char userName[255];
    char userPwd[255];

    // get user input
    system("clear");
    printf("\n\t\t\t\t\t\tWelcome to R01 Bank");
    printf("\n\tEnter UserName: ");

    scanf("%s", userName);

    printf("\n\tEnter Password: ");

    scanf("%s", userPwd);

    conn = mysql_init(NULL);

    if (conn == NULL)
    {
        fprintf(stderr, "Failed to initialize MySQL connection.\n");
        return;
    }

    if (mysql_real_connect(conn, "localhost", "atmuser", "Abdoo@2004", "atm", 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "Failed to connect to the database: Error: %s\n", mysql_error(conn));
        mysql_close(conn);
        return;
    }

    char query[1000];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM Users WHERE UserName='%s' AND UserPwd='%s'", userName, userPwd);

    if (mysql_query(conn, query) != 0)
    {
        fprintf(stderr, "Failed to execute query: Error: %s\n", mysql_error(conn));
        mysql_close(conn);
        return;
    }

    res = mysql_store_result(conn);
    if (res == NULL)
    {
        fprintf(stderr, "Failed to retrieve result set: Error: %s\n", mysql_error(conn));
        mysql_close(conn);
        return;
    }

    row = mysql_fetch_row(res);
    if (row == NULL)
    {
        fprintf(stderr, "Failed to fetch row: Error: %s\n", mysql_error(conn));
        mysql_free_result(res);
        mysql_close(conn);
        return;
    }

    unsigned long *lengths = mysql_fetch_lengths(res);
    unsigned long num_records = atoi(row[0]); // Assuming the count is returned as the first column

    if (num_records > 0)
    {
        printf("\n\t\t\t\tlogin success\n");
        mysql_free_result(res);
        mysql_close(conn);
        struct User lu;
        strcpy(lu.name, userName);
        strcpy(lu.password, userPwd);
        mainMenu(lu);
    }
    else
    {
        printf("login failed\n");
    }

    mysql_free_result(res);
    mysql_close(conn);
    return;
}
