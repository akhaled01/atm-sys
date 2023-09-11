#include "header.h"
#include <mysql/mysql.h>
#include <stdio.h>

void CreateNewUser()
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char userName[255];
    char userPwd[255];

    // get user input
    system("clear");
    printf("\n\tEnter UserName: ");

    scanf("%s", userName);

    disableEcho();
    printf("\n\tEnter Password: ");

    scanf("%s", userPwd);
    enableEcho();

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
        fprintf(stderr, "Error connecting to database: %s\n", mysql_error(conn));
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

    printf("New user record created successfully.\n");

    mysql_close(conn);
}

void createNewAcc(struct User u)
{
}

void checkAllAccounts(struct User u)
{
}
