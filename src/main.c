#include "header.h"
#include <ncurses.h>

/// @brief this is the main Menu, terminal UI implements ncurses
/// @param u Current User
void mainMenu(struct User u)
{
    int option;
    initscr();
    clear();
    start_color();
    curs_set(0);
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(3, (maxX / 2 - strlen("Welcome Back, choose one of the options to proceed") / 2), "Welcome Back, choose one of the options to proceed");
    refresh();
    attroff(COLOR_PAIR(2) | A_BOLD);
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(7, (maxX / 2 - strlen("[1] New Account") / 2), "[1] New Account");
    mvprintw(9, (maxX / 2 - strlen("[2] Update Account") / 2), "[2] Update Account");
    mvprintw(11, (maxX / 2 - strlen("[3] Check Specific Account") / 2), "[3] Check Specific Account");
    mvprintw(13, (maxX / 2 - strlen("[4] Check All Accounts") / 2), "[4] Check All Accounts");
    mvprintw(15, (maxX / 2 - strlen("[5] Make Transaction") / 2), "[5] Make Transaction");
    mvprintw(17, (maxX / 2 - strlen("[6] Remove existing Account") / 2), "[6] Remove existing Account");
    mvprintw(19, (maxX / 2 - strlen("[7] Transfer Ownership") / 2), "[7] Transfer Ownership");
    mvprintw(21, (maxX / 2 - strlen("[8] Exit") / 2), "[8] Exit");
    refresh();
    attroff(COLOR_PAIR(1) | A_BOLD);
    noecho();
    scanw("%d", &option);
    echo();

    switch (option)
    {
    case 1:
        CreateNewAcc(u);
        break;
    case 2:
        UpdateAccInfo(u);
        break;
    case 3:
        checkSpecificAcc(u);
        break;
    case 4:
        checkAllAccounts(u);
        break;
    case 5:
        MakeTransaction(u);
        break;
    case 6:
        DelAcc(u);
        break;
    case 7:
        TransferAcc(u);
        break;
    default:
        clear();
        endwin();
        //  ensure that the terminal is reset
        system("reset");
        exit(1);
    }
};

/// @brief Entry point to the Application. 
void startMenu()
{
    int r = 0;
    int option;

    initscr();
    clear();
    start_color();
    curs_set(0);
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    // title display
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(maxY / 4, (maxX / 2 - strlen("R01 ATM BANK SYSTEM") / 2), "R01 ATM BANK SYSTEM");
    refresh();
    attroff(COLOR_PAIR(1) | A_BOLD);
    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(maxY / 2, (maxX / 2 - strlen("[1]-LOGIN  [2]-REGISTER  [3]-EXIT SYSTEM") / 2), "[1]-LOGIN  [2]-REGISTER  [3]-EXIT SYSTEM");
    refresh();
    attroff(COLOR_PAIR(2) | A_BOLD);
    noecho();
    scanw("%d", &option);
    echo();
    switch (option)
    {
    case 1:
        curs_set(2);
        clear();
        loginMenu();
        r = 1;
        break;
    case 2:
        curs_set(2);
        clear();
        CreateNewUser();
        r = 1;
        break;
    case 3:
        endwin();
        system("reset");
        break;
    default:
        clear();
        attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(0, 0, "Please insert a Valid Operation");
        refresh();
        attroff(COLOR_PAIR(3) | A_BOLD);
        getch();
        endwin();
        system("reset");
    }
    endwin();
}

// launch from here

int main()
{
    startMenu();
    return 0;
}
