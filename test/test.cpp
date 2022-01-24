#include "test.h"

std::string test_fun(int n) {
    return std::to_string(n);
}

Test::Test(int x, double y) {
    std::cout << "Test construct, x is " << x << ", y is " << y << std::endl;
}

int Test::fun(char ch) {
    return ch;
}
