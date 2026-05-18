#include <ui.h>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <conio.h>

struct ReportTransaction {
	int id;
	int patientId;
	std::string date;
	float amount;
	std::string serviceType;
	std::string description;
};

namespace {

std::string trim(const std::string& s) {
	size_t start = 0;
	while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
		++start;
	}

	size_t end = s.size();
	while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
		--end;
	}

	return s.substr(start, end - start);
}

std::vector<std::string> splitLine(const std::string& line, char delimiter) {
	std::vector<std::string> tokens;
	std::stringstream ss(line);
	std::string token;

	while (std::getline(ss, token, delimiter)) {
		tokens.push_back(trim(token));
	}

	return tokens;
}

std::ifstream openExistingFile(const std::vector<std::string>& paths) {
	for (const std::string& path : paths) {
		std::ifstream file(path);
		if (file.is_open()) {
			return file;
		}
	}

	return std::ifstream();
}

std::unordered_map<int, std::string> loadPatientNamesById() {
	std::unordered_map<int, std::string> patientNames;

	std::ifstream file = openExistingFile({
		"data/patients.csv",
		"patients.csv",
		"src/patients.csv"
	});

	if (!file.is_open()) {
		return patientNames;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (trim(line).empty()) {
			continue;
		}

		char delimiter = (line.find('|') != std::string::npos) ? '|' : ',';
		std::vector<std::string> tokens = splitLine(line, delimiter);
		if (tokens.size() < 2) {
			continue;
		}

		try {
			int id = std::stoi(tokens[0]);
			patientNames[id] = tokens[1];
		} catch (...) {
			// Ignore malformed rows and continue loading valid data.
		}
	}

	return patientNames;
}

std::vector<ReportTransaction> loadTransactions() {
	std::vector<ReportTransaction> transactions;

	std::ifstream file = openExistingFile({
		"transactions.csv",
		"data/transactions.csv",
		"src/transactions.csv"
	});

	if (!file.is_open()) {
		return transactions;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (trim(line).empty()) {
			continue;
		}

		char delimiter = (line.find('|') != std::string::npos) ? '|' : ',';
		std::vector<std::string> tokens = splitLine(line, delimiter);
		if (tokens.size() < 6) {
			continue;
		}

		try {
			ReportTransaction t;
			t.id = std::stoi(tokens[0]);
			t.patientId = std::stoi(tokens[1]);
			t.date = tokens[2];
			t.amount = std::stof(tokens[3]);
			t.serviceType = tokens[4];
			t.description = tokens[5];
			transactions.push_back(t);
		} catch (...) {
			// Ignore malformed rows and continue loading valid data.
		}
	}

	return transactions;
}

void waitForEnter() {
	std::cout << "\nPress Enter to continue...";
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::cin.get();
}

void printReportHeader(const std::string& title) {
	std::cout << "\n====================================\n";
	std::cout << title << "\n";
	std::cout << "====================================\n";
}

void showTotalSales(const std::vector<ReportTransaction>& transactions) {
	float totalSales = 0.0f;
	for (const ReportTransaction& t : transactions) {
		totalSales += t.amount;
	}

	system("cls");
	printReportHeader("            TOTAL SALES");
	std::cout << "Total Sales: PHP " << std::fixed << std::setprecision(2) << totalSales << "\n";
}

void showTopPatientsBySpending(const std::vector<ReportTransaction>& transactions,
							   const std::unordered_map<int, std::string>& patientNames) {
	std::map<int, float> spendingByPatient;
	for (const ReportTransaction& t : transactions) {
		spendingByPatient[t.patientId] += t.amount;
	}

	std::vector<std::pair<int, float>> rankedPatients(spendingByPatient.begin(), spendingByPatient.end());
	std::sort(rankedPatients.begin(), rankedPatients.end(),
			  [](const std::pair<int, float>& a, const std::pair<int, float>& b) {
				  if (a.second == b.second) {
					  return a.first < b.first;
				  }
				  return a.second > b.second;
			  });

	system("cls");
	printReportHeader("      TOP PATIENTS BY SPENDING");

	if (rankedPatients.empty()) {
		std::cout << "No transactions found.\n";
		return;
	}

	int limit = static_cast<int>(std::min<size_t>(3, rankedPatients.size()));
	for (int i = 0; i < limit; ++i) {
		int patientId = rankedPatients[i].first;
		float amount = rankedPatients[i].second;

		auto it = patientNames.find(patientId);
		std::string patientLabel = (it != patientNames.end())
			? it->second
			: ("Patient ID " + std::to_string(patientId));

		std::cout << (i + 1) << ". " << patientLabel
				  << " - PHP " << std::fixed << std::setprecision(2) << amount << "\n";
	}
}

void showTransactionCount(const std::vector<ReportTransaction>& transactions) {
	system("cls");
	printReportHeader("    TOTAL NUMBER OF TRANSACTIONS");
	std::cout << "Total Transactions: " << transactions.size() << "\n";
}

}  // namespace

void reportsModule() {
    int choice;
    char key;
    int index = 0;
    bool pressEnter = false;
    bool redraw = true;

    while (true) {
        if (redraw) {
            system("cls");
            std::cout << reportsMenuHeader;
            std::cout << reportsMenuFrames[index] << "\n";
            redraw = false;
        }

        key = _getch();

        if (key == 'w') {
            index = (index == 0) ? 4 : index - 1;
            redraw = true;
        }
        if (key == 's') {
            index = (index + 1) % 4;
            redraw = true;
        }
        if (key == '\r') {
            pressEnter = true;
            choice = index + 1;

			std::vector<ReportTransaction> transactions = loadTransactions();
			std::unordered_map<int, std::string> patientNames = loadPatientNamesById();

			switch (choice) {
				case 1:
					showTotalSales(transactions);
					waitForEnter();
					break;
				case 2:
					showTopPatientsBySpending(transactions, patientNames);
					waitForEnter();
					break;
				case 3:
					showTransactionCount(transactions);
					waitForEnter();
					break;
				case 4:
					return;
					break;
			}
            redraw = true;
        }
    }
}