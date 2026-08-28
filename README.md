# Bank-Account-Management-System
A feature-rich Bank Account Management System built with C++17. Create savings or current accounts, use PIN authentication, deposit, withdraw and transfer money, view mini statements, set savings goals, apply interest, freeze accounts and save all records using file handling. An educational project demonstrating OOP, STL and input validation.

============================= Nova Bank – Bank Account Management System ==========================

A console-based Bank Account Management System developed in C++17. It simulates essential banking operations while demonstrating object-oriented programming, file handling, input validation, data structures, and transaction management.

Features

Create Savings and Current accounts

Unique 10-digit account number generation

Four-digit PIN authentication

Deposit and withdraw money

Transfer money between accounts

Check account details and current balance

View the 10 most recent transactions

Set and monitor a personal savings goal

Large-withdrawal confirmation for added protection

Apply interest to all active savings accounts

Freeze or unfreeze accounts through the admin menu

Save accounts and transaction history automatically

Validate menu choices, PINs, account numbers, and amounts

Unique Additions

Smart Savings Goal

Users can set a target amount and view how much of the goal has been completed.

Suspicious Withdrawal Protection

Withdrawals of Rs. 50,000 or more require the user to type CONFIRM before processing.

Dated Transaction History

Every deposit, withdrawal, transfer, interest credit, and account status change is recorded with its date, time, amount, resulting balance, and a short note.

Technologies Used

C++17

Standard Template Library (STL)

Object-oriented programming

File handling

Vectors and structures

Random number generation

Date and time utilities

Project Files

BankAccountManagementSystem.cpp  Main source code
bank_data.txt                    Generated automatically after first use
README.md                        Project documentation

Compile and Run

Windows with MinGW

g++ -std=c++17 BankAccountManagementSystem.cpp -o bank_system.exe
bank_system.exe

Linux or macOS

g++ -std=c++17 BankAccountManagementSystem.cpp -o bank_system
./bank_system

Menu Options

1. Create account
2. Deposit money
3. Withdraw money
4. Transfer money
5. View account
6. Mini statement
7. Set savings goal
8. Apply interest (Admin)
9. Freeze/unfreeze account (Admin)
0. Exit

Admin Access

The demonstration admin password is:

BANK@123

Important: This password is stored directly in the source code for educational purposes. A real banking application should store salted password hashes, use a proper database, encrypt sensitive information, and implement role-based authentication.

Data Storage

The application creates bank_data.txt in the current working directory. Account details and transactions are loaded automatically when the program starts and saved after every successful change.

Do not manually edit bank_data.txt, as an invalid format may prevent records from loading correctly.

Concepts Demonstrated

Classes, structures, and member functions

Encapsulation of banking operations

Searching and updating vector records

Reading and writing structured text files

Authentication and account-status checks

Formatted console output

Exception-resistant input validation

Future Improvements

Hash and salt customer PINs

Replace text storage with SQLite or MySQL

Add separate customer and administrator logins

Export statements to CSV

Add loan and fixed-deposit modules

Create a graphical or web-based interface

Add unit tests and audit logging

---------------------------------------------------- Disclaimer --------------------------------------------------------------------------

This project is an educational banking simulation. It must not be used to store real financial or personal information.

Author

Developed by Pradeep Kumar.
