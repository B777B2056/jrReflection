#ifndef TEST_H
#define TEST_H

#include "../src/reflection.hpp"
#include <iostream>

std::string test_fun(int n);

class Test : public jrReflection::Reflectable {
public:
    int attr;

public:
    Test(int x, double y);
    int fun(char ch);
    ~Test() { std::cout << "Test dtor." << std::endl; }
};

#endif // TEST_H
