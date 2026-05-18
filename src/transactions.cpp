#include "../include/transactions.h"
#include "../include/database.h"
#include "../include/auth.h"
#include "../include/patients.h"
#include "../include/ui.h"

#include <iostream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <conio.h>

std::vector<Transaction> transactions;

static const std::string TRANSACTIONS_FILE = "data/transactions.csv";

//Serialization
std::string serializeTransaction(const Transaction& t) {
    return std::to_string(t.id)          + "|" +
           std::to_string(t.patientId)   + "|" +
           t.date                        + "|" +
           std::to_string(t.amount)      + "|" +
           t.serviceType                 + "|" +
           t.description;
}
//Deserialization
Transaction deserializeTransaction(const std::string& line) {
    Transaction t;
    std::stringstream ss(line);
    std::string token;

    std::getline(ss, token, '|'); t.id          = std::stoi(token);
    std::getline(ss, token, '|'); t.patientId   = std::stoi(token);
    std::getline(ss, token, '|'); t.date        = token;
    std::getline(ss, token, '|'); t.amount      = std::stof(token);
    std::getline(ss, token, '|'); t.serviceType = token;
    std::getline(ss, token, '|'); t.description = token;

    return t;
}

//Persitence
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
}

static std::string getPatientName(int patientId) {
    for (const Patient& p : patients) {
        if (p.id == patientId) {
            return p.name;
        }
    }
    return "Unknown";
}

static void printTransaction(const Transaction& t) {
    std::cout << "Assigned Transaction ID - " << t.id << "\n";
    std::cout << "Patient ID     : " << t.patientId   << "\n";
    std::cout << "Patient Name   : " << getPatientName(t.patientId) << "\n";
    std::cout << "Date           : " << t.date        << "\n";
    std::cout << "Amount         : " << std::fixed << std::setprecision(2) << t.amount << "\n";
    std::cout << "Service Type   : " << t.serviceType << "\n";
    std::cout << "Description    : " << t.description << "\n";
    std::cout << "-----------------------------\n";
}

void viewAllTransactionHistory() {
    if (transactions.empty()) {
        std::cout << "\nNo transactions found.\n";
        return;
    }

    system("cls");

    std::cout << transactionHistoryHeader << "\n";

    for (const Transaction& t : transactions) {
        printTransaction(t);
    }
    
    std::cout << "Press enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// VIEW BY PATIENT
void viewTransactionsByPatient() {
    if (transactions.empty()) {
        std::cout << "\nNo transactions found.\n";
        return;
    }

    std::cout << "\nEnter Patient ID: ";
    int patientId;
    while (!(std::cin >> patientId)) {
        std::cout << "Invalid input. Enter a number: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string name = getPatientName(patientId);
    if (name == "Unknown") {
        std::cout << "\nNo patient found with ID " << patientId << ".\n";
        return;
    }

    system("cls");

    std::cout << patientTransactionsHeader << "\n";

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
    
    std::cout << "Press enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// SEARCH
static std::string toLower(const std::string& value) {
    std::string lower = value;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower;
}

static bool containsIgnoreCase(const std::string& text, const std::string& query) {
    return toLower(text).find(toLower(query)) != std::string::npos;
}

void searchTransactions() {
    if (transactions.empty()) {
        std::cout << "\nNo transactions found.\n";
        std::cout << "Press enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // ADD THIS

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
    
    std::cout << "Press enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// MENU
void transactionManagement() {
    int choice;
    char key;
    int index = 0;
    bool pressEnter = false;
    bool redraw = true;

    while (true) {
        if (redraw) {
            system("cls");
            std::cout << transactionManagementModuleHeader;
            std::cout << transactionMenuFrames[index] << "\n";
            std::cout << "Logged in as: " << currentUser.username << "(" << currentUser.role << ")\n";
            redraw = false;
        }

        key = _getch();

        if (key == 'w') {
            index = (index == 0) ? 6 : index - 1;
            redraw = true;
        }
        if (key == 's') {
            index = (index + 1) % 7;
            redraw = true;
        }
        if (key == '\r') {
            pressEnter = true;
            choice = index + 1;
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
                    return;
                    break;
            }
            redraw = true;
        }
    }
}

// ADD TRANSACTION
void addTransaction(std::vector<Transaction>& transactions) {
    Transaction t;
    t.id = getNextId (transactions, [](const Transaction& tx) { return tx.id; }); 

    system("cls");

    std::cout << addTransactionHeader << "\n";
    std::cout << "Transaction ID: " << t.id << "\n";

    std::cout << "Enter Patient ID: ";
    while (!(std::cin >> t.patientId)) {
        std::cout << "Invalid input. Enter a number: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Enter Date (YYYY/MM/DD): ";
    std::getline(std::cin, t.date);

    std::cout << "Enter Amount: ";
    while (!(std::cin >> t.amount) || t.amount < 0) {
        std::cout << "Invalid input. Enter a non-negative number: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Enter Service Type: ";
    std::getline(std::cin, t.serviceType);

    std::cout << "Enter Description: ";
    std::getline(std::cin, t.description);

    transactions.push_back(t);
    std::cout << "\nTransaction added successfully.\n";
}

// VIEW TRANSACTIONS
void viewTransactions(const std::vector<Transaction>& transactions) {
    if (transactions.empty()) {
        std::cout << "\nNo transactions found.\n";
        return;
    }

    system("cls");

    std::cout << viewTransactionsHeader << "\n";

    for (const Transaction& t : transactions) {
        printTransaction(t);
    }

    std::cout << "\nEnter Patient ID: ";
    int patientId;
    while (!(std::cin >> patientId)) {
        std::cout << "Invalid input. Enter a number: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string name = getPatientName(patientId);
    if (name == "Unknown") {
        std::cout << "\nNo patient found with ID " << patientId << ".\n";
        return;
    }
}

// EDIT TRANSACTION

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

        system("cls");

        std::cout << "\n--- Current values ---\n";
        std::cout << "Patient ID   : " << t.patientId   << "\n";
        std::cout << "Date         : " << t.date        << "\n";
        std::cout << "Amount       : " << t.amount      << "\n";
        std::cout << "Service Type : " << t.serviceType << "\n";
        std::cout << "Description  : " << t.description << "\n";
        std::cout << "--- Enter new values (leave blank to keep current) ---\n";


        // Patient ID
        std::cout << "New Patient ID [" << t.patientId << "]: ";
        std::string input;
        std::getline(std::cin, input);
        if (!input.empty()) {
            try { t.patientId = std::stoi(input); }
            catch (...) { std::cout << "Invalid — keeping original.\n"; }
        }

        // Date
        std::cout << "New Date [" << t.date << "]: ";
        std::getline(std::cin, input);
        if (!input.empty()) t.date = input;

        // Amount
        std::cout << "New Amount [" << t.amount << "]: ";
        std::getline(std::cin, input);
        if (!input.empty()) {
            try {
                float val = std::stof(input);
                if (val >= 0) t.amount = val;
                else std::cout << "Negative amount — keeping original.\n";
            }
            catch (...) { std::cout << "Invalid — keeping original.\n"; }
        }

        // Service Type
        std::cout << "New Service Type [" << t.serviceType << "]: ";
        std::getline(std::cin, input);
        if (!input.empty()) t.serviceType = input;

        // Description
        std::cout << "New Description [" << t.description << "]: ";
        std::getline(std::cin, input);
        if (!input.empty()) t.description = input;

        std::cout << "\nTransaction updated successfully.\n";
        
        std::cout << "Press enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    std::cout << "\nTransaction ID " << id << " not found.\n";
}

// DELETE TRANSACTION
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

    if (confirm == 'y' || confirm == 'Y') {
        transactions.erase(it);
        std::cout << "\nTransaction deleted successfully.\n";
    } else {
        std::cout << "\nDeletion cancelled.\n";
    }
    
    std::cout << "Press enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void viewMyTransactions() {
    system("cls");
    std::cout << patientTransactionsHeader << "\n";

    bool found = false;
    for (Transaction& t : transactions) {
        if (t.patientId != currentUser.linkedPatientId) continue;

        found = true;
        std::cout << "[" << t.id << "] " << t.date << "\n";
        std::cout << "    Service : " << t.serviceType  << "\n";
        std::cout << "    Amount  : PHP " << std::fixed << std::setprecision(2) << t.amount << "\n";
        std::cout << "    Note    : " << t.description  << "\n\n";
    }

    if (!found) std::cout << "No transactions found.\n";

    std::cout << "Press enter to continue...";
    std::cin.ignore();
    std::cin.get();
}
