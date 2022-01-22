#ifndef TEST_H
#define TEST_H

#include "../src/reflection.hpp"
#include <iostream>

class Test : public jrReflection::Reflectable {
public:
    int attr;

public:
    Test(int x, double y);
    int fun(char ch);

    static jrReflection::Reflectable* createInstance(int x, double y);
};

#endif // TEST_H
