#include <bits/stdc++.h>
using namespace std;

// Interface for storage operations
class Istorage {
protected:
    string fileName = "dataset.txt";

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
};

// Hasher class to hash passwords
class Hasher : public IHash {
public:
    Hasher(const string& pass, const string& sal) {
        password = pass;
        salt = sal;
    }

    string hashPassword() override {
        string alternated = alternate_characters(password, salt);
        string hashed = to_hex(alternated);
        return hashed;
    }

private:
    string alternate_characters(const string& password, const string& salt) {
        string result;
        size_t max_length = max(password.length(), salt.length());
        for (size_t i = 0; i < max_length; i++) {
            if (i < password.length()) {
                result += password[i];
            }
            if (i < salt.length()) {
                result += salt[i];
            }
        }
        result+='k';
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

// Interface for user service
class IuserService {
protected:
    FileManager fileManager;

public:
    virtual bool RegisterUser(const string& username, const string& password,const string&salt) = 0;
    virtual bool LoginUser(const string& username, const string& password) = 0;
};

// UserService class to handle user operations
class UserService : protected IuserService {
public:
    bool RegisterUser(const string& username, const string& password,const string& salt) override {
        Hasher hasher(password, salt);
        string hashedPassword = hasher.hashPassword();
        bool ok=fileManager.addUser(username, hashedPassword, salt);
        return ok;
    }

    bool LoginUser(const string& username, const string& password) override {
        auto [exists, storedHashedPassword, salt] = fileManager.getUserInfo(username);
    
        if (!exists) {
            cout << "Login Failed: Username does not exist.\n";
            return false;
        }
    
        Hasher hasher(password, salt);
        if (hasher.hashPassword() == storedHashedPassword) {
            cout << "Login Successful! Welcome, " << username << "!\n";
            return true;
        } else {
            cout << "Login Failed: Incorrect password.\n";
            return false;
        }
    }

};
bool test_function(const string& mode, const string& username, const string& pass, const string& salt, int testcases) {
    UserService user;

    bool result = false;
    if (mode == "login") {
        result = user.LoginUser(username, pass);
    } else {
        result = user.RegisterUser(username, pass, salt);
    }

    if (result) {
        cout << "Testcase " << testcases << " SUCCESSFUL" << endl;
    } else {
        cout << "Testcase " << testcases << " FAILED" << endl;
    }

    return result;
}

int main(){
    ifstream file("testcases.txt");
    if (!file.is_open()){
        cerr<<"Error while loading file"<<endl;
        return 1;
    }
    string line;
    int testcases=1;
    while(getline(file,line)){
        size_t comma1=line.find(',');
        size_t comma2=line.find(',',comma1+1);
        size_t comma3=line.find(',',comma2+1);
        size_t end=line.find(' ',comma3);
        string username=line.substr(0,comma1);
        string mode=line.substr(comma1+1,comma2-comma1-1);
        string pass=line.substr(comma2+1,comma3-comma2-1);
        string salt=line.substr(comma3+1,end);
        cout<<username<<'\t'<<mode<<'\t'<<pass<<'\t'<<salt<<endl;
        test_function(mode,username,pass,salt,testcases);
        testcases++;
    }
    
    return 0;
}
