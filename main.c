#include <stdio.h>
#include "libs/users/users.h"
#include "libs/accounts/accounts.h"
#include "libs/transactions/transactions.h"
#include "libs/reports/reports.h"
#include "libs/validation/validation.h"
#include <string.h>
#include "libs/importantUsers/importantUsers.h"
#include <stdbool.h>
#include <stdlib.h>
#include "libs/activityLog/activityLog.h"
#include "xlsxwriter.h"
#include "libs/obtainData/obtainData.h"
#include "utils/utils.h"

int main() {
    // global variables
    bool running = true;
    bool loggedIn = false;
    int loggedInUser = 0;
    char* username;
    char* password, action[2000] = "";
    // application loop
    while (running) {
        // print login menu instruction
        printf("In order to use this application you need to create an account and log in.\nYou can do this using "
               "the following commands:\n  1. Create an account\n  2. Log in\n  3. Exit\n");
        // log in menu loop
        while (!loggedIn && running) {
            // the user chooses an action
            printf("Select an action:\n");
            fgets(action, 10, stdin);
            removeTrailingNewLine(action);
            // check if the action is valid
            if (validateActionLoginMenu(action)) {
                switch (atoi(action)) {
                    // create a new account option
                    case 1:
                        username = obtainUsername();
                        password = obtainPassword();
                        printf("The account was created successfully.\n");
                        createUser(username, password);
                        break;
                    // log in option
                    case 2:
                        // the user is asked to input their username and password
                        printf("Input the username.\n");
                        username = malloc(sizeof(char) * 100);
                        fgets(username, 100, stdin);
                        removeTrailingNewLine(username);
                        printf("Input the password.\n");
                        password = malloc(sizeof(char) * 100);
                        fgets(password, 100, stdin);
                        removeTrailingNewLine(password);
                        // loggedInUser has the id of the logged user or 0 if the data wasn't good
                        loggedInUser = logInUser(username, password);
                        if (loggedInUser == 0)
                            printf("The username or the password were wrong!\n");
                        else {
                            loggedIn = true;
                            printf("You have logged in successfully.\n");
                        }
                        break;
                        // exit option
                    case 3:
                        running = false;
                        printf("Thank you for using this application!\n");
                        loggedInUser = 0;
                        break;
                }
            } else {
                printf("The was not a valid action, please choose a valid one!\n");
            }

        }
        // print instructions
        if (running) {
            printf("In this application you can manage different financial accounts.\n");
            printf("In order to interact with this application you need to choose one of the following commands:"
                    "\n  1. Create financial account.\n  2. Edit financial account.\n  3. Delete financial account.\n"
                    "  4. Deposit money.\n  5. Withdraw money.\n  6. Transfer money.\n  7. Make a payment.\n  8. "
                    "Print statements of an account.\n  9. Print balances.\n  10. Print transactions of an account.\n"
                    "  11. Print the expenses of an account.\n  12. Add user to the list of important users.\n"
                    "  13. Update an important user.\n  14. Delete an important user.\n  15. Display important users.\n  16. Log out.\n");
        }
        // application menu loop
        while (loggedIn && running) {
            // needed variables
            char accountName[2000], accountType[200], accountId[200], confirmation[200], value[200], accountIdToTransferTo[2000], userId[200], description[2000], importantUsername[2000];
            bool validAccountName = false, validAccountType = false, validValue = false, validDescription = false, validImportantUsername = false;
            bool validAccountId = false, validConfirmation = false, validAccountIdToTransfer = false, validUserId = false;
            printf("Select an action:\n");
            scanf("%s", action);

            if (validateActionMenu(action)) {
                switch (atoi(action)) {
                    // financial account creation
                    case 1:
                        // setup variables
                        validAccountName = false;
                        validAccountType = false;
                        // loop that runs until a valid account name is received
                        while (!validAccountName) {
                            printf("Input the name of the account.\n");
                            scanf("%s", accountName);
                            validAccountName = validateUsername(accountName);
                            if (!validAccountName)
                                printf("The account name is invalid, please input a valid one.\n");
                        }
                        // loop that runs until a valid account type is received
                        while (!validAccountType) {
                            printf("Choose the type of the account:\n  1. Savings\n  2. Checking\n  3. Credits\n");
                            scanf("%s", accountType);
                            validAccountType = validateType(accountType);
                            if (!validAccountType)
                                printf("The chosen account type is invalid, please choose a valid one.\n");
                        }
                        // Check if the account name is already used and give a proper response.
                        if (createAccount(loggedInUser, accountName, accountType))
                            printf("The account was created successfully.\n");
                        else
                            printf("There is already an account that uses the input name\n");
                        break;
                    // edit financial account
                    case 2:
                        // if the logged in user doesn't have any account show a special message
                        if (returnNumberOfAccounts(loggedInUser) == 0)
                            printf("In order to edit an account you need to create one, please create one before"
                                   " trying to edit it.\n");
                        else {
                            // prepare variables for input loops
                            validAccountId = false;
                            validAccountName = false;
                            validAccountType = false;
                            printf("Input the ID of the financial account that will be edited:\n");
                            displayAccounts(loggedInUser);
                            // choosing an account loop
                            while (!validAccountId) {
                                scanf("%s", accountId);
                                // if the id is invalid (as in it is not a valid integer) display a special message
                                if (!validateId(accountId))
                                    printf("The input account id is invalid, please input a valid one.\n");
                                else {
                                    // if the account id was not chosen from the displayed list print a message
                                    validAccountId = validateAccountId(loggedInUser, atoi(accountId));
                                    if (!validAccountId)
                                        printf("You do not own the account with the input id, please input the"
                                               "id of an account from the list.\n");
                                }
                            }
                            printf("Enter the new name of the account\n");
                            // choosing new account name loop
                            while (!validAccountName) {
                                scanf("%s", accountName);
                                validAccountName = validateUsername(accountName);
                                if (!validAccountName)
                                    printf("The input account name is invalid, please input a valid one.\n");
                            }
                            // choosing new type loop
                            while (!validAccountType) {
                                printf("Choose the new type of the account:\n  1. Savings\n  2. Checking\n  3. Credits\n");
                                scanf("%s", accountType);
                                validAccountType = validateType(accountType);
                                if (!validAccountType)
                                    printf("The chosen account type is invalid, please choose a valid one.\n");
                            }
                            // Check if the update can be done,
                            if (editAccount(loggedInUser, atoi(accountId), accountName, accountType))
                                printf("The account was updated successfully.\n");
                            else
                                printf("The input name is already used by another account used by you.\n");

                        }
                        break;
                        // delete account function
                    case 3:
                        // if the logged in user doesn't have any account show a special message
                        if (returnNumberOfAccounts(loggedInUser) == 0)
                            printf("In order to delete an account you need to create one, please create one before"
                                   " trying to delete it.\n");
                        else {
                            // prepare variables for input loops
                            validAccountId = false;
                            validConfirmation = false;
                            printf("Input the ID of the financial account that will be deleted:\n");
                            displayAccounts(loggedInUser);
                            // choosing an account loop
                            while (!validAccountId) {
                                scanf("%s", accountId);
                                // if the id is invalid (as in it is not a valid integer) display a special message
                                if (!validateId(accountId))
                                    printf("The input account id is invalid, please input a valid one.\n");
                                else {
                                    // if the account id was not chosen from the displayed list print a message
                                    validAccountId = validateAccountId(loggedInUser, atoi(accountId));
                                    if (!validAccountId)
                                        printf("You do not own the account with the input id, please input the"
                                               "id of an account from the list.\n");
                                }
                            }
                            // ask for confirmation before deleting the account
                            printf("Do you want to delete selected financial account? (y/n)\n");
                            while (!validConfirmation) {
                                scanf("%s", confirmation);
                                validConfirmation = validateConfirmation(confirmation);
                                if (!validConfirmation)
                                    printf("The input confirmation is not valid, please input a valid one.\n");
                            }
                            // display a message depending on the confirmation
                            if (strcmp(confirmation, "y") == 0) {
                                deleteAccount(loggedInUser, atoi(accountId));
                                printf("The account was deleted successfully.\n");
                            }
                            else
                                printf("The account with the input id was not deleted.\n");
                        }
                        break;
                    // deposit money
                    case 4:
                        // if the user does not have any account display a special message
                        if (returnNumberOfAccounts(loggedInUser) == 0)
                            printf("In order to deposit money into an account you need to create one, please "
                                   "create one before trying to deposit money into it.\n");
                        else {
                            // prepare variables for input loops
                            validAccountId = false;
                            validValue = false;
                            printf("Input the id of the account in which the money will be deposited.\n");
                            displayAccounts(loggedInUser);
                            // choosing an account loop
                            while (!validAccountId) {
                                scanf("%s", accountId);
                                // if the id is invalid (as in it is not a valid integer) display a special message
                                if (!validateId(accountId))
                                    printf("The input account id is invalid, please input a valid one.\n");
                                else {
                                    // if the account id was not chosen from the displayed list print a message
                                    validAccountId = validateAccountId(loggedInUser, atoi(accountId));
                                    if (!validAccountId)
                                        printf("You do not own the account with the input id, please input the"
                                               "id of an account from the list.\n");
                                }
                            }
                            // getting a valid value loop
                            printf("Input the amount of money that will be deposited.\n");
                            while (!validValue) {
                                scanf("%s", value);
                                validValue = validateValue(value);
                                if (!validValue)
                                    printf("The input value is not valid, please input a valid one.\n");
                            }
                            // deposit the money and notify the user
                            deposit(loggedInUser, atoi(accountId), atof(value));
                            printf("The deposit was made successfully!\n");
                        }
                        break;
                    // withdraw money
                    case 5:
                        // if the user does not have any account display a special message
                        if (returnNumberOfAccounts(loggedInUser) == 0)
                            printf("In order to withdraw money from an account you need to create one, please "
                                   "create one before trying to withdraw money from it.\n");
                        else {
                            // prepare variables for input loops
                            validAccountId = false;
                            validValue = false;
                            printf("Input the id of the account from which the money will be withdrawn.\n");
                            displayAccounts(loggedInUser);
                            // choosing an account loop
                            while (!validAccountId) {
                                scanf("%s", accountId);
                                // if the id is invalid (as in it is not a valid integer) display a special message
                                if (!validateId(accountId))
                                    printf("The input account id is invalid, please input a valid one.\n");
                                else {
                                    // if the account id was not chosen from the displayed list print a message
                                    validAccountId = validateAccountId(loggedInUser, atoi(accountId));
                                    if (!validAccountId)
                                        printf("You do not own the account with the input id, please input the"
                                               "id of an account from the list.\n");
                                }
                            }
                            // getting a valid value loop
                            printf("Input the amount of money that will be withdrawn\n");
                            while (!validValue) {
                                scanf("%s", value);
                                validValue = validateValue(value);
                                if (!validValue)
                                    printf("The input value is not valid, please input a valid one.\n");
                            }
                            // withdraw the money and notify the user
                            if (withdraw(loggedInUser, atoi(accountId), atof(value)))
                                printf("The withdrawal was made successfully!\n");
                            else
                                printf("The withdrawal was unsuccessful, you can not withdraw more money then are"
                                       " in the account.\n");
                        }
                        break;
                    // transfer money
                    case 6:
                        // if the user does not have any account display a special message
                         if (returnNumberOfAccounts(loggedInUser) == 0)
                            printf("In order to transfer money from an account to another you need to create one, please "
                                   "create one before trying to transfer money from it to another.\n");
                        else {
                            // prepare variables for input loops
                            validAccountId = false;
                            validAccountIdToTransfer = false;
                            validValue = false;
                            printf("Input the id of the account from which the money will be transferred.\n");
                            displayAccounts(loggedInUser);
                            // choosing an account to transfer from
                            while (!validAccountId) {
                                scanf("%s", accountId);
                                // if the id is invalid (as in it is not a valid integer) display a special message
                                if (!validateId(accountId))
                                    printf("The input account id is invalid, please input a valid one.\n");
                                else {
                                    // if the account id was not chosen from the displayed list print a message
                                    validAccountId = validateAccountId(loggedInUser, atoi(accountId));
                                    if (!validAccountId)
                                        printf("You do not own the account with the input id, please input the"
                                               "id of an account from the list.\n");
                                }
                            }
                            // choosing an account to transfer to
                            printf("Input the id of the account to which to money will be transferred.\n");
                            while (!validAccountIdToTransfer) {
                                scanf("%s", accountIdToTransferTo);
                                // if the id is invalid (as in it is not a valid integer) display a special message
                                if (!validateId(accountIdToTransferTo))
                                    printf("The input account id is invalid, please input a valid one.\n");
                                else {
                                    // if the account id does not belong to any account display a special message
                                    validAccountIdToTransfer = checkIfAccountExists(atoi(accountIdToTransferTo));
                                    if (!validAccountIdToTransfer)
                                        printf("The input id does not belong the any account, you need to input "
                                               "an id that belongs to an account.\n");
                                }
                            }
                            // get the value
                            printf("Input the amount of money that will be transferred.\n");
                            while (!validValue) {
                                scanf("%s", value);
                                validValue = validateValue(value);
                                if (!validValue)
                                    printf("The input value is not valid, please input a valid one.\n");
                            }
                            // get confirmation
                            printf("Do you want to transfer %f to the account with the id of %d? (y/n)\n",
                                   atof(value), atoi(accountIdToTransferTo));
                            while (!validConfirmation) {
                                scanf("%s", confirmation);
                                validConfirmation = validateConfirmation(confirmation);
                                if (!validConfirmation)
                                    printf("The input confirmation is not valid, please input a valid one.\n");
                            }
                            // transfer the money and notify the user depending on the results
                            if (strcmp(confirmation, "y") == 0)
                                if (transfer(loggedInUser, atoi(accountId), atoi(accountIdToTransferTo), atof(value)))
                                    printf("The transfer was successful!\n");
                                else
                                    printf("You cannot transfer money to the same account.\n");
                            else
                                printf("The transfer was cancelled.\n");
                        }
                        break;
                    //make a payment feature
                    case 7:
                        // if the user does not have any account display a special message
                        if (returnNumberOfAccounts(loggedInUser) == 0)
                            printf("In order to withdraw money from an account you need to create one, please "
                                   "create one before trying to withdraw money from it.\n");
                        else {
                            // prepare variables for input loops
                            validAccountId = false;
                            validValue = false;
                            printf("Input the id of the account from which the payment will be made.\n");
                            displayAccounts(loggedInUser);
                            // choosing an account loop
                            while (!validAccountId) {
                                scanf("%s", accountId);
                                // if the id is invalid (as in it is not a valid integer) display a special message
                                if (!validateId(accountId))
                                    printf("The input account id is invalid, please input a valid one.\n");
                                else {
                                    // if the account id was not chosen from the displayed list print a message
                                    validAccountId = validateAccountId(loggedInUser, atoi(accountId));
                                    if (!validAccountId)
                                        printf("You do not own the account with the input id, please input the"
                                               "id of an account from the list.\n");
                                }
                            }
                            // getting a valid value loop
                            printf("Input the amount of money that will be withdrawn\n");
                            while (!validValue) {
                                scanf("%s", value);
                                validValue = validateValue(value);
                                if (!validValue)
                                    printf("The input value is not valid, please input a valid one.\n");
                            }
                            // make the payment and notify the user
                            if (payment(loggedInUser, atoi(accountId), atof(value)))
                                printf("The withdrawal was made successfully!\n");
                            else
                                printf("The payment was unsuccessful, you can not pay more money then are"
                                       " in the account.\n");
                        }
                        break;
                    // display statements
                    case 8:
                        validAccountId = false;
                        printf("Input the id of the account:\n");
                        displayAccounts(loggedInUser);
                        // choosing an account to display the statements of
                        while (!validAccountId) {
                            scanf("%s", accountId);
                            // if the id is invalid (as in it is not a valid integer) display a special message
                            if (!validateId(accountId))
                                printf("The input account id is invalid, please input a valid one.\n");
                            else {
                                // if the account id was not chosen from the displayed list print a message
                                validAccountId = validateAccountId(loggedInUser, atoi(accountId));
                                if (!validAccountId)
                                    printf("You do not own the account with the input id, please input the"
                                           "id of an account from the list.\n");
                            }
                        }
                        // print all statements
                        printf("Statements:\n");
                        statements(atoi(accountId));
                        break;
                    // print balances feature
                    case 9:
                        // print balances
                        printf("Balances:\n");
                        balances(loggedInUser);
                        break;
                    // display transactions
                    case 10:
                        validAccountId = false;
                        printf("Input the id of the account:\n");
                        displayAccounts(loggedInUser);
                        // choosing an account to display the transactions of
                        while (!validAccountId) {
                            scanf("%s", accountId);
                            // if the id is invalid (as in it is not a valid integer) display a special message
                            if (!validateId(accountId))
                                printf("The input account id is invalid, please input a valid one.\n");
                            else {
                                // if the account id was not chosen from the displayed list print a message
                                validAccountId = validateAccountId(loggedInUser, atoi(accountId));
                                if (!validAccountId)
                                    printf("You do not own the account with the input id, please input the"
                                           "id of an account from the list.\n");
                            }
                        }
                        // displaying the transactions
                        transactions(atoi(accountId));
                        break;
                    //display the expenses of an account
                    case 11:
                        validAccountId = false;
                        printf("Input the id of the account:\n");
                        displayAccounts(loggedInUser);
                        // choosing an account to display the transactions of
                        while (!validAccountId) {
                            scanf("%s", accountId);
                            // if the id is invalid (as in it is not a valid integer) display a special message
                            if (!validateId(accountId))
                                printf("The input account id is invalid, please input a valid one.\n");
                            else {
                                // if the account id was not chosen from the displayed list print a message
                                validAccountId = validateAccountId(loggedInUser, atoi(accountId));
                                if (!validAccountId)
                                    printf("You do not own the account with the input id, please input the"
                                           "id of an account from the list.\n");
                            }
                        }
                        // displaying the expenses
                        expenses(atoi(accountId));
                        break;
                    // adding important user
                    case 12:
                         validUserId = false;
                         validDescription = false;
                         printf("Input the id of the important user.\n");
                         // get user id
                         while (!validUserId) {
                             scanf("%s", userId);
                             // if the id is invalid (as in it is not a valid integer) display a special message
                             if (!validateId(userId))
                                 printf("The input account id is invalid, please input a valid one.\n");
                             else {
                                 // if the user id does not exist display a message
                                 validUserId = checkIfUserExists(atoi(userId));
                                 if (!validUserId)
                                     printf("The input id does not belong to any user, please input an id that belongs to an user.\n");
                                 // if the id of the user that is logged in is input display a message
                                 if (atoi(userId) == loggedInUser) {
                                     printf("The input id belongs to your account, please input another id.\n");
                                     validUserId = false;
                                 }
                             }
                         }
                         // get description
                         printf("Input a description for the important user.\n");
                         while (!validDescription) {
                             getchar();
                             fgets(description, 2000, stdin);
                             description[strlen(description) - 1] = 0;
                             validDescription = validateDescription(description);
                             if (!validDescription)
                                 printf("The input description is not valid, please input a valid one.\n");
                         }
                         // add important user
                         if (addUser(loggedInUser, atoi(userId), description))
                             printf("The user was added successfully in the list of important users!\n");
                         else
                             printf("The chosen user is already in the list of important users.\n");
                         break;
                    // edit important user
                    case 13:
                        // if the logged in user doesn't have any important users return a message
                        if (!checkIfUserHasImportantUsers(loggedInUser))
                            printf("In order to edit an important user you need to add one, please add one before"
                                   " trying to edit it.\n");
                        else {
                            // prepare variables for input loops
                            validUserId = false;
                            validDescription = false;
                            printf("Input the id of the important user that will be edited:\n");
                            displayImportantUsers(loggedInUser);
                            // choosing a user loop
                            while (!validUserId) {
                                scanf("%s", userId);
                                // if the id is invalid (as in it is not a valid integer) display a special message
                                if (!validateId(userId))
                                    printf("The input user id is invalid, please input a valid one.\n");
                                else {
                                    // if the user id was not chosen from the displayed list print a message
                                    validUserId = checkImportantUser(loggedInUser, atoi(userId));
                                    if (!validUserId)
                                        printf("You do not have the user with the input id as an important user,"
                                               " please input the id of another important user.\n");
                                }
                            }
                            printf("Input the new description of the important user.\n");
                            // choosing new description loop
                            while (!validDescription) {
                                getchar();
                                fgets(description, 2000, stdin);
                                description[strlen(description) - 1] = 0;
                                validDescription= validateDescription(description);
                                if (!validDescription)
                                    printf("The input description is invalid, please input another one.\n");
                            }
                            editUser(loggedInUser, validUserId, description);
                            printf("The user was updated successfully!\n");
                        }
                        break;
                    // delete important user
                    case 14:
                        if (!checkIfUserHasImportantUsers(loggedInUser))
                            printf("In order to delete an important user you need to add one, please add one before"
                                   " trying to delete it.\n");
                        else {
                            // prepare variables for input loops
                            validUserId = false;
                            validConfirmation = false;
                            printf("Input the id of the important user that will be deleted:\n");
                            displayImportantUsers(loggedInUser);
                            // choosing an account loop
                            while (!validUserId) {
                                scanf("%s", userId);
                                // if the id is invalid (as in it is not a valid integer) display a special message
                                if (!validateId(userId))
                                    printf("The input user id is invalid, please input a valid one.\n");
                                else {
                                    // if the account id was not chosen from the displayed list print a message
                                    validUserId = checkImportantUser(loggedInUser, atoi(userId));
                                    if (!validUserId)
                                        printf("You do not have the user with the input id as an important user,"
                                               " please input the id of another important user.\n");
                                }
                            }
                            // ask for confirmation before deleting the important user
                            printf("Do you want to delete the selected important user? (y/n)\n");
                            while (!validConfirmation) {
                                scanf("%s", confirmation);
                                validConfirmation = validateConfirmation(confirmation);
                                if (!validConfirmation)
                                    printf("The input confirmation is not valid, please input a valid one.\n");
                            }
                            // display a message depending on the confirmation
                            if (strcmp(confirmation, "y") == 0) {
                                deleteUser(loggedInUser, atoi(userId));
                                printf("The user was deleted successfully from the list of important users.\n");
                            }
                            else
                                printf("The important user with the input id was not deleted.\n");
                        }
                        break;
                    // display important users
                    case 15:
                        displayImportantUsers(loggedInUser);
                        break;
                    // log out
                    case 16:
                        logOutUser(loggedInUser);
                        loggedInUser = 0;
                        loggedIn = false;
                        break;
                }
            } else {

            }
        }
    }
}

