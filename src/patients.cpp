#include "../include/patients.h"
#include "../include/database.h"
#include "../include/auth.h"
#include "../include/ui.h"

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <conio.h>
#include <algorithm>

std::vector<Patient> patients;

// ─── HELPERS ──────────────────────────────────────────────────────────────────

static std::string toLower(const std::string& value) {
    std::string lower = value;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower;
}

static bool containsIgnoreCase(const std::string& text, const std::string& query) {
    return toLower(text).find(toLower(query)) != std::string::npos;
}

// ─── MENU ─────────────────────────────────────────────────────────────────────

void printPatientManagementMenu() {
    system("cls");
    std::cout << "=================================\n";
    std::cout << "       PATIENT MANAGEMENT\n";
    std::cout << "=================================\n";
    std::cout << "1. Add new patient\n";
    std::cout << "2. View patient list\n";
    std::cout << "3. Update patient information\n";
    std::cout << "4. Delete patient record\n";
    std::cout << "5. Search patient by ID or name\n";
    std::cout << "6. Exit to main menu\n";
}

// ─── ADD ──────────────────────────────────────────────────────────────────────

void addPatientRecords() {
    Patient p;

    system("cls");

    std::cout << addPatientHeader << "\n";

    p.id = getNextId(patients, [](const Patient& px) { return px.id; }); 

    // name — must not be empty
    while (true) {
        std::cout << "Enter Full Name: ";
        std::getline(std::cin, p.name);
        if (p.name.empty()) {
            std::cout << "Name cannot be empty.\n";
            continue;
        }
        break;
    }

    // phone — must not be empty
    while (true) {
        std::cout << "Enter Phone Number: ";
        std::getline(std::cin, p.phone);
        if (p.phone.empty()) {
            std::cout << "Phone number cannot be empty.\n";
            continue;
        }
        break;
    }

    // email — must not be empty
    while (true) {
        std::cout << "Enter Email: ";
        std::getline(std::cin, p.email);
        if (p.email.empty()) {
            std::cout << "Email cannot be empty.\n";
            continue;
        }
        break;
    }

    // age — must be positive integer
    while (true) {
        std::cout << "Enter Age: ";
        if (!(std::cin >> p.age) || p.age <= 0 || p.age > 150) {
            std::cout << "Invalid age. Enter a number between 1 and 150.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        break;
    }

    // address — optional
    std::cout << "Address (optional): ";
    std::getline(std::cin, p.address);

    patients.push_back(p);
    std::cout << "\nPatient added successfully.\n";
    std::cout << "Press enter to continue...";
    std::cin.get();
}

// ─── SERIALIZATION ────────────────────────────────────────────────────────────

std::string serializePatientRecord(const Patient& p) {
    return std::to_string(p.id) + "|" +
           p.name               + "|" +
           p.phone              + "|" +
           p.email              + "|" +
           std::to_string(p.age) + "|" +
           p.address;
}

Patient deserializePatientRecord(const std::string& line) {
    Patient p;
    std::string id, age;
    std::stringstream ss(line);

    try {
        std::getline(ss, id,      '|'); p.id  = std::stoi(id);
        std::getline(ss, p.name,  '|');
        std::getline(ss, p.phone, '|');
        std::getline(ss, p.email, '|');
        std::getline(ss, age,     '|'); p.age = std::stoi(age);
        std::getline(ss, p.address);
    } catch (...) {
        std::cerr << "Warning: Malformed patient record skipped.\n";
        p.id = -1;  // sentinel
    }

    return p;
}

// ─── PERSISTENCE ──────────────────────────────────────────────────────────────

void savePatientRecords() {
    saveRecords<Patient>("data/patients.csv", patients, serializePatientRecord);
}

void loadPatientRecords() {
    loadRecords<Patient>("data/patients.csv", patients, deserializePatientRecord);

    // remove malformed records
    patients.erase(
        std::remove_if(patients.begin(), patients.end(),
            [](const Patient& p){ return p.id == -1; }),
        patients.end()
    );
}

// ─── VIEW ─────────────────────────────────────────────────────────────────────

void viewPatientRecords() {
    system("cls");

    std::cout << "═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════\n";
    if (patients.empty()) {
        std::cout << "No patient records found.\n";
        std::cout << "Press enter to continue...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    std::cout << std::left
              << std::setw(10) << "ID"
              << std::setw(40) << "Name"
              << std::setw(20) << "Phone"
              << std::setw(35) << "Email"
              << std::setw(10) << "Age"
              << std::setw(40) << "Address" << "\n";
    std::cout << "═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════\n";
              
    for (Patient& p : patients) {
        std::cout << std::left
                  << std::setw(10) << p.id
                  << std::setw(40) << p.name
                  << std::setw(20) << p.phone
                  << std::setw(35) << p.email
                  << std::setw(10) << p.age
                  << std::setw(40) << p.address << "\n";
    }
    std::cout << "═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════\n";
    std::cout << "Press enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// ─── UPDATE ───────────────────────────────────────────────────────────────────

void updatePatientRecord() {
    system("cls");

    std::cout << updatePatientHeader << "\n";

    if (patients.empty()) {
        std::cout << "No patient records found.\n";
        std::cout << "Press enter to continue...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    std::cout << "Enter Patient ID to update: ";
    int id;
    while (!(std::cin >> id)) {
        std::cout << "Invalid input. Enter a number: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    for (Patient& p : patients) {
        if (p.id != id) continue;

        std::cout << "Patient found: " << p.name << "\n";
        showUpdateMenu(p);
        return;
    }

    std::cout << "Patient ID " << id << " not found.\n";
    std::cout << "Press enter to continue...";
    std::cin.get();
}

void showUpdateMenu(Patient& p) {
    int choice;

    do {
        system("cls");

        std::cout << currentPatientInformationHeader << "\n";
        std::cout << "Patient ID: " << p.id << "\n";
        std::cout << "1. Name: " << p.name << "\n";
        std::cout << "2. Email: " << p.email << "\n";
        std::cout << "3. Phone: " << p.phone << "\n";
        std::cout << "4. Age: " << p.age << "\n";
        std::cout << "5. Address: " << p.address << "\n\n";
        std::cout << "6. Save Changes\n";
        std::cout << "7. Cancel\n\n";
        std::cout << ">> ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Enter a number (1-7).\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                system("cls");
                updateName(p);
                std::cout << "Press enter to continue...";
                std::cin.get();
                break;
            case 2:
                system("cls");
                updateEmail(p);
                std::cout << "Press enter to continue...";
                std::cin.get();
                break;
            case 3:
                system("cls");
                updatePhone(p);
                std::cout << "Press enter to continue...";
                std::cin.get();
                break;
            case 4:
                system("cls");
                updateAge(p);
                std::cout << "Press enter to continue...";
                std::cin.get();
                break;
            case 5:
                system("cls");
                updateAddress(p);
                std::cout << "Press enter to continue...";
                std::cin.get();
                break;
            case 6:
                savePatientRecords();
                std::cout << "Patient information saved successfully.\n";
                std::cout << "Press enter to continue...";
                std::cin.get();
                choice = 7;  // exit loop
                break;
            case 7:
                std::cout << "Cancelling update...\n";
                break;
            default:
                std::cout << "Invalid input. Enter a number (1-7).\n";
                std::cout << "Press enter to continue...";
                std::cin.get();
                break;
        }
    } while (choice != 7);
}

void updateName(Patient& p) {
    std::cout << "Current Name: " << p.name << "\n";
    while (true) {
        std::cout << "Enter New Name: ";
        std::string input;
        std::getline(std::cin, input);
        if (input.empty()) {
            std::cout << "Name cannot be empty.\n";
            continue;
        }
        p.name = input;
        break;
    }
    std::cout << "Name updated successfully.\n";
}

void updatePhone(Patient& p) {
    std::cout << "Current Phone: " << p.phone << "\n";
    while (true) {
        std::cout << "Enter New Phone Number: ";
        std::string input;
        std::getline(std::cin, input);
        if (input.empty()) {
            std::cout << "Phone number cannot be empty.\n";
            continue;
        }
        p.phone = input;
        break;
    }
    std::cout << "Phone updated successfully.\n";
}

void updateEmail(Patient& p) {
    std::cout << "Current Email: " << p.email << "\n";
    while (true) {
        std::cout << "Enter New Email: ";
        std::string input;
        std::getline(std::cin, input);
        if (input.empty()) {
            std::cout << "Email cannot be empty.\n";
            continue;
        }
        p.email = input;
        break;
    }
    std::cout << "Email updated successfully.\n";
}

void updateAge(Patient& p) {
    std::cout << "Current Age: " << p.age << "\n";
    while (true) {
        std::cout << "Enter New Age: ";
        int age;
        if (!(std::cin >> age) || age <= 0 || age > 150) {
            std::cout << "Invalid age. Enter a number between 1 and 150.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        p.age = age;
        break;
    }
    std::cout << "Age updated successfully.\n";
}

void updateAddress(Patient& p) {
    std::cout << "Current Address: " << p.address << "\n";
    std::cout << "Enter New Address (optional): ";
    std::getline(std::cin, p.address);
    std::cout << "Address updated successfully.\n";
}

// ─── DELETE ───────────────────────────────────────────────────────────────────

void deletePatientRecord() {
    system("cls");
    
    std::cout << deletePatientHeader << "\n";

    if (patients.empty()) {
        std::cout << "No patient records found.\n";
        std::cout << "Press enter to continue...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    std::cout << "Enter Patient ID to delete: ";
    int id;
    while (!(std::cin >> id)) {
        std::cout << "Invalid input. Enter a number: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore();

    for (Patient& p : patients) {
        if (p.id != id) {
            continue;
        }

        found = true;
        system("cls");
        std::cout << patientDeletingHeader << "\n";
        std::cout << "ID:      " << p.id      << "\n";
        std::cout << "Name:    " << p.name    << "\n";
        std::cout << "Phone:   " << p.phone   << "\n";
        std::cout << "Email:   " << p.email   << "\n";
        std::cout << "Age:     " << p.age     << "\n";
        std::cout << "Address: " << p.address << "\n\n";
        break;
    }

    if (!found) {
        std::cout << "Patient ID " << id << " not found.\n";
        std::cout << "Press enter to continue...";
        std::cin.get();
        return;
    }

    while (true) {
        std::cout << "Are you sure you want to delete this patient? (Y/N): ";
        char confirm;
        std::cin >> confirm;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (confirm == 'y' || confirm == 'Y') {
            deleteRecord(patients, id);
            savePatientRecords();
            std::cout << "Patient deleted successfully.\n";
            break;
        } else if (confirm == 'n' || confirm == 'N') {
            std::cout << "Deletion cancelled.\n";
            break;
        } else {
            std::cout << "Invalid input. Enter Y or N.\n";
        }
    }

    std::cout << "Press enter to continue...";
    std::cin.get();
}

void printPatient(const Patient& p) {
    std::cout << patientInformationHeader << "\n";
    std::cout << "ID:      " << p.id      << "\n";
    std::cout << "Name:    " << p.name    << "\n";
    std::cout << "Phone:   " << p.phone   << "\n";
    std::cout << "Email:   " << p.email   << "\n";
    std::cout << "Age:     " << p.age     << "\n";
    std::cout << "Address: " << p.address << "\n\n";
}


void searchPatientRecord() {
    int choice;
    char key;
    int index = 0;
    bool pressEnter = false;
    bool redraw = true;

    while (true) {
        if (redraw) {
            system("cls");
            std::cout << searchPatientHeader;
            std::cout << searchMenuFrames[index] << "\n";
            redraw = false;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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
                    searchPatientById();
                    break;
                case 2:                
                    searchPatientByName();
                    break;
                case 3:
                    return;
                    break;
            }
            redraw = true;
        }
    }
}

void searchPatientById() {
    std::cout << "Enter Patient ID: ";
    int id;
    while (!(std::cin >> id)) {
        std::cout << "Invalid input. Enter a number: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    for (const Patient& p : patients) {
        if (p.id != id) continue;

        system("cls");
        printPatient(p);
    }

    if (!found) {
        std::cout << "Patient ID not found.\n";
        std::cout << "Press enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    std::cout << "Press enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void searchPatientByName() {
    std::cout << "Enter Patient Name: ";
    std::string name;
    std::getline(std::cin, name);

    if (name.empty()) {
        std::cout << "Search term cannot be empty.\n";
        return;
    }

    system("cls");
    bool found = false;

    for (const Patient& p : patients) {
        if (!containsIgnoreCase(p.name, name)) continue;  // ← case insensitive now

        found = true;
        printPatient(p);
    }

    if (!found) std::cout << "No patients found.\n";
    
    std::cout << "Press enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// ─── PATIENT VIEW ─────────────────────────────────────────────────────────────

void viewMyRecords() {
    system("cls");
    std::cout << "=============================\n";
    std::cout << "         MY RECORDS\n";
    std::cout << "=============================\n\n";

    if (currentUser.linkedPatientId <= 0) {
        std::cout << "No patient record linked to your account.\n";
        std::cout << "\nPress enter to continue...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    bool found = false;
    for (const Patient& p : patients) {
        if (p.id != currentUser.linkedPatientId) continue;

        found = true;
        std::cout << "ID:      " << p.id      << "\n";
        std::cout << "Name:    " << p.name    << "\n";
        std::cout << "Phone:   " << p.phone   << "\n";
        std::cout << "Email:   " << p.email   << "\n";
        std::cout << "Age:     " << p.age     << "\n";
        std::cout << "Address: " << p.address << "\n";
        break;
    }

    if (!found)
        std::cout << "Patient record not found.\n";

    std::cout << "\nPress enter to continue...";
    std::cin.ignore();
    std::cin.get();
}