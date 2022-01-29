#ifndef TEST_H
#define TEST_H

#include "../src/reflection.hpp"
#include <iostream>

std::string test_fun(int n);

struct Test : public jrReflection::Reflectable {
    int attr;
    std::string attr2;
    std::string attr3;
    std::string attr4;

    Test(int x, double y);
    int fun(char ch);
    std::string fun2(int n);
    ~Test() { std::cout << "Test dtor." << std::endl; }
};

#endif // TEST_H
