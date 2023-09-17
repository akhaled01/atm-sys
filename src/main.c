#include "header.h"
#include <ncurses.h>

void mainMenu(struct User u)
{
    int option;
    system("clear");
    printf("\n\n\t\t======= ATM =======\n\n");
    printf("\n\t\t-->> Feel free to choose one of the options below <<--\n");
    printf("\n\t\t[1]- Create a new account\n");
    printf("\n\t\t[2]- Update account information\n");
    printf("\n\t\t[3]- Check accounts\n");
    printf("\n\t\t[4]- Check list of owned account\n");
    printf("\n\t\t[5]- Make Transaction\n");
    printf("\n\t\t[6]- Remove existing account\n");
    printf("\n\t\t[7]- Transfer ownership\n");
    printf("\n\t\t[8]- Exit\n");
    scanf("%d", &option);

    switch (option)
    {
    case 1:
        CreateNewAcc(u);
        break;
    case 2:
        // student TODO : add your **Update account information** function
        // here
        break;
    case 3:
        // student TODO : add your **Check the details of existing accounts** function
        // here
        break;
    case 4:
        checkAllAccounts(u);
        break;
    case 5:
        // student TODO : add your **Make transaction** function
        // here
        break;
    case 6:
        // student TODO : add your **Remove existing account** function
        // here
        break;
    case 7:
        // student TODO : add your **Transfer owner** function
        // here
        break;
    case 8:
        exit(1);
        break;
    default:
        printf("Invalid operation!\n");
    }
};

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
    mvprintw(maxY / 2, (maxX / 2 - strlen("[1]-LOGIN  [2]-REGISTER NEW USER  [3]-EXIT SYSTEM") / 2), "[1]-LOGIN  [2]-REGISTER NEW USER  [3]-EXIT SYSTEM");
    refresh();
    attroff(COLOR_PAIR(2) | A_BOLD);
    noecho();
    scanw("%d", &option);
    echo();
    switch (option)
    {
    case 1:
        clear();
        loginMenu();
        r = 1;
        break;
    case 2:
        clear();
        CreateNewUser();
        r = 1;
        break;
    case 3:
        endwin();
        break;
    default:
        clear();
        attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(0, 0, "Please insert a Valid Operation");
        refresh();
        attroff(COLOR_PAIR(3) | A_BOLD);
        int endall = getch();
        endwin();
    }
    endwin();
}


int main()
{
    startMenu();
    return 0;
}
