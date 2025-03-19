#include <bits/stdc++.h>
#include <filesystem> // For better file handling
#include <random>     // For secure random number generation
using namespace std;
namespace fs = std::filesystem;

// Interface for storage operations
class Istorage {
protected:
    string fileName;

public:
    Istorage(const string& file) : fileName(file) {}
    virtual bool addUser(const string&, const string&, const string&) = 0;
    virtual vector<tuple<string, string, string>> getdata() = 0;
    virtual bool userExists(const string& Username) = 0;
    virtual tuple<bool, string, string> getUserInfo(const string& Username) = 0;
    virtual ~Istorage() {}
};

// FileManager class to handle file-based storage
class FileManager : public Istorage {
public:
    FileManager(const string& file) : Istorage(file) {}

    bool addUser(const string& Username, const string& HashedPassword, const string& salt) override {
        if (userExists(Username)) {
            cout << "Error: Username '" << Username << "' already exists. Choose another username.\n";
            return false;
        }

        ofstream outFile(fileName, ios::app);
        if (!outFile) {
            cerr << "ERROR: Unable to open file for writing.\n";
            return false;
        }

        outFile << Username << "," << HashedPassword << "," << salt << endl;
        cout << "User " << Username << " added successfully!\n";
        return true;
    }

    vector<tuple<string, string, string>> getdata() override {
        vector<tuple<string, string, string>> users;
        ifstream inFile(fileName);

        if (!inFile) {
            cerr << "ERROR: Unable to open file for reading.\n";
            return users;
        }

        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            string Username, HashedPassword, salt;

            if (getline(ss, Username, ',') &&
                getline(ss, HashedPassword, ',') &&
                getline(ss, salt, ',')) {
                users.emplace_back(Username, HashedPassword, salt);
            }
        }

        return users;
    }

    bool userExists(const string& Username) override {
        auto users = getdata();
        for (const auto& user : users) {
            if (get<0>(user) == Username) {
                return true;
            }
        }
        return false;
    }

    tuple<bool, string, string> getUserInfo(const string& Username) override {
        auto users = getdata();
        for (const auto& user : users) {
            if (get<0>(user) == Username) {
                return {true, get<1>(user), get<2>(user)};
            }
        }
        return {false, "", ""};
    }
};

// SaltGenerator class to generate random salts
class SaltGenerator {
    mt19937 rng; // Mersenne Twister random number generator

public:
    SaltGenerator() : rng(random_device{}()) {}

    string generateSalt(int length = 8) {
        const string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        uniform_int_distribution<size_t> dist(0, characters.size() - 1);

        string salt;
        for (int i = 0; i < length; ++i) {
            salt += characters[dist(rng)];
        }

        return salt;
    }
};

// Interface for hashing
class IHash {
public:
    virtual string hashPassword(const string& password, const string& salt) = 0;
    virtual ~IHash() = default;
};

// Hasher class to hash passwords
class Hasher : public IHash {
public:
    string hashPassword(const string& password, const string& salt) override {
        string alternated = alternate_characters(password, salt);
        return to_hex(alternated);
    }

private:
    string alternate_characters(const string& password, const string& salt) {
        string result;
        size_t max_length = max(password.length(), salt.length());
        for (size_t i = 0; i < max_length; i++) {
            if (i < password.length()) result += password[i];
            if (i < salt.length()) result += salt[i];
        }
        result += 'k'; // Add a constant for additional complexity
        return result;
    }

    string to_hex(const string& input) {
        stringstream ss;
        for (char c : input) {
            ss << hex << setw(2) << setfill('0') << (int)c;
        }
        return ss.str();
    }
};

// Interface for validation
class IValidator {
public:
    virtual bool validate(const string& input) const = 0;
    virtual ~IValidator() = default;
};

// PasswordValidator class to validate passwords
class PasswordValidator : public IValidator {
public:
    bool validate(const string& password) const override {
        if (password.length() < 8) return false;

        bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
        const string specialChars = "@#$%^&*()_+!~";

        for (char ch : password) {
            if (isupper(ch)) hasUpper = true;
            else if (islower(ch)) hasLower = true;
            else if (isdigit(ch)) hasDigit = true;
            else if (specialChars.find(ch) != string::npos) hasSpecial = true;
        }

        return hasUpper && hasLower && hasDigit && hasSpecial;
    }
};

// Interface for user service
class IuserService {
protected:
    FileManager fileManager;

public:
    IuserService(const string& file) : fileManager(file) {}
    virtual bool RegisterUser(const string& username, const string& password, const string& salt) = 0;
    virtual bool LoginUser(const string& username, const string& password) = 0;
};

// UserService class to handle user operations
class UserService : public IuserService {
    Hasher hasher;
    PasswordValidator validator;

public:
    UserService(const string& file) : IuserService(file) {}

    bool RegisterUser(const string& username, const string& password, const string& salt) override {
        if (!validator.validate(password)) {
            cout << "Error: Password does not meet requirements.\n";
            return false;
        }

        string hashedPassword = hasher.hashPassword(password, salt);
        return fileManager.addUser(username, hashedPassword, salt);
    }

    bool LoginUser(const string& username, const string& password) override {
        auto userInfo = fileManager.getUserInfo(username);
        bool exists = get<0>(userInfo);
        string storedHashedPassword = get<1>(userInfo);
        string salt = get<2>(userInfo);

        if (!exists) {
            cout << "Login Failed: Username does not exist.\n";
            return false;
        }

        string enteredHashedPassword = hasher.hashPassword(password, salt);
        if (enteredHashedPassword == storedHashedPassword) {
            cout << "Login Successful! Welcome, " << username << "!\n";
            return true;
        } else {
            cout << "Login Failed: Incorrect password.\n";
            return false;
        }
    }
};

// Test function
bool test_function(const string& mode, const string& username, const string& pass, const string& salt, int testcases) {
    UserService userService("dataset.txt");

    if (mode == "login") {
        bool login_bool = userService.LoginUser(username, pass);
        cout << "Testcase " << testcases << (login_bool ? " SUCCESSFUL" : " FAILED") << endl;
        return login_bool;
    } else {
        bool register_bool = userService.RegisterUser(username, pass, salt);
        cout << "Testcase " << testcases << (register_bool ? " SUCCESSFUL" : " FAILED") << endl;
        return register_bool;
    }
}

int main() {
    ifstream file("database.txt");
    if (!file) {
        cerr << "Error: Unable to open database file.\n";
        return 1;
    }

    string line;
    int testcases = 1;
    while (getline(file, line)) {
        stringstream ss(line);
        string username, mode, pass, salt;

        getline(ss, username, ',');
        getline(ss, mode, ',');
        getline(ss, pass, ',');
        getline(ss, salt, ',');

        test_function(mode, username, pass, salt, testcases);
        testcases++;
    }

    return 0;
}
