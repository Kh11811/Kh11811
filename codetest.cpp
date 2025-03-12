#include <bits/stdc++.h>

using namespace std;

// Your existing code (Istorage, FileManager, SaltGenerator, IHash, Hasher, IValidator, PasswordValidator, IuserService, UserService) goes here...

// Add this test function
void test() {
    // Test FileManager
    FileManager fm;
    fm.addUser("testUser", "hashedPassword", "salt");
    assert(fm.userExists("testUser") == true);
    assert(fm.userExists("nonExistentUser") == false);

    auto [exists, hashedPassword, salt] = fm.getUserInfo("testUser");
    assert(exists == true);
    assert(hashedPassword == "hashedPassword");
    assert(salt == "salt");

    // Test Hasher
    Hasher hasher("password", "salt");
    string hashed = hasher.hashPassword();
    assert(!hashed.empty());

    // Test PasswordValidator
    PasswordValidator validator;
    assert(validator.validate("ValidPass1!") == true);
    assert(validator.validate("invalid") == false);

    // Test UserService
    UserService userService;
    userService.RegisterUser("testUser2", "ValidPass1!");
    assert(userService.LoginUser("testUser2", "ValidPass1!") == true);
    assert(userService.LoginUser("testUser2", "WrongPass1!") == false);

    cout << "All tests passed!" << endl;
}

int main(int argc, char* argv[]) {
    if (argc > 1 && string(argv[1]) == "--test") {
        test();
        return 0;
    }

    UserService userService;
    FileManager fileManager;

    int choice;
    string username, password;

    while (true) {
        cout << "\n1. Register\n2. Login\n3. Exit\nChoose an option: ";
        cin >> choice;
        if (!(cin >> choice)) {
            cout << "Invalid input! Please enter a number.\n";
            cin.clear(); // Clear error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
            continue;
        }

        if (choice == 1) {
            cout << "Enter username: ";
            cin >> username;
            bool test = true;
            while (test) {
                cout << "Enter password: ";
                cin >> password;
                PasswordValidator v;
                if (v.validate(password)) { test = false; }
            }
            userService.RegisterUser(username, password);
        } else if (choice == 2) {
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter password: ";
            cin >> password;
            userService.LoginUser(username, password);
        } else if (choice == 3) {
            cout << "Goodbye!\n";
            break;
        } else {
            cout << "Invalid choice, try again!\n";
        }
    }

    return 0;
}
