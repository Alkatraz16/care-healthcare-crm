#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H

#include <string>
#include <vector>

struct Transaction {
    int id;
    int patientId;
    std::string date;
    float amount;
    std::string serviceType;
    std::string description;
};      

//menu
void transactionManagement();

//CRUD operations
void addTransaction (std::vector <Transaction>& transactions);
void viewTransactions (const std::vector <Transaction>& transactions);
void editTransaction (std::vector <Transaction>& transactions);
void deleteTransaction (std::vector <Transaction>& transactions);

// persistence
void saveTransactionRecords();
void loadTransactionRecords();

// history
void viewAllTransactionHistory();
void viewTransactionsByPatient();
void searchTransactions();

#endif