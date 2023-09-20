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

char *getTodaysDateAsString()
{
    time_t t = time(NULL);
    struct tm *now = localtime(&t);

    char *dateStr = (char *)malloc(sizeof(char) * 11); // "YYYY-MM-DD\0"
    snprintf(dateStr, 11, "%04d-%02d-%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);

    return dateStr;
}

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

void errprint(char *errmsg)
{
    clear();
    init_pair(9, COLOR_RED, COLOR_BLACK);
    attron(COLOR_PAIR(9) | A_BOLD);
    mvprintw(0, 0, "%s", errmsg);
    attroff(COLOR_PAIR(9) | A_BOLD);
    getch();
    endwin();
    return;
}

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

bool IsValidAccountType(const char *str)
{
    const char *validTypes[] = {"fixed01", "fixed02", "fixed03", "current", "savings"};
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
