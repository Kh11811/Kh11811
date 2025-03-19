#include <iostream>
#include <fstream> // For file operations
#include <iomanip> // For std::hex, std::setw, std::setfill
#include <sstream> // For std::stringstream
using namespace std;

// Interface for hashing (if needed)
class IHash {
public:
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
    string password;
    string salt;

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

int main() {
    // Open a file for writing
    ofstream outFile("testcases.txt");

    // Check if the file was opened successfully
    if (!outFile.is_open()) {
        cerr << "Error: Unable to open file for writing." << endl;
        return 1; // Exit the program with an error code
    }

    // Create Hasher objects and hash passwords
    Hasher hash1("Khis onit@@", "98q7Hp3m");
    Hasher hash2("John Doe@@", "abc123");

    // Write hashed passwords to the file and console
    outFile << hash1.hashPassword() << endl;
    cout << hash1.hashPassword() << endl;

    outFile << hash2.hashPassword() << endl;
    cout << hash2.hashPassword() << endl;

    // Close the file
    outFile.close();

    return 0;
}
