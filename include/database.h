#ifndef DATABASE_H
#define DATABASE_H
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <functional>
#include <algorithm>
#include <iostream>

template <typename T>
void saveRecords(const std::string& filepath, const std::vector<T>& records, std::function<std::string(const T&)> serializer) {
    std::ofstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filepath << "\n";
        return;
    }

    for (const T& record : records) {
        file << serializer(record) << '\n';

        if (file.fail()) {
            std::cerr << "Error: Failed to write record to: " << filepath << "\n";
            break;
        }
    }

    file.close();
}

template <typename T>
void loadRecords(const std::string& filepath, std::vector<T>& records, std::function<T(const std::string&)> deserializer) {
    std::ifstream file(filepath);

    if (!file.is_open()) {
        // not an error — file may just not exist yet on first run
        return;
    }

    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        lineNumber++;

        if (line.empty()) continue;  // skip blank lines

        try {
            records.push_back(deserializer(line));
        } catch (...) {
            std::cerr << "Warning: Skipping malformed record at line "
                      << lineNumber << " in " << filepath << "\n";
        }
    }

    file.close();
}

template <typename T>
bool deleteRecord(std::vector<T>& records, int id) {
    auto it = std::remove_if(records.begin(), records.end(), [id](const T& record) {
        return record.id == id;
    });

    if (it == records.end()) {
        return false;
    }

    records.erase(it, records.end());
    return true;
}

template <typename T, typename F>
int getNextId(const std::vector<T>& records, F idSelector) {
    if (records.empty()) return 1;
    int maxId = 0;
    for (const T& record : records)
        maxId = std::max(maxId, idSelector(record));
    return maxId + 1;
}

#endif