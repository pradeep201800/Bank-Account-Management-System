#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Transaction {
    string dateTime;
    string type;
    double amount{};
    double balanceAfter{};
    string note;
};

struct Account {
    long long number{};
    string holder;
    string phone;
    string pin;
    string type;
    double balance{};
    double savingsGoal{};
    bool frozen{};
    vector<Transaction> history;
};

class BankSystem {
private:
    vector<Account> accounts;
    const string dataFile = "bank_data.txt";

    static string now() {
        auto current = chrono::system_clock::to_time_t(chrono::system_clock::now());
        tm localTime{};
#ifdef _WIN32
        localtime_s(&localTime, &current);
#else
        localtime_r(&current, &localTime);
#endif
        ostringstream out;
        out << put_time(&localTime, "%d-%m-%Y %H:%M:%S");
        return out.str();
    }

    static void clearInput() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    static double readAmount(const string& prompt) {
        double value;
        while (true) {
            cout << prompt;
            if (cin >> value && value > 0) {
                clearInput();
                return value;
            }
            cout << "Invalid amount. Enter a value greater than zero.\n";
            clearInput();
        }
    }

    Account* findAccount(long long number) {
        auto it = find_if(accounts.begin(), accounts.end(),
                          [number](const Account& a) { return a.number == number; });
        return it == accounts.end() ? nullptr : &(*it);
    }

    const Account* findAccount(long long number) const {
        auto it = find_if(accounts.begin(), accounts.end(),
                          [number](const Account& a) { return a.number == number; });
        return it == accounts.end() ? nullptr : &(*it);
    }

    Account* authenticate() {
        long long number;
        string enteredPin;
        cout << "Account number: ";
        if (!(cin >> number)) {
            clearInput();
            cout << "Invalid account number.\n";
            return nullptr;
        }
        cout << "4-digit PIN: ";
        cin >> enteredPin;
        clearInput();

        Account* account = findAccount(number);
        if (!account || account->pin != enteredPin) {
            cout << "Authentication failed.\n";
            return nullptr;
        }
        return account;
    }

    static void addTransaction(Account& account, const string& type,
                               double amount, const string& note) {
        account.history.push_back({now(), type, amount, account.balance, note});
    }

    long long generateAccountNumber() const {
        random_device rd;
        mt19937_64 generator(rd());
        uniform_int_distribution<long long> distribution(1000000000LL, 9999999999LL);
        long long number;
        do { number = distribution(generator); } while (findAccount(number));
        return number;
    }

    void save() const {
        ofstream file(dataFile, ios::trunc);
        if (!file) {
            cerr << "Warning: data could not be saved.\n";
            return;
        }
        file << setprecision(17) << accounts.size() << '\n';
        for (const auto& a : accounts) {
            file << a.number << ' ' << quoted(a.holder) << ' ' << quoted(a.phone) << ' '
                 << quoted(a.pin) << ' ' << quoted(a.type) << ' ' << a.balance << ' '
                 << a.savingsGoal << ' ' << a.frozen << ' ' << a.history.size() << '\n';
            for (const auto& t : a.history) {
                file << quoted(t.dateTime) << ' ' << quoted(t.type) << ' ' << t.amount << ' '
                     << t.balanceAfter << ' ' << quoted(t.note) << '\n';
            }
        }
    }

    void load() {
        ifstream file(dataFile);
        size_t accountCount;
        if (!(file >> accountCount)) return;

        vector<Account> loaded;
        for (size_t i = 0; i < accountCount; ++i) {
            Account a;
            size_t historyCount;
            if (!(file >> a.number >> quoted(a.holder) >> quoted(a.phone) >> quoted(a.pin)
                      >> quoted(a.type) >> a.balance >> a.savingsGoal >> a.frozen
                      >> historyCount)) {
                cerr << "Warning: saved data is damaged; starting with valid records only.\n";
                break;
            }
            for (size_t j = 0; j < historyCount; ++j) {
                Transaction t;
                if (!(file >> quoted(t.dateTime) >> quoted(t.type) >> t.amount
                          >> t.balanceAfter >> quoted(t.note))) break;
                a.history.push_back(t);
            }
            loaded.push_back(a);
        }
        accounts = move(loaded);
    }

    void createAccount() {
        Account a;
        cout << "\n--- CREATE ACCOUNT ---\nFull name: ";
        getline(cin, a.holder);
        cout << "Phone number: ";
        getline(cin, a.phone);
        do {
            cout << "Create a 4-digit PIN: ";
            getline(cin, a.pin);
            if (a.pin.size() != 4 || !all_of(a.pin.begin(), a.pin.end(), ::isdigit))
                cout << "PIN must contain exactly four digits.\n";
        } while (a.pin.size() != 4 || !all_of(a.pin.begin(), a.pin.end(), ::isdigit));

        int choice;
        cout << "Account type (1. Savings  2. Current): ";
        cin >> choice;
        clearInput();
        a.type = choice == 2 ? "Current" : "Savings";
        a.balance = readAmount("Opening deposit (minimum Rs. 500): Rs. ");
        if (a.balance < 500) {
            cout << "Opening deposit must be at least Rs. 500. Account not created.\n";
            return;
        }
        a.number = generateAccountNumber();
        addTransaction(a, "OPENING", a.balance, "Account created");
        accounts.push_back(a);
        save();
        cout << "Account created successfully. Your account number is " << a.number << ".\n";
    }

    void deposit() {
        cout << "\n--- DEPOSIT ---\n";
        Account* a = authenticate();
        if (!a) return;
        if (a->frozen) { cout << "Account is frozen. Contact the administrator.\n"; return; }
        double amount = readAmount("Deposit amount: Rs. ");
        a->balance += amount;
        addTransaction(*a, "CREDIT", amount, "Cash deposit");
        save();
        cout << "Deposit successful. New balance: Rs. " << fixed << setprecision(2)
             << a->balance << "\n";
    }

    void withdraw() {
        cout << "\n--- WITHDRAW ---\n";
        Account* a = authenticate();
        if (!a) return;
        if (a->frozen) { cout << "Account is frozen. Contact the administrator.\n"; return; }
        double amount = readAmount("Withdrawal amount: Rs. ");
        double minimum = a->type == "Savings" ? 500.0 : 0.0;
        if (a->balance - amount < minimum) {
            cout << "Declined: minimum balance of Rs. " << minimum << " must remain.\n";
            return;
        }
        if (amount >= 50000) {
            string answer;
            cout << "Security alert: large withdrawal. Type CONFIRM to continue: ";
            getline(cin, answer);
            if (answer != "CONFIRM") { cout << "Transaction cancelled.\n"; return; }
        }
        a->balance -= amount;
        addTransaction(*a, "DEBIT", amount, "Cash withdrawal");
        save();
        cout << "Withdrawal successful. Remaining balance: Rs. " << fixed
             << setprecision(2) << a->balance << "\n";
    }

    void transfer() {
        cout << "\n--- MONEY TRANSFER ---\nSender login:\n";
        Account* sender = authenticate();
        if (!sender) return;
        if (sender->frozen) { cout << "Sender account is frozen.\n"; return; }
        long long receiverNumber;
        cout << "Receiver account number: ";
        cin >> receiverNumber;
        clearInput();
        Account* receiver = findAccount(receiverNumber);
        if (!receiver || receiver == sender) {
            cout << "Invalid receiver account.\n";
            return;
        }
        if (receiver->frozen) { cout << "Receiver account is frozen.\n"; return; }
        double amount = readAmount("Transfer amount: Rs. ");
        double minimum = sender->type == "Savings" ? 500.0 : 0.0;
        if (sender->balance - amount < minimum) {
            cout << "Insufficient available balance.\n";
            return;
        }
        sender->balance -= amount;
        receiver->balance += amount;
        addTransaction(*sender, "TRANSFER OUT", amount,
                       "To account " + to_string(receiver->number));
        addTransaction(*receiver, "TRANSFER IN", amount,
                       "From account " + to_string(sender->number));
        save();
        cout << "Rs. " << fixed << setprecision(2) << amount
             << " transferred to " << receiver->holder << " successfully.\n";
    }

    void showAccount() {
        cout << "\n--- ACCOUNT DETAILS ---\n";
        Account* a = authenticate();
        if (!a) return;
        cout << "Name          : " << a->holder
             << "\nAccount No.   : " << a->number
             << "\nPhone         : " << a->phone
             << "\nType          : " << a->type
             << "\nBalance       : Rs. " << fixed << setprecision(2) << a->balance
             << "\nStatus        : " << (a->frozen ? "FROZEN" : "ACTIVE") << '\n';
        if (a->savingsGoal > 0) {
            double progress = min(100.0, a->balance * 100.0 / a->savingsGoal);
            cout << "Savings goal  : Rs. " << a->savingsGoal
                 << " (" << setprecision(1) << progress << "% reached)\n";
        }
    }

    void statement() {
        cout << "\n--- MINI STATEMENT ---\n";
        Account* a = authenticate();
        if (!a) return;
        cout << left << setw(20) << "DATE & TIME" << setw(15) << "TYPE"
             << right << setw(12) << "AMOUNT" << setw(14) << "BALANCE" << "  NOTE\n";
        cout << string(85, '-') << '\n';
        size_t start = a->history.size() > 10 ? a->history.size() - 10 : 0;
        for (size_t i = start; i < a->history.size(); ++i) {
            const auto& t = a->history[i];
            cout << left << setw(20) << t.dateTime << setw(15) << t.type
                 << right << setw(12) << fixed << setprecision(2) << t.amount
                 << setw(14) << t.balanceAfter << "  " << t.note << '\n';
        }
    }

    void setGoal() {
        cout << "\n--- SMART SAVINGS GOAL ---\n";
        Account* a = authenticate();
        if (!a) return;
        a->savingsGoal = readAmount("Target amount: Rs. ");
        save();
        double remaining = max(0.0, a->savingsGoal - a->balance);
        cout << "Goal saved. You need Rs. " << fixed << setprecision(2)
             << remaining << " more to reach it.\n";
    }

    void applyInterest() {
        cout << "\n--- APPLY SAVINGS INTEREST (ADMIN) ---\nAdmin password: ";
        string password;
        getline(cin, password);
        if (password != "BANK@123") { cout << "Access denied.\n"; return; }
        double rate = readAmount("Interest rate (%): ");
        int updated = 0;
        for (auto& a : accounts) {
            if (a.type == "Savings" && !a.frozen) {
                double interest = a.balance * rate / 100.0;
                a.balance += interest;
                addTransaction(a, "INTEREST", interest, to_string(rate) + "% credited");
                ++updated;
            }
        }
        save();
        cout << "Interest credited to " << updated << " savings account(s).\n";
    }

    void freezeToggle() {
        cout << "\n--- FREEZE / UNFREEZE (ADMIN) ---\nAdmin password: ";
        string password;
        getline(cin, password);
        if (password != "BANK@123") { cout << "Access denied.\n"; return; }
        long long number;
        cout << "Account number: ";
        cin >> number;
        clearInput();
        Account* a = findAccount(number);
        if (!a) { cout << "Account not found.\n"; return; }
        a->frozen = !a->frozen;
        addTransaction(*a, "STATUS", 0, a->frozen ? "Account frozen" : "Account unfrozen");
        save();
        cout << "Account is now " << (a->frozen ? "FROZEN" : "ACTIVE") << ".\n";
    }

public:
    BankSystem() { load(); }

    void run() {
        cout << fixed << setprecision(2);
        while (true) {
            cout << "\n========================================\n"
                 << "   NOVA BANK - ACCOUNT MANAGEMENT\n"
                 << "========================================\n"
                 << "1. Create account\n2. Deposit money\n3. Withdraw money\n"
                 << "4. Transfer money\n5. View account\n6. Mini statement\n"
                 << "7. Set savings goal\n8. Apply interest (Admin)\n"
                 << "9. Freeze/unfreeze account (Admin)\n0. Exit\n"
                 << "Choose an option: ";
            int choice;
            if (!(cin >> choice)) {
                clearInput();
                cout << "Please enter a valid menu number.\n";
                continue;
            }
            clearInput();
            switch (choice) {
                case 1: createAccount(); break;
                case 2: deposit(); break;
                case 3: withdraw(); break;
                case 4: transfer(); break;
                case 5: showAccount(); break;
                case 6: statement(); break;
                case 7: setGoal(); break;
                case 8: applyInterest(); break;
                case 9: freezeToggle(); break;
                case 0: save(); cout << "Thank you for using Nova Bank.\n"; return;
                default: cout << "Option not available.\n";
            }
        }
    }
};

int main() {
    BankSystem bank;
    bank.run();
    return 0;
}
