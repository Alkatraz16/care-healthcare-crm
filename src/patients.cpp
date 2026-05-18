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

std::vector<Patient> patients;

void addPatientRecords() {
    Patient p;

    system("cls");

    std::cout << addPatientHeader << "\n";

    p.id = getNextId(patients, [](const Patient& px) { return px.id; }); 

    std::cout << "Assigned Patient ID - " << p.id << "\n";

    std::cout << "Enter Full Name: ";
    std::getline(std::cin, p.name);

    std::cout << "Enter Phone Number: ";
    std::getline(std::cin, p.phone);

    std::cout << "Enter Email: ";
    std::getline(std::cin, p.email);

    std::cout << "Enter Age: ";
    std::cin >> p.age;
    std::cin.ignore();

    std::cout << "Address: ";
    std::getline(std::cin, p.address);

    patients.push_back(p);
}

std::string serializePatientRecord(const Patient& p) {
    return std::to_string(p.id) + "|" + p.name + "|" + p.phone + "|" + p.email + "|" + std::to_string(p.age) + "|" + p.address;
}

Patient deserializePatientRecord(const std::string& line) {
    Patient p;
    std::string id, age;

    std::stringstream ss(line);

    std::getline(ss, id, '|');
    std::getline(ss, p.name, '|');
    std::getline(ss, p.phone, '|');
    std::getline(ss, p.email, '|');
    std::getline(ss, age, '|');
    std::getline(ss, p.address, '\n');

    p.id = std::stoi(id);
    p.age = std::stoi(age);

    return p;
}

void savePatientRecords() {
    saveRecords<Patient>("data/patients.csv", patients, serializePatientRecord);
}

void loadPatientRecords() {
    loadRecords<Patient>("data/patients.csv", patients, deserializePatientRecord);
}

void viewPatientRecords() {
    system("cls");

    std::cout << "═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════\n";
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

void updatePatientRecord() {
    int id;

    system("cls");

    std::cout << updatePatientHeader << "\n";

    std::cout << "Enter Patient ID to update: ";
    std::cin >> id;

    bool found = false;

    for (Patient& p : patients) {
        if (p.id != id) {
            continue;
        }

        found = true;

        std::cout << "Patient found: " << p.name << "\n";
        std::cout << "Press enter to continue...\n";

        showUpdateMenu(p);
        
        break;
    }

    if (!found) {
        std::cout << "Patient ID not found.\n";
        std::cout << "Press enter to continue...";
        std::cin.ignore();
        std::cin.get();
    }
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
        std::cin >> choice;
        std::cin.ignore();
            
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        switch(choice) {
            case 1:
                system("cls");

                updateName(p);
                
                std::cout << "Press enter to continue...";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                break;

            case 2:
                system("cls");

                updateEmail(p);
                
                std::cout << "Press enter to continue...";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                break;

            case 3:
                system("cls");

                updatePhone(p);
                
                std::cout << "Press enter to continue...";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                break;

            case 4:
                system("cls");

                updateAge(p);
                
                std::cout << "Press enter to continue...";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                break;

            case 5:
                system("cls");

                updateAddress(p);
                
                std::cout << "Press enter to continue...";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                break;

            case 6:
                savePatientRecords();

                choice = 7;

                std::cout << "Patient information has been successfully saved!\n";            
                std::cout << "Press enter to continue...";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
                
            case 7:
                std::cout << "Cancelling update...\n";
                break;

            default:
                std::cout << "Invalid input! Try again. (1-7)\n";
                std::cout << "Press enter to continue...";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
        }
    } while (choice != 7);
}

void updateName(Patient& p) {
    std::cout << "Current Name: " << p.name << "\n";
    std::cout << "Enter New Name:\n";
    std::cout << ">> ";

    std::getline(std::cin, p.name);

    std::cout << "Patient name successfully updated!\n";
}

void updatePhone(Patient& p) {
    std::cout << "Current Phone Number: " << p.phone << "\n";
    std::cout << "Enter New Phone Number:\n";
    std::cout << ">> ";

    std::getline(std::cin, p.phone);

    std::cout << "Patient phone number successfully updated!\n";
}

void updateEmail(Patient& p) {
    std::cout << "Current Email: " << p.email << "\n";
    std::cout << "Enter New Email:\n";
    std::cout << ">> ";

    std::getline(std::cin, p.email);
    
    std::cout << "Patient email successfully updated!\n";
}

void updateAge(Patient& p) {
    std::cout << "Current Age: " << p.age << "\n";
    std::cout << "Enter New Age:\n";
    std::cout << ">> ";

    std::cin >> p.age;
    std::cin.ignore();
    
    std::cout << "Patient age successfully updated!\n";
}

void updateAddress(Patient& p) {
    std::cout << "Current Address: " << p.address << "\n";
    std::cout << "Enter New Address:\n";
    std::cout << ">> ";

    std::getline(std::cin, p.address);
    
    std::cout << "Patient address successfully updated!\n";
}

void deletePatientRecord() {
    int id;
    bool found = false;

    system("cls");
    
    std::cout << deletePatientHeader << "\n";

    std::cout << "Enter Patient ID to delete: ";
    std::cin >> id;

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
    }

    if (!found) {
        std::cout << "Patient ID not found.\n";
        std::cout << "Press enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    char choice;
    bool valid = false;

    do {
        std::cout << "Are you sure you want to delete this patient record? (Y/N)\n";
        std::cout << ">> ";
        std::cin >> choice;
        std::cin.ignore();
        if (choice == 'y' || choice == 'Y') {
            valid = true;
            deleteRecord(patients, id);
            savePatientRecords();
            std::cout << "Patient deleted successfully.\n";
        } else if (choice == 'n' || choice == 'N') {
            valid = true;
            std::cout << "Deletion cancelled...\n";
        } else {    
            std::cout << "Invalid input! Please enter Y or N.\n";
        }
    } while (!valid);

    std::cout << "Press enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
    int id;
    bool found = false;

    std::cout << "Enter Patient ID: ";
    std::cin >> id;
    std::cin.ignore();

    for (Patient& p : patients) {
        if (p.id != id) {
            continue;
        }

        found = true;
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
    std::string name;
    bool found = false;

    std::cout << "Enter Patient Name: ";
    std::getline(std::cin, name);

    system("cls");

    for (Patient& p : patients) {
        if (p.name.find(name) == std::string::npos) continue;
        found = true;
        printPatient(p);
    }

    if (!found) std::cout << "No patients found.\n";
    
    std::cout << "Press enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void viewMyRecords() {
    system("cls");
    std::cout << "=============================\n";
    std::cout << "         MY RECORDS\n";
    std::cout << "=============================\n\n";

    for (Patient& p : patients) {
        if (p.id != currentUser.linkedPatientId) continue;

        std::cout << "ID:      " << p.id      << "\n";
        std::cout << "Name:    " << p.name    << "\n";
        std::cout << "Phone:   " << p.phone   << "\n";
        std::cout << "Email:   " << p.email   << "\n";
        std::cout << "Age:     " << p.age     << "\n";
        std::cout << "Address: " << p.address << "\n";
        break;
    }

    std::cout << "\nPress enter to continue...";
    std::cin.ignore();
    std::cin.get();
}