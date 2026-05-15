#include "../include/patients.h"
#include "../include/database.h"

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>

std::vector<Patient> patients;

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
}

void addPatientRecords() {
    Patient p;    
    p.id = getNextId(patients, [](const Patient& p){ return p.id; });
    
    system("cls");

    std::cout << "==================================\n";
    std::cout << "           ADD PATIENT\n";
    std::cout << "==================================\n";

    std::cin.ignore();

    std::cout << "Enter Full Name: ";
    std::getline(std::cin, p.name);

    std::cout << "Enter Phone Number: ";
    std::getline(std::cin, p.phone);

    std::cout << "Enter Email: ";
    std::getline(std::cin, p.email);

    std::cout << "Enter Age: ";
    while (!(std::cin >> p.age) || p.age <= 0) {
        std::cout << "Invalid input. Enter a valid age: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore();

    std::cout << "Address: ";
    std::getline(std::cin, p.address);

    patients.push_back(p);
    std::cout << "\nPatient added successfully.\n";
}

std::string serializePatientRecord(const Patient& p) {
    return std::to_string(p.id) + "|" +
           p.name                + "|" +
           p.phone               + "|" +
           p.email               + "|" +
           std::to_string(p.age) + "|" +
           p.address;
}


Patient deserializePatientRecord(const std::string& line) {
    Patient p;
    std::string id, age;
    std::stringstream ss(line);

    std::getline(ss, id,      '|'); p.id  = std::stoi(id);
    std::getline(ss, p.name,  '|');
    std::getline(ss, p.phone, '|');
    std::getline(ss, p.email, '|');
    std::getline(ss, age,     '|'); p.age = std::stoi(age);
    std::getline(ss, p.address);

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

    std::cout << std::left
              << std::setw(25) << "Name"
              << std::setw(20) << "Phone"
              << std::setw(35) << "Email"
              << std::setw(10) << "Age"
              << std::setw(40) << "Address" << "\n";
              std::cout << std::string(132, '-') << "\n";
              
    for (const Patient& p : patients) {
        std::cout << std::left
                  << std::setw(25) << p.name
                  << std::setw(20) << p.phone
                  << std::setw(35) << p.email
                  << std::setw(10) << p.age
                  << std::setw(40) << p.address << "\n";
    }
    std::cout << "Press enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

/*
g++ src/main.cpp src/patients.cpp src/interactions.cpp src/database.cpp -Iinclude -o CARE
./CARE
*/