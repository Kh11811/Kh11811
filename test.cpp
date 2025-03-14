int main() {
    FileManager fileManager;

    // Read test cases from file
    string testCaseFile = "testcases.txt";
    auto testCases = readTestCases(testCaseFile);

    if (testCases.empty()) {
        cerr << "No test cases found or error reading file." << endl;
        return 1;
    }

    // Add users and verify
    addAndVerifyUsers(fileManager, testCases);

    return 0;
}
