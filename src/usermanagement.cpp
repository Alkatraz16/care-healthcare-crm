// usermanagement.cpp
#include "../include/usermanagement.h"
#include "../include/auth.h"
#include "../include/patients.h"
#include "../include/database.h"

#include <iostream>
#include <limits>
#include <stdlib.h>

void printUserManagementMenu() {
    system("cls");
    std::cout << "=============================\n";
    std::cout << "      USER MANAGEMENT\n";
    std::cout << "=============================\n";
    std::cout << "1. Add Staff Account\n";
    std::cout << "2. Create Patient Portal Access\n";
    std::cout << "3. View All Users\n";
    std::cout << "4. Reset Password\n";
    std::cout << "5. Back\n";
}

void userManagementModule() {
    if (currentUser.role != "admin") {
        std::cout << "Access denied.\n";
        return;
    }

    int choice;
    do {
        printUserManagementMenu();
        std::cout << ">> ";
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cin.ignore();

        switch (choice) {
            case 1: addStaffAccount();   break;
            case 2: createPatientPortalAccess(); break;
            case 3: viewAllUsers();      break;
            case 4: resetPassword();     break;
            case 5: break;
            default:
                std::cout << "Invalid option!\n";
                break;
        }
    } while (choice != 5);
}

void addStaffAccount() {
    system("cls");
    std::cout << "=============================\n";
    std::cout << "      ADD STAFF ACCOUNT\n";
    std::cout << "=============================\n";

    User u;
    u.id              = getNextId(users, [](const User& x) { return x.id; });
    u.role            = "staff";
    u.linkedPatientId = -1;

    std::cout << "Enter Username: ";
    std::cin >> u.username;
    std::cin.ignore();

    // check if username already exists
    for (User& existing : users) {
        if (existing.username == u.username) {
            std::cout << "Username already exists.\n";
            std::cout << "Press enter to continue...";
            std::cin.get();
            return;
        }
    }

    std::string password;
    std::cout << "Enter Password: ";
    std::cin >> password;
    std::cin.ignore();

    u.passwordHash = hashPassword(password);

    users.push_back(u);
    saveUsers();

    std::cout << "Staff account created successfully.\n";
    std::cout << "Press enter to continue...";
    std::cin.get();
}

void createPatientPortalAccess() {
    system("cls");
    std::cout << "=============================\n";
    std::cout << "   CREATE PATIENT PORTAL\n";
    std::cout << "=============================\n";

    int patientId;
    std::cout << "Enter Patient ID to link: ";
    std::cin >> patientId;
    std::cin.ignore();

    // verify patient exists
    bool found = false;
    std::string patientName;
    for (Patient& p : patients) {
        if (p.id == patientId) {
            found = true;
            patientName = p.name;
            break;
        }
    }

    if (!found) {
        std::cout << "Patient ID not found.\n";
        std::cout << "Press enter to continue...";
        std::cin.get();
        return;
    }

    // check if patient already has an account
    for (User& u : users) {
        if (u.linkedPatientId == patientId) {
            std::cout << "Patient already has a portal account.\n";
            std::cout << "Press enter to continue...";
            std::cin.get();
            return;
        }
    }

    User u;
    u.id = getNextId(users, [](const User& x) { return x.id; });
    u.role = "patient";
    u.linkedPatientId = patientId;

    std::cout << "Creating portal for: " << patientName << "\n";
    std::cout << "Enter Username: ";
    std::cin >> u.username;
    std::cin.ignore();

    std::string password;
    std::cout << "Enter Temporary Password: ";
    std::cin >> password;
    std::cin.ignore();

    u.passwordHash = hashPassword(password);

    users.push_back(u);
    saveUsers();

    std::cout << "Patient portal created successfully.\n";
    std::cout << "Username: " << u.username << "\n";
    std::cout << "Press enter to continue...";
    std::cin.get();
}

void viewAllUsers() {
    system("cls");
    std::cout << "=============================\n";
    std::cout << "         ALL USERS\n";
    std::cout << "=============================\n\n";

    if (users.empty()) {
        std::cout << "No users found.\n";
        std::cout << "Press enter to continue...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    for (User& u : users) {
        std::cout << "[" << u.id << "] " << u.username
                  << " (" << u.role << ")";
        if (u.role == "patient")
            std::cout << " - Patient ID: " << u.linkedPatientId;
        std::cout << "\n";
    }

    std::cout << "\nPress enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

void resetPassword() {
    system("cls");
    std::cout << "=============================\n";
    std::cout << "       RESET PASSWORD\n";
    std::cout << "=============================\n";

    std::string username;
    std::cout << "Enter Username: ";
    std::cin >> username;
    std::cin.ignore();

    for (User& u : users) {
        if (u.username != username) continue;

        std::string newPassword;
        std::cout << "Enter New Password: ";
        std::cin >> newPassword;
        std::cin.ignore();

        u.passwordHash = hashPassword(newPassword);
        saveUsers();

        std::cout << "Password reset successfully.\n";
        std::cout << "Press enter to continue...";
        std::cin.get();
        return;
    }

    std::cout << "User not found.\n";
    std::cout << "Press enter to continue...";
    std::cin.get();
}