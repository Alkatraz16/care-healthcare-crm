#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
using namespace std;

ifstream file("transactions.csv");
string line;
int main() {	
	cout << "- Transaction History -" << "\n";
	cout << "\n";
	
    if (!file.is_open()) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    while (getline(file, line)) {
        stringstream ss(line);
        string transact, date, amount;

        getline(ss, transact, ',');
    	getline(ss, date, ',');
    	getline(ss, amount, ',');

        cout << transact << " - " << date << " - PHP " << amount << endl; //note: you can change it to the peso sign
    }

    file.close();
    return 0;
}

