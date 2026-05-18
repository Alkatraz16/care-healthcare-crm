#include "../include/interactions.h"
#include "../include/patients.h"
#include "../include/database.h"
#include "../include/auth.h"
#include "../include/ui.h"

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <limits>
#include <chrono>
#include <ctime>
#include <conio.h>

std::vector<Interaction> interactions;

void addInteractionRecord() {
    Interaction i;

    system("cls");

    std::cout << addInteractionHeader;

    std::cout << "Enter Patient ID: ";
    std::cin >> i.patientId;
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore();


    bool found = false;

    for (Patient& p : patients) {
        if (p.id == i.patientId) { 
            found = true; 
            break; 
        }
    }

    if (!found) {
        std::cout << "Patient ID not found.\n";
        std::cout << "Press enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    int choice;
    char key;
    int index = 0;
    bool pressEnter = false;
    bool redraw = true;

    while (!pressEnter) {
        if (redraw) {
            system("cls");
            std::cout << selectInteractionTypeHeader;
            std::cout << interactionTypeFrames[index] << "\n";
            redraw = false;
        }

        key = _getch();

        if (key == 'w') {
            index = (index == 0) ? 8 : index - 1;
            redraw = true;
        }
        if (key == 's') {
            index = (index + 1) % 9;
            redraw = true;
        }
        if (key == '\r') {
            switch (choice) {
                case 1: i.type = "Clinic Visit"; break;
                case 2: i.type = "Appointment Reminder"; break;
                case 3: i.type = "Follow-up Call"; break;
                case 4: i.type = "Lab Results Notification"; break;
                case 5: i.type = "Appointment Rescheduled"; break;
                case 6: i.type = "Patient Inquiry"; break;
                case 7: i.type = "Payment Reminder"; break;
                case 8: i.type = "Other"; break;
                case 9: 
                    std::cout << "Cancel logging...\n"; 
                    return;
            }
            pressEnter = true;
            redraw = true;
        }
    }

    std::cout << "Enter Note: ";
    std::getline(std::cin, i.note);

    std::cout << "Enter Interaction Date (YYYY-MM-DD): ";
    std::getline(std::cin, i.date);

    i.loggedAt = getCurrentTimestamp();
    i.id = getNextId(interactions, [](const Interaction& x) { return x.id; });

    interactions.push_back(i);

    saveInteractionLogs();

    std::cout << "Interaction logged successfully!\n";
    std::cout << "Press enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string getCurrentTimestamp() {
    auto end = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    std::string timestamp = std::ctime(&end_time);
    if (!timestamp.empty() && timestamp.back() == '\n') {
        timestamp.pop_back();
    }
    return timestamp;
}

std::string serializeInteractionRecord(const Interaction& i) {
    return std::to_string(i.id) + "|" + std::to_string(i.patientId) + "|" + i.type + "|" + i.note + "|" + i.date + "|" + i.loggedAt;
}

Interaction deserializeInteractionRecord(const std::string& line) {
    Interaction i;
    std::string id, patientId;

    std::stringstream ss(line);

    std::getline(ss, id, '|');
    std::getline(ss, patientId, '|');
    std::getline(ss, i.type, '|');
    std::getline(ss, i.note, '|');
    std::getline(ss, i.date, '|');
    std::getline(ss, i.loggedAt, '\n');

    i.id = std::stoi(id);
    i.patientId = std::stoi(patientId);

    return i;
}

void saveInteractionLogs() {
    saveRecords<Interaction>("data/interactions.csv", interactions, serializeInteractionRecord);
}

void loadInteractionLogs() {
    loadRecords<Interaction>("data/interactions.csv", interactions, deserializeInteractionRecord);
}

void viewInteractionLogs() {
    system("cls");

    std::cout << interactionLogsHeader << "\n";
              
    if (interactions.empty()) {
        std::cout << "No interaction logs found.\n";
        std::cout << "Press enter to continue...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    for (int idx = interactions.size() - 1; idx >= 0; idx--) {
        Interaction& i = interactions[idx];
        std::cout << "[" << i.id << "] " << i.date << " | "
                  << "Patient #" << i.patientId << " | "
                  << i.type << "\n";
        std::cout << "    Note: " << i.note << "\n";
        std::cout << "    Logged at: " << i.loggedAt << "\n\n";
    }

    std::cout << "Press enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

void viewLogsByPatient() {
    system("cls");

    std::cout << interactionLogsHeader << "\n";

    std::cout << "Enter Patient ID: ";
    int id;
    std::cin >> id;
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input!\n";
        std::cout << "Press enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }
    std::cin.ignore();

    std::string patientName = "";
    for (Patient& p : patients) {
        if (p.id == id) {
            patientName = p.name;
            break;
        }
    }

    if (patientName.empty()) {
        std::cout << "Patient ID not found.\n";
        std::cout << "Press enter to continue...";
        std::cin.get();
        return;
    }

    system("cls");

    std::cout << interactionLogsHeader << "\n";
    std::cout << "Patient: " << patientName << "\n";

    bool found = false;

    for (const Interaction& i : interactions) {
        if (i.patientId != id) {
            continue;
        }

        found = true;
        std::cout << "[" << i.id << "] " << i.type << "\n";
        std::cout << "    Date      : " << i.date << "\n";
        std::cout << "    Note      : " << i.note << "\n";
        std::cout << "    Logged at : " << i.loggedAt << "\n\n";
    }

    if (!found) {
        std::cout << "No interaction logs found for this patient.\n";
    }

    std::cout << "Press enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void deleteInteractionLog() {
    int id;
    bool found = false;

    system("cls");
    
    std::cout << deleteInteractionHeader << "\n";

    std::cout << "Enter Log ID to delete: ";
    std::cin >> id;
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input!\n";
        std::cout << "Press enter to continue...";
        std::cin.get();
        return;
    }
    std::cin.ignore();

    for (Interaction& i : interactions) {
        if (i.id != id) {
            continue;
        }

        found = true;
        system("cls");
        std::cout << interactionDeletedHeader << "\n";
        std::cout << "Log ID:      " << i.id           << "\n";
        std::cout << "Patient ID:  " << i.patientId    << "\n";
        std::cout << "Type:        " << i.type         << "\n";
        std::cout << "Note:        " << i.note         << "\n";
        std::cout << "Date:        " << i.date         << "\n";
        std::cout << "Logged at:   " << i.loggedAt     << "\n\n";
        break;
    }

    if (!found) {
        std::cout << "Log ID not found.\n";
        std::cout << "Press enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    char choice;
    bool valid = false;

    do {
        std::cout << "Are you sure you want to delete this interaction log? (Y/N)\n";
        std::cout << ">> ";
        std::cin >> choice;
        std::cin.ignore();
        if (choice == 'y' || choice == 'Y') {
            valid = true;
            deleteRecord(interactions, id);
            saveInteractionLogs();
            std::cout << "Interaction log deleted successfully.\n";
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

void viewMyInteractionLogs() {
    system("cls");
    std::cout << interactionLogsHeader << "\n";

    for (int idx = interactions.size() - 1; idx >= 0; idx--) {
        Interaction& i = interactions[idx];
        if (i.patientId != currentUser.linkedPatientId) continue;

        std::cout << "[" << i.id << "] " << i.type << "\n";
        std::cout << "    Date      : " << i.date      << "\n";
        std::cout << "    Note      : " << i.note      << "\n";
        std::cout << "    Logged at : " << i.loggedAt  << "\n\n";
    }

    std::cout << "Press enter to continue...";
    std::cin.ignore();
    std::cin.get();
}