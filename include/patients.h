#ifndef PATIENTS_H
#define PATIENTS_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

struct Patient {
   int id;
   std::string name;
   std::string phone;
   std::string email;
   int age;
   std::string address;
};

extern std::vector<Patient> patients;

void printPatientManagementMenu();
void addPatientRecords();
void savePatientRecords();
void loadPatientRecords();
std::string serializePatientRecord(const Patient& p);
Patient deserializePatientRecord(const std::string& line);
void viewPatientRecords();
void updatePatientRecord();
void showUpdateMenu(Patient& p);
void updateName(Patient& p);
void updateEmail(Patient& p);
void updatePhone(Patient& p);
void updateAge(Patient& p);
void updateAddress(Patient& p);
void deletePatientRecord();
void searchPatientRecord();
void searchPatientById();
void searchPatientByName();
void viewMyRecords();
void printPatient(const Patient& p);

#endif