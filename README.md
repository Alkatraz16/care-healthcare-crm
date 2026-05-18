# care-healthcare-crm
A Healthcare Customer Relationship Management (CRM) system designed to help small clinics or private practices manage patient records, track service interactions, and maintain transaction histories.

# HOW TO RUN
g++ src/main.cpp src/patients.cpp src/interactions.cpp src/database.cpp src/transactions.cpp src/reports.cpp src/auth.cpp src/usermanagement.cpp -Iinclude -o CARE
./CARE

# SEED ACCOUNTS
Admin   - username: admin   | password: admin123
Staff   - username: staff   | password: staff123
Patient - username: patient | password: patient123