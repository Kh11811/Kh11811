#include <bits/stdc++.h>
#include <ctime>

using namespace std;

// Your existing code (Istorage, FileManager, SaltGenerator, IHash, Hasher, IValidator, PasswordValidator, IuserService, UserService) goes here...

// Function to run a single test case
bool run_test(FILE *file, int test_case_num, UserService &userService) {
    char username[256], password[256], expected[256];

    // Read test case from file
    if (fscanf(file, "%s %s %s", username, password, expected) != 3) {
        if (feof(file)) {
            return false; // End of file
        }
        printf("Test case %d: Error reading dataset file\n", test_case_num);
        return false;
    }

    // Determine the operation based on the expected result
    if (strcmp(expected, "success") == 0) {
        // Test registration
        userService.RegisterUser(username, password);
        printf("Test case %d: Registration for %s completed\n", test_case_num, username);
        return true;
    } else {
        // Test login
        bool result = userService.LoginUser(username, password);
        bool expected_result = (strcmp(expected, "true") == 0);

        if (result == expected_result) {
            printf("Test case %d: Passed\n", test_case_num);
            return true;
        } else {
            printf("Test case %d: Failed, expected %s, got %s\n", test_case_num, expected, result ? "true" : "false");
            return false;
        }
    }
}

int main() {
    FILE *file = fopen("testcase.txt", "r");
    if (file == NULL) {
        perror("Error opening testcase file");
        return 1;
    }

    UserService userService;
    int test_case_num = 0;
    int passed = 0, failed = 0;

    printf("Running tests...\n");
    clock_t start_time = clock();
    while (true) {
        test_case_num++;
        if (!run_test(file, test_case_num, userService)) {
            if (feof(file)) break; // End of file
            failed++;
        } else {
            passed++;
        }
    }
    clock_t end_time = clock();

    fclose(file);
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("\nTotal tests: %d | Passed: %d | Failed: %d\n", passed + failed, passed, failed);
    printf("Execution time: %.2f seconds\n", elapsed_time);

    return (failed == 0) ? 0 : 1;
}
