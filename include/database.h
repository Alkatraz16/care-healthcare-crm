#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <functional>
#include <algorithm>

template <typename T>
void saveRecords(const std::string& filepath, const std::vector<T>& records, std::function<std::string(const T&)> serializer) {
    std::ofstream file(filepath);
    for (const T& record : records) {
        file << serializer(record) << '\n';
    }
    file.close();
}

template <typename T>
void loadRecords(const std::string& filepath, std::vector<T>& records, std::function<T(const std::string&)> deserializer) {
    std::ifstream file(filepath);
    std::string line;
    while (std::getline(file, line)) {
        records.push_back(deserializer(line));
    }
    file.close();
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