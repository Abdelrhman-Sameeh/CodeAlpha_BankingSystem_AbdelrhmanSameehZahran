#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib> // for rand()
#include <sstream> // for std::stringstream
#include <algorithm> // for std::find

using namespace std;

class Customer {
public:
    string Name;
    string CustomerId;
    string CustomerInfo;

    Customer(string name, string customerId, string customerInfo)
        : Name(name), CustomerId(customerId), CustomerInfo(customerInfo) {}

    Customer() {}

    Customer(const Customer& customer) {
        Name = customer.Name;
        CustomerId = customer.CustomerId;
        CustomerInfo = customer.CustomerInfo;
    }
};

class Transaction {
public:
    string TransactionId;
    string AccountId;
    string Type;
    double Amount;
    string Timestamp;

    Transaction(string transactionId, string accountId, string type, double amount)
        : TransactionId(transactionId), AccountId(accountId), Type(type), Amount(amount) {
        time_t now = time(0);
        Timestamp = ctime(&now);
        // Remove newline character from Timestamp
        Timestamp.erase(Timestamp.find_last_not_of("\n\r") + 1);
    }
};

class Account {
protected:
    string AccountId;
    double Balance;
    Customer Customer;
    vector<Transaction> transactions;

    void RecordTransaction(const string& type, double amount) {
        if (type.empty() || amount <= 0) {
            cout << "Invalid transaction details." << endl;
            return;
        }

        // Generate TransactionId (for demonstration, using a random number)
        stringstream ss;
        ss << rand();
        string transactionId = ss.str();

        transactions.push_back(Transaction(transactionId, AccountId, type, amount));
    }

public:
    Account(string accountId, double balance, const class Customer& customer)
        : AccountId(accountId), Balance(balance), Customer(customer) {}

    virtual void Deposit(double amount) {
        if (amount > 0) {
            Balance += amount;
            RecordTransaction("Deposit", amount);
            cout << "Deposited " << amount << " into account: " << AccountId << ". New balance: " << Balance << endl;
        } else {
            cout << "Deposit amount must be greater than zero." << endl;
        }
    }

    virtual bool Withdraw(double amount) {
        if (amount > 0 && Balance >= amount) {
            Balance -= amount;
            RecordTransaction("Withdrawal", amount);
            cout << "Withdrew " << amount << " from account " << AccountId << ". New balance: " << Balance << endl;
            return true;
        } else {
            cout << "Insufficient funds or invalid amount for withdrawal!" << endl;
            return false;
        }
    }

    virtual void Transfer(Account* destinationAccount, double amount) {
        if (amount <= 0) {
            cout << "Transfer amount must be greater than zero." << endl;
            return;
        }

        if (Balance < amount) {
            cout << "Insufficient funds for transfer." << endl;
            return;
        }

        if (destinationAccount == NULL) {
            cout << "Invalid destination account." << endl;
            return;
        }

        if (Withdraw(amount)) {
            destinationAccount->Deposit(amount);
            RecordTransaction("Transfer to " + destinationAccount->GetAccountId(), amount);
            cout << "Transferred " << amount << " from account " << AccountId << " to account " << destinationAccount->GetAccountId() << "." << endl;
        } else {
            cout << "Transfer failed." << endl;
        }
    }

    string GetAccountId() const {
        return AccountId;
    }

    virtual void ViewAccountInfo() const {
        cout << "Account ID: " << AccountId << ", Balance: $" << Balance << endl;
        cout << "Customer: " << Customer.Name << " (ID: " << Customer.CustomerId << "), Contact: " << Customer.CustomerInfo << endl;
        cout << "Recent Transactions:" << endl;
        for (vector<Transaction>::const_iterator it = transactions.begin(); it != transactions.end(); ++it) {
            const Transaction& transaction = *it;
            cout << transaction.Type << " of $" << transaction.Amount << " at " << transaction.Timestamp << endl;
        }
    }
};

class SavingsAccount : public Account {
public:
    double InterestRate;

    SavingsAccount(string accountId, double initialBalance, const class Customer& customer, double interestRate)
        : Account(accountId, initialBalance, customer), InterestRate(interestRate) {}

    void CalculateInterest() {
        double interest = Balance * (InterestRate / 100.0);
        Balance += interest;
        RecordTransaction("Interest", interest);
        cout << "Interest of " << interest << " earned for account " << AccountId << ". New balance: " << Balance << endl;
    }

    void ViewAccountInfo() const {
        cout << "Savings Account ID: " << AccountId << ", Balance: $" << Balance << ", Interest Rate: " << InterestRate << "%" << endl;
        cout << "Customer: " << Customer.Name << " (ID: " << Customer.CustomerId << ")" << endl;
        cout << "Recent Transactions:" << endl;
        for (vector<Transaction>::const_iterator it = transactions.begin(); it != transactions.end(); ++it) {
            const Transaction& transaction = *it;
            cout << transaction.Type << " of $" << transaction.Amount << " at " << transaction.Timestamp << endl;
        }
    }
};

class CheckingAccount : public Account {
public:
    double TransactionFee;

    CheckingAccount(string accountId, double initialBalance, const class Customer& customer, double transactionFee)
        : Account(accountId, initialBalance, customer), TransactionFee(transactionFee) {}

    bool Withdraw(double amount) {
        bool success = Account::Withdraw(amount);
        if (success) {
            DeductTransactionFee();
        }
        return success;
    }

    void DeductTransactionFee() {
        Balance -= TransactionFee;
        RecordTransaction("Transaction Fee", TransactionFee);
        cout << "Transaction fee of " << TransactionFee << " deducted for account " << AccountId << ". New balance: " << Balance << endl;
    }

    void ViewAccountInfo() const {
        cout << "Checking Account ID: " << AccountId << ", Balance: $" << Balance << ", Transaction Fee: $" << TransactionFee << endl;
        cout << "Customer: " << Customer.Name << " (ID: " << Customer.CustomerId << ")" << endl;
        cout << "Recent Transactions:" << endl;
        for (vector<Transaction>::const_iterator it = transactions.begin(); it != transactions.end(); ++it) {
            const Transaction& transaction = *it;
            cout << transaction.Type << " of $" << transaction.Amount << " at " << transaction.Timestamp << endl;
        }
    }
};

class BankService {
private:
    vector<Customer> customers;
    vector<Account*> accounts;
    vector<Transaction> transactions;

    void RecordTransaction(const string& accountId, const string& type, double amount) {
        // Generate TransactionId (for demonstration, using a random number)
        stringstream ss;
        ss << rand();
        string transactionId = ss.str();

        transactions.push_back(Transaction(transactionId, accountId, type, amount));
    }

    // Helper function to find account by ID
    Account* FindAccountById(const string& accountId) {
        for (vector<Account*>::iterator it = accounts.begin(); it != accounts.end(); ++it) {
            if ((*it)->GetAccountId() == accountId) {
                return *it;
            }
        }
        return NULL;
    }

public:
    void AddCustomer(const Customer& customer) {
        customers.push_back(customer);
        cout << "Customer added: " << customer.Name << endl;
    }

    void CreateSavingsAccount(const string& accountId, double initialBalance, const Customer& customer, double interestRate) {
        accounts.push_back(new SavingsAccount(accountId, initialBalance, customer, interestRate));
        cout << "Savings Account created: " << accountId << endl;
    }

    void CreateCheckingAccount(const string& accountId, double initialBalance, const Customer& customer, double transactionFee) {
        accounts.push_back(new CheckingAccount(accountId, initialBalance, customer, transactionFee));
        cout << "Checking Account created: " << accountId << endl;
    }

    void Deposit(const string& accountId, double amount) {
        Account* account = FindAccountById(accountId);
        if (account != NULL) {
            account->Deposit(amount);
            RecordTransaction(accountId, "Deposit", amount);
        } else {
            cout << "Account " << accountId << " not found." << endl;
        }
    }

    void Withdraw(const string& accountId, double amount) {
        Account* account = FindAccountById(accountId);
        if (account != NULL) {
            account->Withdraw(amount);
            RecordTransaction(accountId, "Withdrawal", amount);
        } else {
            cout << "Account " << accountId << " not found." << endl;
        }
    }

    void Transfer(const string& fromAccountId, const string& toAccountId, double amount) {
        Account* fromAccount = FindAccountById(fromAccountId);
        Account* toAccount = FindAccountById(toAccountId);
        if (fromAccount != NULL && toAccount != NULL) {
            fromAccount->Transfer(toAccount, amount);
            RecordTransaction(fromAccountId, "Transfer to " + toAccountId, amount);
        } else {
            cout << "One or both accounts not found." << endl;
        }
    }

    void DisplayRecentTransactions() const {
        cout << "Recent Bank Transactions:" << endl;
        for (vector<Transaction>::const_iterator it = transactions.begin(); it != transactions.end(); ++it) {
            const Transaction& transaction = *it;
            cout << transaction.Type << " of $" << transaction.Amount << " at " << transaction.Timestamp << endl;
        }
    }

    ~BankService() {
        // Clean up allocated memory for accounts
        for (vector<Account*>::iterator it = accounts.begin(); it != accounts.end(); ++it) {
            delete *it;
        }
        accounts.clear();
    }
};

int main() {
    srand(static_cast<unsigned int>(time(NULL))); // Initialize random seed

    BankService bankService;

    // Add customers
    Customer customer1("Alice", "C001", "alice@example.com");
    Customer customer2("Bob", "C002", "bob@example.com");
    bankService.AddCustomer(customer1);
    bankService.AddCustomer(customer2);

    // Create accounts
    bankService.CreateSavingsAccount("A001", 1000.0, customer1, 2.5);
    bankService.CreateCheckingAccount("A002", 500.0, customer2, 1.0);

    // Perform transactions
    bankService.Deposit("A001", 200.0);
    bankService.Withdraw("A002", 50.0);
    bankService.Transfer("A001", "A002", 100.0);

    // Display recent transactions
    bankService.DisplayRecentTransactions();

    return 0;
}



