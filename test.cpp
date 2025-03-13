#include <gtest/gtest.h>
#include "projetc++.cpp"

TEST(UserServiceTest, RegisterUserTest) {
    UserService userService;
    userService.RegisterUser("testuser", "Test@1234");
    EXPECT_TRUE(userService.LoginUser("testuser", "Test@1234"));
}

TEST(UserServiceTest, LoginUserTest) {
    UserService userService;
    userService.RegisterUser("testuser", "Test@1234");
    EXPECT_FALSE(userService.LoginUser("testuser", "wrongpassword"));
}

TEST(PasswordValidatorTest, ValidatePasswordTest) {
    PasswordValidator validator;
    EXPECT_TRUE(validator.validate("Test@1234"));
    EXPECT_FALSE(validator.validate("weak"));
}
