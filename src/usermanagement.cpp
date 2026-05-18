#include "../include/transactions.h"
#include "../include/database.h"
#include "../include/auth.h"
#include "../include/patients.h"

#include <iostream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <sstream>

std::vector<Transaction> transactions;

static const std::string TRANSACTIONS_FILE = "transactions.csv";

//helpers

static std::string toLower(const std::string& value) {
    std::string lower = value;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower;
}

static bool containsIgnoreCase(const std::string& text, const std::string& query) {
    return toLower(text).find(toLower(query)) != std::string::npos;
}

static std::string getPatientName(int patientId) {
    for (const Patient& p : patients) {
        if (p.id == patientId) return p.name;
    }
    return "Unknown";
}

static bool patientExists(int patientId) {
    for (const Patient& p : patients) {
        if (p.id == patientId) return true;
    }
    return false;
}

// validates into MM/DD/YYYY format
static bool isValidDate(const std::string& date) {
    if (date.length() != 10) return false;
    if (date[2] != '/' || date[5] != '/') return false;

    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (!std::isdigit(date[i])) return false;
    }

    int month = std::stoi(date.substr(0, 2));
    int day   = std::stoi(date.substr(3, 2));
    int year  = std::stoi(date.substr(6, 4));

    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31)     return false;
    if (year < 1900)              return false;

    return true;
}

static void printTransaction(const Transaction& t) {
    std::cout << "Transaction ID : " << t.id                         << "\n";
    std::cout << "Patient ID     : " << t.patientId                  << "\n";
    std::cout << "Patient Name   : " << getPatientName(t.patientId)  << "\n";
    std::cout << "Date           : " << t.date                       << "\n";
    std::cout << "Amount         : PHP " << std::fixed
              << std::setprecision(2)   << t.amount                  << "\n";
    std::cout << "Service Type   : " << t.serviceType                << "\n";
    std::cout << "Description    : " << t.description                << "\n";
    std::cout << "-----------------------------\n";
}

//searialization

std::string serializeTransaction(const Transaction& t) {
    return std::to_string(t.id)        + "|" +
           std::to_string(t.patientId) + "|" +
           t.date                      + "|" +
           std::to_string(t.amount)    + "|" +
           t.serviceType               + "|" +
           t.description;
}

Transaction deserializeTransaction(const std::string& line) {
    Transaction t;
    std::stringstream ss(line);
    std::string token;

    try {
        std::getline(ss, token, '|'); t.id          = std::stoi(token);
        std::getline(ss, token, '|'); t.patientId   = std::stoi(token);
        std::getline(ss, token, '|'); t.date        = token;
        std::getline(ss, token, '|'); t.amount      = std::stof(token);
        std::getline(ss, token, '|'); t.serviceType = token;
        std::getline(ss, token, '|'); t.description = token;
    } catch (...) {
        std::cerr << "Warning: Skipping malformed transaction record.\n";
        t.id = -1; // sentinel value to signal bad record
    }

    return t;
}

//persistence

void saveTransactionRecords() {
    saveRecords<Transaction>(
        TRANSACTIONS_FILE,
        transactions,
        serializeTransaction
    );
}

void loadTransactionRecords() {
    transactions.clear();
    loadRecords<Transaction>(
        TRANSACTIONS_FILE,
        transactions,
        deserializeTransaction
    );

    //removes malformed records that came back with sentinel id
    transactions.erase(
        std::remove_if(transactions.begin(), transactions.end(),
            [](const Transaction& t) { return t.id == -1; }),
        transactions.end()
    );
}

//menu

void transactionManagement() {
    loadTransactionRecords();
    loadPatientRecords();

    int choice;
    do {
        std::cout << "=============================\n";
        std::cout << "    TRANSACTION MANAGEMENT\n";
        std::cout << "=============================\n";
        std::cout << "1. Add Transaction\n";
        std::cout << "2. View All Transactions\n";
        std::cout << "3. View Transactions per Patient\n";
        std::cout << "4. Search Transactions\n";
        std::cout << "5. Edit Transaction\n";
        std::cout << "6. Delete Transaction\n";
        std::cout << "7. Back\n";
        std::cout << "Enter choice: ";

        std::cin >> choice;
        while (std::cin.fail()) {
            std::cout << "Invalid input. Enter a number: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin >> choice;
        }

        switch (choice) {
            case 1:
                addTransaction(transactions);
                saveTransactionRecords();
                break;
            case 2:
                viewAllTransactionHistory();
                break;
            case 3:
                viewTransactionsByPatient();
                break;
            case 4:
                searchTransactions();
                break;
            case 5:
                editTransaction(transactions);
                saveTransactionRecords();
                break;
            case 6:
                deleteTransaction(transactions);
                saveTransactionRecords();
                break;
            case 7:
                std::cout << "Returning to main menu...\n";
                break;
            default:
                std::cout << "Invalid option! Try again.\n";
                break;
        }
    } while (choice != 7);
}

//add

void addTransaction(std::vector<Transaction>& transactions) {
    Transaction t;
    t.id = getNextId(transactions, [](const Transaction& tx){ return tx.id; });

    std::cout << "\n=============================\n";
    std::cout << "      ADD TRANSACTION\n";
    std::cout << "=============================\n";
    std::cout << "Transaction ID: " << t.id << "\n";

    // patient ID
    while (true) {
        std::cout << "Enter Patient ID: ";
        if (!(std::cin >> t.patientId)) {
            std::cout << "Invalid input. Enter a number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (!patientExists(t.patientId)) {
            std::cout << "No patient found with ID " << t.patientId
                      << ". Try again.\n";
            continue;
        }
        break;
    }

    // date (MM/DD/YYYY) format
    while (true) {
        std::cout << "Enter Date (MM/DD/YYYY): ";
        std::getline(std::cin, t.date);
        if (!isValidDate(t.date)) {
            std::cout << "Invalid date format. Use MM/DD/YYYY.\n";
            continue;
        }
        break;
    }

    // amount (must not be negative)
    std::cout << "Enter Amount: ";
    while (!(std::cin >> t.amount) || t.amount < 0) {
        std::cout << "Invalid input. Enter a non-negative number: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // service type (must not be empty)
    while (true) {
        std::cout << "Enter Service Type: ";
        std::getline(std::cin, t.serviceType);
        if (t.serviceType.empty()) {
            std::cout << "Service type cannot be empty.\n";
            continue;
        }
        break;
    }

    // description
    std::cout << "Enter Description (optional): ";
    std::getline(std::cin, t.description);

    transactions.push_back(t);
    std::cout << "\nTransaction added successfully.\n";
}

// view all

void viewAllTransactionHistory() {
    if (transactions.empty()) {
        std::cout << "\nNo transactions found.\n";
        return;
    }

    std::cout << "\n=============================\n";
    std::cout << "    ALL TRANSACTION HISTORY\n";
    std::cout << "=============================\n";

    for (const Transaction& t : transactions)
        printTransaction(t);
}

// view transactions

void viewTransactions(const std::vector<Transaction>& transactions) {
    if (transactions.empty()) {
        std::cout << "\nNo transactions found.\n";
        return;
    }

    std::cout << "\n=============================\n";
    std::cout << "      ALL TRANSACTIONS\n";
    std::cout << "=============================\n";

    for (const Transaction& t : transactions)
        printTransaction(t);
}

// view by patient

void viewTransactionsByPatient() {
    if (transactions.empty()) {
        std::cout << "\nNo transactions found.\n";
        return;
    }

    int patientId;
    while (true) {
        std::cout << "\nEnter Patient ID: ";
        if (!(std::cin >> patientId)) {
            std::cout << "Invalid input. Enter a number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (!patientExists(patientId)) {
            std::cout << "No patient found with ID " << patientId << ".\n";
            return;
        }
        break;
    }

    std::string name = getPatientName(patientId);

    std::cout << "\n=============================\n";
    std::cout << "  TRANSACTIONS FOR " << name << "\n";
    std::cout << "=============================\n";

    bool found = false;
    float total = 0;

    for (const Transaction& t : transactions) {
        if (t.patientId == patientId) {
            printTransaction(t);
            total += t.amount;
            found = true;
        }
    }

    if (!found)
        std::cout << "No transactions found for this patient.\n";
    else
        std::cout << "Total Spent: PHP " << std::fixed
                  << std::setprecision(2) << total << "\n";
}

// search

void searchTransactions() {
    if (transactions.empty()) {
        std::cout << "\nNo transactions found.\n";
        return;
    }

    std::cout << "\nEnter search term (service type, description, date, patient name): ";
    std::string query;
    std::getline(std::cin, query);

    if (query.empty()) {
        std::cout << "No search term entered.\n";
        return;
    }

    std::cout << "\nSearch results for '" << query << "':\n";
    std::cout << "=============================\n";

    bool found = false;
    for (const Transaction& t : transactions) {
        if (containsIgnoreCase(t.date,                      query) ||
            containsIgnoreCase(t.serviceType,               query) ||
            containsIgnoreCase(t.description,               query) ||
            containsIgnoreCase(getPatientName(t.patientId), query)) {
            printTransaction(t);
            found = true;
        }
    }

    if (!found)
        std::cout << "No matching transactions found.\n";
}

// edit

void editTransaction(std::vector<Transaction>& transactions) {
    if (transactions.empty()) {
        std::cout << "\nNo transactions to edit.\n";
        return;
    }

    std::cout << "\nEnter Transaction ID to edit: ";
    int id;
    while (!(std::cin >> id)) {
        std::cout << "Invalid input. Enter a number: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    for (Transaction& t : transactions) {
        if (t.id != id) continue;

        std::cout << "\n--- Current values ---\n";
        std::cout << "Patient ID   : " << t.patientId   << "\n";
        std::cout << "Date         : " << t.date        << "\n";
        std::cout << "Amount       : " << t.amount      << "\n";
        std::cout << "Service Type : " << t.serviceType << "\n";
        std::cout << "Description  : " << t.description << "\n";
        std::cout << "--- Enter new values (leave blank to keep current) ---\n";

        std::string input;

        // patient ID
        std::cout << "New Patient ID [" << t.patientId << "]: ";
        std::getline(std::cin, input);
        if (!input.empty()) {
            try {
                int newId = std::stoi(input);
                if (!patientExists(newId))
                    std::cout << "Patient ID " << newId
                              << " not found — keeping original.\n";
                else
                    t.patientId = newId;
            }
            catch (...) { std::cout << "Invalid — keeping original.\n"; }
        }

        // date
        std::cout << "New Date [" << t.date << "]: ";
        std::getline(std::cin, input);
        if (!input.empty()) {
            if (!isValidDate(input))
                std::cout << "Invalid date format — keeping original.\n";
            else
                t.date = input;
        }

        // amount
        std::cout << "New Amount [" << t.amount << "]: ";
        std::getline(std::cin, input);
        if (!input.empty()) {
            try {
                float val = std::stof(input);
                if (val < 0) std::cout << "Negative amount — keeping original.\n";
                else t.amount = val;
            }
            catch (...) { std::cout << "Invalid — keeping original.\n"; }
        }

        // service type
        std::cout << "New Service Type [" << t.serviceType << "]: ";
        std::getline(std::cin, input);
        if (!input.empty()) t.serviceType = input;

        // description
        std::cout << "New Description [" << t.description << "]: ";
        std::getline(std::cin, input);
        if (!input.empty()) t.description = input;

        std::cout << "\nTransaction updated successfully.\n";
        return;
    }

    std::cout << "\nTransaction ID " << id << " not found.\n";
}

// delete

void deleteTransaction(std::vector<Transaction>& transactions) {
    if (transactions.empty()) {
        std::cout << "\nNo transactions to delete.\n";
        return;
    }

    std::cout << "\nEnter Transaction ID to delete: ";
    int id;
    while (!(std::cin >> id)) {
        std::cout << "Invalid input. Enter a number: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    auto it = std::find_if(
        transactions.begin(), transactions.end(),
        [id](const Transaction& t) { return t.id == id; }
    );

    if (it == transactions.end()) {
        std::cout << "\nTransaction ID " << id << " not found.\n";
        return;
    }

    std::cout << "Delete transaction ID " << id << "? (y/n): ";
    char confirm;
    std::cin >> confirm;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (confirm != 'y' && confirm != 'Y' &&
        confirm != 'n' && confirm != 'N') {
        std::cout << "Invalid input. Deletion cancelled.\n";
        return;
    }

    if (confirm == 'y' || confirm == 'Y') {
        transactions.erase(it);
        std::cout << "\nTransaction deleted successfully.\n";
    } else {
        std::cout << "\nDeletion cancelled.\n";
    }
}

// patient view

void viewMyTransactions() {
    system("cls");
    std::cout << "=============================\n";
    std::cout << "      MY TRANSACTIONS\n";
    std::cout << "=============================\n\n";

    if (currentUser.linkedPatientId <= 0) {
        std::cout << "No patient record linked to your account.\n";
        std::cout << "Press enter to continue...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    bool found = false;
    for (const Transaction& t : transactions) {
        if (t.patientId != currentUser.linkedPatientId) continue;
        found = true;
        std::cout << "[" << t.id << "] " << t.date                          << "\n";
        std::cout << "    Service : " << t.serviceType                       << "\n";
        std::cout << "    Amount  : PHP " << std::fixed
                  << std::setprecision(2) << t.amount                        << "\n";
        std::cout << "    Note    : " << t.description                       << "\n\n";
    }

    if (!found) std::cout << "No transactions found.\n";

    std::cout << "Press enter to continue...";
    std::cin.ignore();
    std::cin.get();
}