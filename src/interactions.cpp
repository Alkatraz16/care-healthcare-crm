#include "../include/interactions.h"
#include "../include/patients.h"
#include "../include/database.h"
#include "../include/auth.h"

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <limits>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <sstream>

std::vector<Interaction> interactions;

//helpers

// to validate (YYYY-MM-DD) format
static bool isValidDate(const std::string& date) {
    if (date.length() != 10)        return false;
    if (date[4] != '-' || date[7] != '-') return false;

    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!std::isdigit(date[i])) return false;
    }

    int year  = std::stoi(date.substr(0, 4));
    int month = std::stoi(date.substr(5, 2));
    int day   = std::stoi(date.substr(8, 2));

    if (year < 1900)        return false;
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31)     return false;

    return true;
}

// menu
void printInteractionsMenu() {
    system("cls");
    std::cout << "============================\n";
    std::cout << "      INTERACTION LOGS\n";
    std::cout << "============================\n";
    std::cout << "1. Add Interaction Log\n";
    std::cout << "2. View All Logs\n";
    std::cout << "3. View Logs per Patient\n";
    std::cout << "4. Delete Log\n";
    std::cout << "5. Back to Main Menu\n";
}

// add

void addInteractionRecord() {
    Interaction i;

    system("cls");
    std::cout << "==================================\n";
    std::cout << "         LOG INTERACTION\n";
    std::cout << "==================================\n";

    // patient ID
    while (true) {
        std::cout << "Enter Patient ID: ";
        if (!(std::cin >> i.patientId)) {
            std::cout << "Invalid input. Enter a number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        bool found = false;
        for (const Patient& p : patients) {
            if (p.id == i.patientId) { found = true; break; }
        }

        if (!found) {
            std::cout << "Patient ID " << i.patientId << " not found. Try again.\n";
            continue;
        }
        break;
    }

    // interaction type
    std::cout << "Select Interaction Type:\n";
    std::cout << "1. Clinic Visit\n"
              << "2. Appointment Reminder\n"
              << "3. Follow-up Call\n"
              << "4. Lab Results Notification\n"
              << "5. Appointment Rescheduled\n"
              << "6. Patient Inquiry\n"
              << "7. Payment Reminder\n"
              << "8. Other\n"
              << "9. Cancel\n";

    int choice;
    while (true) {
        std::cout << ">> ";
        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Enter a number (1-9).\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice < 1 || choice > 9) {
            std::cout << "Invalid choice. Enter a number (1-9).\n";
            continue;
        }
        break;
    }

    switch (choice) {
        case 1: i.type = "Clinic Visit";               break;
        case 2: i.type = "Appointment Reminder";        break;
        case 3: i.type = "Follow-up Call";              break;
        case 4: i.type = "Lab Results Notification";    break;
        case 5: i.type = "Appointment Rescheduled";     break;
        case 6: i.type = "Patient Inquiry";             break;
        case 7: i.type = "Payment Reminder";            break;
        case 8: i.type = "Other";                       break;
        case 9:
            std::cout << "Cancelled.\n";
            return;
    }

    while (true) {
        std::cout << "Enter Note: ";
        std::getline(std::cin, i.note);
        if (i.note.empty()) {
            std::cout << "Note cannot be empty.\n";
            continue;
        }
        break;
    }

    // date )YYYY-MM-DD) format
    while (true) {
        std::cout << "Enter Interaction Date (YYYY-MM-DD): ";
        std::getline(std::cin, i.date);
        if (!isValidDate(i.date)) {
            std::cout << "Invalid date format. Use YYYY-MM-DD.\n";
            continue;
        }
        break;
    }

    i.loggedAt = getCurrentTimestamp();
    i.id = getNextId(interactions, [](const Interaction& x){ return x.id; });

    interactions.push_back(i);
    saveInteractionLogs();

    std::cout << "\nInteraction logged successfully.\n";
    std::cout << "Press enter to continue...";
    std::cin.get();
}

// tiimetsamp

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::string timestamp = std::ctime(&now_time);
    if (!timestamp.empty() && timestamp.back() == '\n')
        timestamp.pop_back();
    return timestamp;
}

// serialization

std::string serializeInteractionRecord(const Interaction& i) {
    return std::to_string(i.id)        + "|" +
           std::to_string(i.patientId) + "|" +
           i.type                      + "|" +
           i.note                      + "|" +
           i.date                      + "|" +
           i.loggedAt;
}

Interaction deserializeInteractionRecord(const std::string& line) {
    Interaction i;
    std::string id, patientId;
    std::stringstream ss(line);

    try {
        std::getline(ss, id,        '|'); i.id        = std::stoi(id);
        std::getline(ss, patientId, '|'); i.patientId = std::stoi(patientId);
        std::getline(ss, i.type,    '|');
        std::getline(ss, i.note,    '|');
        std::getline(ss, i.date,    '|');
        std::getline(ss, i.loggedAt);
    } catch (...) {
        std::cerr << "Warning: Malformed interaction record skipped.\n";
        i.id = -1;  // sentinel
    }

    return i;
}

// persistence

void saveInteractionLogs() {
    saveRecords<Interaction>("data/interactions.csv", interactions, serializeInteractionRecord);
}

void loadInteractionLogs() {
    loadRecords<Interaction>("data/interactions.csv", interactions, deserializeInteractionRecord);

    // remove malformed records
    interactions.erase(
        std::remove_if(interactions.begin(), interactions.end(),
            [](const Interaction& i){ return i.id == -1; }),
        interactions.end()
    );
}

// view all

void viewInteractionLogs() {
    system("cls");
    std::cout << "================================================================\n";
    std::cout << "                     INTERACTION LOGS\n";
    std::cout << "================================================================\n";

    if (interactions.empty()) {
        std::cout << "No interaction logs found.\n";
        std::cout << "Press enter to continue...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    for (int idx = interactions.size() - 1; idx >= 0; idx--) {
        const Interaction& i = interactions[idx];
        std::cout << "[" << i.id << "] " << i.date << " | "
                  << "Patient #" << i.patientId << " | "
                  << i.type << "\n";
        std::cout << "    Note      : " << i.note      << "\n";
        std::cout << "    Logged at : " << i.loggedAt  << "\n\n";
    }

    std::cout << "Press enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

// view by patient

void viewLogsByPatient() {
    system("cls");
    std::cout << "================================================================\n";
    std::cout << "              INTERACTION LOGS PER PATIENT\n";
    std::cout << "================================================================\n";

    int id;
    while (true) {
        std::cout << "Enter Patient ID: ";
        if (!(std::cin >> id)) {
            std::cout << "Invalid input. Enter a number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        break;
    }

    std::string patientName = "";
    for (const Patient& p : patients) {
        if (p.id == id) { patientName = p.name; break; }
    }

    if (patientName.empty()) {
        std::cout << "Patient ID " << id << " not found.\n";
        std::cout << "Press enter to continue...";
        std::cin.get();
        return;
    }

    system("cls");
    std::cout << "================================================================\n";
    std::cout << "                    INTERACTION LOGS\n";
    std::cout << "Patient: " << patientName << "\n";
    std::cout << "================================================================\n\n";

    bool found = false;
    for (const Interaction& i : interactions) {
        if (i.patientId != id) continue;

        found = true;
        std::cout << "[" << i.id << "] " << i.type         << "\n";
        std::cout << "    Date      : " << i.date           << "\n";
        std::cout << "    Note      : " << i.note           << "\n";
        std::cout << "    Logged at : " << i.loggedAt       << "\n\n";
    }

    if (!found)
        std::cout << "No interaction logs found for this patient.\n";

    std::cout << "Press enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// delete

void deleteInteractionLog() {
    system("cls");
    std::cout << "====================================\n";
    std::cout << "       DELETE INTERACTION LOG\n";
    std::cout << "====================================\n";

    if (interactions.empty()) {
        std::cout << "No interaction logs found.\n";
        std::cout << "Press enter to continue...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    int id;
    while (true) {
        std::cout << "Enter Log ID to delete: ";
        if (!(std::cin >> id)) {
            std::cout << "Invalid input. Enter a number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        break;
    }

    bool found = false;
    for (const Interaction& i : interactions) {
        if (i.id != id) continue;

        found = true;
        system("cls");
        std::cout << "===========================================\n";
        std::cout << "      INTERACTION LOG TO BE DELETED\n";
        std::cout << "===========================================\n";
        std::cout << "Log ID:      " << i.id        << "\n";
        std::cout << "Patient ID:  " << i.patientId << "\n";
        std::cout << "Type:        " << i.type      << "\n";
        std::cout << "Note:        " << i.note      << "\n";
        std::cout << "Date:        " << i.date      << "\n";
        std::cout << "Logged at:   " << i.loggedAt  << "\n\n";
        break;
    }

    if (!found) {
        std::cout << "Log ID " << id << " not found.\n";
        std::cout << "Press enter to continue...";
        std::cin.get();
        return;
    }

    while (true) {
        std::cout << "Are you sure you want to delete this log? (Y/N): ";
        char confirm;
        std::cin >> confirm;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (confirm == 'y' || confirm == 'Y') {
            deleteRecord(interactions, id);
            saveInteractionLogs();
            std::cout << "Interaction log deleted successfully.\n";
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

// patient view

void viewMyInteractionLogs() {
    system("cls");
    std::cout << "=============================\n";
    std::cout << "    MY INTERACTION LOGS\n";
    std::cout << "=============================\n\n";

    if (currentUser.linkedPatientId <= 0) {
        std::cout << "No patient record linked to your account.\n";
        std::cout << "Press enter to continue...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    bool found = false;
    for (int idx = interactions.size() - 1; idx >= 0; idx--) {
        const Interaction& i = interactions[idx];
        if (i.patientId != currentUser.linkedPatientId) continue;

        found = true;
        std::cout << "[" << i.id << "] " << i.type    << "\n";
        std::cout << "    Date      : " << i.date      << "\n";
        std::cout << "    Note      : " << i.note      << "\n";
        std::cout << "    Logged at : " << i.loggedAt  << "\n\n";
    }

    if (!found) std::cout << "No interaction logs found.\n";

    std::cout << "Press enter to continue...";
    std::cin.ignore();
    std::cin.get();
}