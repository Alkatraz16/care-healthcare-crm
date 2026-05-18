#include "../include/auth.h"
#include "../include/patients.h"
#include "../include/database.h"
#include "../include/ui.h"

#include <iostream>
#include <limits>
#include <stdlib.h>
#include <functional>
#include <conio.h>
#include <sstream>

std::vector<User> users;
User currentUser;
bool isLoggedIn = false;
bool exitRequested = false;

void login() {
    int choice;
    char key;
    int index = 0;
    bool pressEnter = false;
    bool redraw = true;

    while (true) {
        if (redraw) {
            system("cls");
            std::cout << CARE;
            std::cout << loginMenuFrames[index];
            redraw = false;
        }

        key = _getch();

        if (key == 'w') {
            index = (index == 0) ? 2 : index - 1;
            redraw = true;
        }
        if (key == 's') {
            index = (index + 1) % 3;
            redraw = true;
        }
        if (key == '\r') {
            pressEnter = true;
            choice = index + 1;

            switch (choice) {
                case 1:
                    staffLogin(); 
                    if (isLoggedIn) return;
                    break;
                case 2:
                    patientLogin();
                    if (isLoggedIn) return;
                    break;
                case 3: 
                    exitRequested = true;
                    return;
            }
            redraw = true;
        }
    }
}

std::string hashPassword(const std::string& password) {
    if (password.empty()) return "";
    std::hash<std::string> hasher;
    return std::to_string(hasher(password));
}

void staffLogin() {
    system("cls");

    std::cout << staffLoginHeader << "\n";
    std::cout << "Enter 'cancel' as username to go back.\n\n";

    while (true) {
        std::string username, password;

        std::cout << "Username: ";
        std::getline(std::cin, username);
        if (!(std::cin >> username)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Try again.\n\n";
            continue;
        }

        if (username == "cancel") {
            std::cout << "Login cancelled.\n";
            std::cout << "Press enter to continue...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return;
        }

        std::cout << "Password: ";
        if (!(std::cin >> password)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Try again.\n\n";
            continue;
        }
        std::cin.ignore();

        if (password.empty()) {
            std::cout << "Password cannot be empty. Try again.\n\n";
            continue;
        }

        std::string hashed = hashPassword(password);

        bool found = false;
        for (User& u : users) {
            if (u.username != username) continue;
            if (u.role == "patient") {
                std::cout << "This account is not a staff account. "
                          << "Use patient login instead.\n\n";
                found = true;
                break;
            }

            found = true;

            if (u.passwordHash != hashed) {
                std::cout << "Incorrect password. Try again.\n\n";
                break;
            }

            currentUser = u;
            isLoggedIn  = true;
            system("cls");
            std::cout << "Welcome, " << currentUser.username
                      << "! (" << currentUser.role << ")\n";
            std::cout << "Press enter to continue...";
            std::cin.get();
            return;
        }

        if (!found) {
            std::cout << "User not found. Try again.\n\n";
        }
    }
}

void patientLogin() {
    system("cls");

    std::cout << patientLoginHeader << "\n";
    std::cout << "Enter 'cancel' as username to go back.\n\n";

    while (true) {
        std::string username, password;

        std::cout << "Username: ";
        std::getline(std::cin, username);
        if (!(std::cin >> username)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Try again.\n\n";
            continue;
        }

        if (username == "cancel") {
            std::cout << "Login cancelled.\n";
            std::cout << "Press enter to continue...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return;
        }

        std::cout << "Password: ";
        if (!(std::cin >> password)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Try again.\n\n";
            continue;
        }
        std::cin.ignore();

        if (password.empty()) {
            std::cout << "Password cannot be empty. Try again.\n\n";
            continue;
        }

        std::string hashed = hashPassword(password);

        bool found = false;
        for (User& u : users) {
            if (u.username != username) continue;
            if (u.role != "patient") {
                std::cout << "This account is not a patient account. "
                          << "Use staff login instead.\n\n";
                found = true;
                break;
            }

            found = true;

            if (u.passwordHash != hashed) {
                std::cout << "Incorrect password. Try again.\n\n";
                break;
            }

            currentUser = u;
            isLoggedIn  = true;
            system("cls");

            std::string patientName = currentUser.username;
            for (Patient& p : patients) {
                if (p.id == currentUser.linkedPatientId) {
                    patientName = p.name;
                    break;
                }
            }
            return;
        }

        if (!found) {
            std::cout << "User not found. Try again.\n\n";
        }
    }
}

void seedAdminAccount() {
    User admin;
    admin.id              = getNextId(users, [](const User& u){ return u.id; });
    admin.username        = "admin";
    admin.passwordHash    = hashPassword("admin123");
    admin.role            = "admin";
    admin.linkedPatientId = -1;

    users.push_back(admin);
    saveUsers();
}

void seedStaffAccount() {
    User staff;
    staff.id              = getNextId(users, [](const User& u){ return u.id; });
    staff.username        = "staff";
    staff.passwordHash    = hashPassword("staff123");
    staff.role            = "staff";
    staff.linkedPatientId = -1;

    users.push_back(staff);
    saveUsers();
}

void seedPatientAccount() {
    User patient;
    patient.id              = getNextId(users, [](const User& u){ return u.id; });
    patient.username        = "patient";
    patient.passwordHash    = hashPassword("patient123");
    patient.role            = "patient";
    patient.linkedPatientId = 11;

    users.push_back(patient);
    saveUsers();
}

std::string serializeUser(const User& u) {
    return std::to_string(u.id)              + "|" +
           u.username                         + "|" +
           u.passwordHash                     + "|" +
           u.role                             + "|" +
           std::to_string(u.linkedPatientId);
}

User deserializeUser(const std::string& line) {
    User u;
    std::string id, linkedPatientId;
    std::stringstream ss(line);

    try {
        std::getline(ss, id,              '|'); u.id              = std::stoi(id);
        std::getline(ss, u.username,      '|');
        std::getline(ss, u.passwordHash,  '|');
        std::getline(ss, u.role,          '|');
        std::getline(ss, linkedPatientId, '\n'); u.linkedPatientId = std::stoi(linkedPatientId);
    } catch (...) {
        std::cerr << "Warning: Malformed user record skipped.\n";
        u.id = -1;  // sentinel value
    }

    return u;
}

void saveUsers() {
    saveRecords<User>("data/users.csv", users, serializeUser);
}

void loadUsers() {
    loadRecords<User>("data/users.csv", users, deserializeUser);

    // remove malformed records
    users.erase(
        std::remove_if(users.begin(), users.end(),
            [](const User& u){ return u.id == -1; }),
        users.end()
    );
}