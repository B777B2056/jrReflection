#ifndef TEST_H
#define TEST_H

#include "../src/reflection.hpp"
#include <iostream>

std::string test_fun(int n);

class Test : jrReflection::Reflectable {
public:
    int attr;

public:
    Test(int x, double y);
    int fun(char ch);

    static jrReflection::Reflectable* createInstance(int x, double y);
};

#endif // TEST_H
