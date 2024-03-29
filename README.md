# ATM-SYS

This project is a multicomponent ATM system built in C with a mysql database.

<a href="https://docs.microsoft.com/en-us/cpp/?view=msvc-170" target="_blank" rel="noreferrer"><img src="https://raw.githubusercontent.com/danielcranney/readme-generator/main/public/icons/skills/c-colored.svg" width="36" height="36" alt="C" /></a> <a href="https://www.mysql.com/" target="_blank" rel="noreferrer"> <img src="https://raw.githubusercontent.com/devicons/devicon/master/icons/mysql/mysql-original-wordmark.svg" alt="mysql" width="40" height="40"/> </a>

## Implemented Features

* Login/Register
* Create a new account
* Check the details of existing accounts
* Update information of existing accounts
* Remove existing accounts
* Check list of owned accounts
* Make transactions


## File System (FS)

```bash
.
├── database
│   └── init.sql
├── Makefile
├── README.md
└── src
├── AccOps.c
├── auth.c
├── header.h
├── main.c
├── system.c
├── TransOps.c
└── utils.c

2 directories, 10 files
```

## Database Information

As said before, this project incorporates a mySQL DB that is used for storage. Here is an Example of one of the tables.

```SQL
CREATE TABLE IF NOT EXISTS `atm`.`Accounts` (
    `AccountID` INT NOT NULL,
    `UserName` VARCHAR(255) NOT NULL,
    `UserID` INT NULL,
    `CreationDate` VARCHAR(255) NOT NULL,
    `Country` VARCHAR(255) NOT NULL,
    `PhoneNo` VARCHAR(255) NOT NULL,
    `Balance` DOUBLE NOT NULL,
    `AccountType` VARCHAR(255) NOT NULL,
    PRIMARY KEY (`AccountID`),
    CONSTRAINT `User-AccountsFK` FOREIGN KEY (`UserName`, `UserID`) REFERENCES `atm`.`Users` (`UserName`, `UserID`) ON DELETE CASCADE ON UPDATE CASCADE
);
```
## Steps to run

1. clone this repo
2. Ensure mysql, and all proper dependencies are installed. 
3. run `make`
4. run `./atm`

## Done By akhaled01 (Abdulrahman Idrees)
