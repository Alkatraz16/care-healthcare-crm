#include "../include/patients.h"
#include "../include/interactions.h"
#include "../include/transactions.h"
#include "../include/auth.h"
#include "../include/usermanagement.h"
#include "../include/ui.h"

#include <iostream>
#include <limits>
#include <conio.h>
#include <windows.h>

void printStaffMenu();
void printAdminMenu();
void printPatientMenu();
void patientManagementModule();
void interactionsModule();
void reportsModule();
void staffMenu();
void adminMenu();
void patientMenu();

int main() {
    // For ASCII art compatability
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    loadUsers();
    loadPatientRecords();
    loadInteractionLogs();
    loadTransactionRecords();

    if (users.empty()) {
        seedAdminAccount();
        seedStaffAccount();
        seedPatientAccount();
    }

    while (true) {
        while (!isLoggedIn && !exitRequested) {
            login();
        }

        if (!isLoggedIn) {
            std::cout << "Thank you for using Clinical Activity and Records Engine!\n";
            return 0;
        }

        if (currentUser.role == "admin") {
            adminMenu();
        }

        if (currentUser.role == "staff") {
            staffMenu();
        }

        if (currentUser.role == "patient") {
            patientMenu();
        }

        isLoggedIn = false;
        currentUser = User{};
    }

    return 0;
}

void adminMenu() {
    int choice;
    char key;
    int index = 0;
    bool pressEnter = false;
    bool redraw = true;

    while (true) {
        if (redraw) {
            system("cls");
            std::cout << mainMenu;
            std::cout << adminMenuFrames[index] << "\n";
            std::cout << "Logged in as: " << currentUser.username << "(" << currentUser.role << ")\n";
            redraw = false;
        }

        key = _getch();

        if (key == 'w') {
            index = (index == 0) ? 5 : index - 1;
            redraw = true;
        }
        if (key == 's') {
            index = (index + 1) % 6;
            redraw = true;
        }
        if (key == '\r') {
            pressEnter = true;
            choice = index + 1;
            switch (choice) {
                case 1:
                    patientManagementModule();
                    break;
                case 2:                
                    transactionManagement();
                    break;
                case 3:
                    interactionsModule();
                    break;
                case 4:
                    reportsModule();
                    break;
                case 5:
                    userManagementModule();
                    break;
                case 6:
                    return;
                    break;
            }
            redraw = true;
        }
    }
}

void staffMenu() {
    int choice;
    char key;
    int index = 0;
    bool pressEnter = false;
    bool redraw = true;

    while (true) {
        if (redraw) {
            system("cls");
            std::cout << mainMenu;
            std::cout << staffMenuFrames[index] << "\n";
            std::cout << "Logged in as: " << currentUser.username << "(" << currentUser.role << ")\n";
            redraw = false;
        }

        key = _getch();

        if (key == 'w') {
            index = (index == 0) ? 4 : index - 1;
            redraw = true;
        }
        if (key == 's') {
            index = (index + 1) % 5;
            redraw = true;
        }
        if (key == '\r') {
            pressEnter = true;
            choice = index + 1;
            switch (choice) {
                case 1:
                    patientManagementModule();
                    break;
                case 2:                
                    transactionManagement();
                    break;
                case 3:
                    interactionsModule();
                    break;
                case 4:
                    reportsModule();
                    break;
                case 5:
                    return;
                    break;
            }
            redraw = true;
        }
    }
}

void patientMenu() {
    int choice;
    char key;
    int index = 0;
    bool pressEnter = false;
    bool redraw = true;

    while (true) {
        if (redraw) {
            system("cls");
            std::cout << mainMenu;
            std::cout << patientMenuFrames[index] << "\n";
            std::cout << "Logged in as: " << currentUser.username << "(" << currentUser.role << ")\n";
            redraw = false;
        }

        key = _getch();

        if (key == 'w') {
            index = (index == 0) ? 3 : index - 1;
            redraw = true;
        }
        if (key == 's') {
            index = (index + 1) % 4;
            redraw = true;
        }
        if (key == '\r') {
            pressEnter = true;
            choice = index + 1;
            switch (choice) {
                case 1: 
                    viewMyRecords();             
                    break;
                case 2: 
                    viewMyTransactions();        
                    break;
                case 3: 
                    viewMyInteractionLogs();     
                    break;
                case 4: 
                    return;
                    break;
            }
            redraw = true;
        }
    }
}

void patientManagementModule() {
    int choice;
    char key;
    int index = 0;
    bool pressEnter = false;
    bool redraw = true;

    while (true) {
        if (redraw) {
            system("cls");
            std::cout << patientManagementModuleHeader;
            std::cout << patientManagementMenuFrames[index] << "\n";
            std::cout << "Logged in as: " << currentUser.username << "(" << currentUser.role << ")\n";
            redraw = false;
        }

        key = _getch();

        if (key == 'w') {
            index = (index == 0) ? 5 : index - 1;
            redraw = true;
        }
        if (key == 's') {
            index = (index + 1) % 6;
            redraw = true;
        }
        if (key == '\r') {
            pressEnter = true;
            choice = index + 1;
            switch (choice) {
                case 1:
                    addPatientRecords();
                    savePatientRecords();
                    break;
                case 2:
                    viewPatientRecords();
                    break;
                case 3:
                    updatePatientRecord();
                    break;
                case 4:
                    deletePatientRecord();
                    break;
                case 5:
                    searchPatientRecord();
                    break;
                case 6:
                    return;
                    break;
            }
            redraw = true;
        }
    }
}

void interactionsModule() {
    int choice;
    char key;
    int index = 0;
    bool pressEnter = false;
    bool redraw = true;

    while (true) {
        if (redraw) {
            system("cls");
            std::cout << interactionManagementModuleHeader;
            std::cout << interactionManagementMenuFrames[index] << "\n";
            std::cout << "Logged in as: " << currentUser.username << "(" << currentUser.role << ")\n";
            redraw = false;
        }

        key = _getch();

        if (key == 'w') {
            index = (index == 0) ? 4 : index - 1;
            redraw = true;
        }
        if (key == 's') {
            index = (index + 1) % 5;
            redraw = true;
        }
        if (key == '\r') {
            pressEnter = true;
            choice = index + 1;
            switch (choice) {
                case 1:
                    addInteractionRecord();
                    break;
                case 2:
                    viewInteractionLogs();
                    break;
                case 3:
                    viewLogsByPatient();
                    break;
                case 4:
                    deleteInteractionLog();
                    break;
                case 5:
                    return;
                    break;
            }
            redraw = true;
        }
    }
}