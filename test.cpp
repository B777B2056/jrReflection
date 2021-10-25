#include "test.h"

Test::Test(int x, double y) {
    std::cout << "Test construct, x is " << x << ", y is " << y << std::endl;
}

int Test::fun(char ch) {
    return ch;
}

std::shared_ptr<Test> createTest(int x, double y) {
    return std::make_shared<Test>(x, y);
}
