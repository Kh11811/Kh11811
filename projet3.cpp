#include <bits/stdc++.h>
using namespace std;

// Interface for storage operations
class Istorage {
protected:
    string fileName = "testcase.txt";

public:
    virtual bool addUser(const string&, const string&, const string&) = 0;
    virtual vector<tuple<string, string, string>> getdata() = 0;
    virtual bool userExists(const string& Username) = 0;
    virtual tuple<bool, string, string> getUserInfo(const string& Username) = 0;
    virtual ~Istorage() {}
};

// FileManager class to handle file-based storage
class FileManager : public Istorage {
public:
    bool addUser(const string& Username, const string& HashedPassword, const string& salt) override {
        if (userExists(Username)) {
            cout << "Error: Username '" << Username << "' already exists. Choose another username.\n";
            return false;
        }
        ofstream outFile(fileName, ios::app);
        if (outFile.is_open()) {
            outFile << "\n"<<Username << "," << HashedPassword << "," << salt << endl;
            outFile.close();
            cout << "User " << Username << " added successfully!\n";
            return true;
        } else {
            cerr << "ERROR WHILE ADDING USER" << endl;
            return false;
        }
        return false;
    }

    vector<tuple<string, string, string>> getdata() override {
        vector<tuple<string, string, string>> users;
        ifstream inFile(fileName);

        if (inFile.is_open()) {
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
            inFile.close();
        } else {
            cerr << "Unable to get user DATA" << endl;
        }
        return users;
    }

    bool userExists(const string& Username) override {
        vector<tuple<string, string, string>> users = getdata();
        for (const auto& user : users) {
            if (get<0>(user) == Username) {
                return true;
            }
        }
        return false;
    }

    tuple<bool, string, string> getUserInfo(const string& Username) override {
        vector<tuple<string, string, string>> users = getdata();
        for (const auto& user : users) {
            if (get<0>(user) == Username) {
                // Return {true, hashedPassword, salt}
                return {true, get<1>(user), get<2>(user)};
            }
        }
        // User not found
        return {false, "", ""};
    }
};

// SaltGenerator class to generate random salts
class SaltGenerator {
    int length;
public:
    SaltGenerator(int l):length(l){}

    string generateSalt(int length = 8) {
        string salt;
        const char characters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        int charSize = sizeof(characters) - 1;
        srand(time(0));

        for (int i = 0; i < length; ++i) {
            salt += characters[rand() % charSize];
        }

        return salt;
    }
};

// Interface for hashing
class IHash {
protected:
    string password;
    string salt;

public:
    IHash() {}
    virtual string hashPassword() = 0;
    virtual ~IHash() = default;

    void setPassword(const string& pwd) {
        password = pwd;
    }

    void setSalt(const string& s) {
        salt = s;
    }
};

// FNV-1a hash implementation
class Hasher : public IHash {
private:
    uint32_t fnv1aHash(const string& input) {
        const uint32_t FNV_PRIME = 0x01000193; // 16777619
        const uint32_t FNV_OFFSET_BASIS = 0x811C9DC5; // 2166136261

        uint32_t hash = FNV_OFFSET_BASIS;
        for (char c : input) {
            hash ^= static_cast<uint8_t>(c);
            hash *= FNV_PRIME;
        }
        return hash;
    }

public:
    string hashPassword() override {
        string combined = salt + password; // Simple salting
        uint32_t hashVal = fnv1aHash(combined);

        // Convert hash to hex string
        stringstream ss;
        ss << hex << setw(8) << setfill('0') << hashVal;
        return ss.str();
    }
};

// Interface for validation
class IValidator {
public:
    virtual ~IValidator() = default;
    virtual bool validate(const string& input) const = 0;
};

// PasswordValidator class to validate passwords
class PasswordValidator : public IValidator {
public:
    bool validate(const string& password) const override {
        if (password.length() < 8) return false;

        bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
        string specialChars = "@#$%^&*()_+!~";

        for (char ch : password) {
            if (isupper(ch)) hasUpper = true;
            else if (islower(ch)) hasLower = true;
            else if (isdigit(ch)) hasDigit = true;
            else if (specialChars.find(ch) != string::npos) hasSpecial = true;
        }

        return hasUpper && hasLower && hasDigit && hasSpecial;
    }
};

// UserService class to handle user operations
class UserService {
protected:
    FileManager fileManager;

public:
    bool RegisterUser(const string& username, const string& password, const string& salt) {
        Hasher hasher;
        hasher.setSalt(salt);
        hasher.setPassword(password);
        string hashedPassword = hasher.hashPassword();
        cout<<hashedPassword<<endl;
        bool ok = fileManager.addUser(username, hashedPassword, salt);
        return ok;
    }

    bool LoginUser(const string& username, const string& password) {
        auto userInfo = fileManager.getUserInfo(username);
        bool exists = get<0>(userInfo);
        string storedHashedPassword = get<1>(userInfo);
        string salt = get<2>(userInfo);
        if (!exists) {
            cout << "Login Failed: Username does not exist.\n";
            return false;
        }
        Hasher hasher;
        hasher.setSalt(salt);
        hasher.setPassword(password);
        cout<<hasher.hashPassword()<<endl;
        if (hasher.hashPassword() == storedHashedPassword) {
            cout << "Login Successful! Welcome, " << username << "!\n";
            return true;
        } else {
            cout << "Login Failed: Incorrect password.\n";
            return false;
        }
    }
};

// Test function
bool test_function(const string& mode, const string& username, const string& password, const string& salt, const string& expected_result, const string& hashedPassword) {
    UserService user;
    bool result = false;
    if (mode == "login") {
        result = user.LoginUser(username, password);
    } else {
        result = user.RegisterUser(username, password, salt);
    }

    // Compare obtained result with expected result
    bool obtainedResult = (result ? "true" : "false") == expected_result;

    cout << username << ", " << mode << ", " << password << ", " << hashedPassword << ", " << expected_result << ", " << (obtainedResult ? "true" : "false") << endl;
    return obtainedResult;
}

int main(){
    ifstream file("testcases1.txt");
    if (!file.is_open()){
        cerr << "Error while loading file" << endl;
        return 1;
    }
    string line;
    int testcases = 1;
    while (getline(file, line)) {
        stringstream ss(line);
        string username, mode, password, hashedPassword, expected_result;
        string salt;

        getline(ss, username, ',');
        getline(ss, mode, ',');
        getline(ss, password, ',');
        getline(ss, salt, ',');
        getline(ss, hashedPassword, ',');
        getline(ss, expected_result, ',');
        cout << username << '\t' << mode << '\t' << password << '\t' << hashedPassword << '\t' << expected_result << '\t' << salt << endl;
        test_function(mode, username, password, salt, expected_result, hashedPassword);
        testcases++;
    }

    return 0;
}
