-- -----------------------------------------------------
-- Databse schema for atm
-- -----------------------------------------------------

-- -----------------------------------------------------
-- Done by akhaled
-- -----------------------------------------------------

-- -----------------------------------------------------
-- Table `atm`.`Users`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `atm`.`Users` (
    `UserID` INT NOT NULL AUTO_INCREMENT,
    `UserName` VARCHAR(255) NOT NULL,
    `UserPwd` VARCHAR(255) NOT NULL,
    PRIMARY KEY (`UserID`),
    UNIQUE INDEX `UserName_UNIQUE` (`UserName`) VISIBLE
);
-- -----------------------------------------------------
-- Table `atm`.`Accounts`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `atm`.`Accounts` (
    `AccountID` INT NOT NULL AUTO_INCREMENT,
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
-- -----------------------------------------------------
-- Table `atm`.`Transactions`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `atm`.`Transactions` (
    `TransactionID` INT NOT NULL AUTO_INCREMENT,
    `UserName` VARCHAR(255) NOT NULL,
    `UserID` INT NULL,
    `AccountID` INT NULL,
    `TransactionAmt` DOUBLE NULL,
    PRIMARY KEY (`TransactionID`),
    CONSTRAINT `User-TransactionFK` FOREIGN KEY (`UserName`, `UserID`) REFERENCES `atm`.`Users` (`UserName`, `UserID`) ON DELETE CASCADE ON UPDATE CASCADE,
    CONSTRAINT `Account-TransactionFK` FOREIGN KEY (`AccountID`) REFERENCES `atm`.`Accounts` (`AccountID`) ON DELETE CASCADE ON UPDATE CASCADE
);
