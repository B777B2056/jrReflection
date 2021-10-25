#ifndef TEST_H
#define TEST_H

#include "class_info.h"
#include <iostream>

class Test {
public:
    int attr;

public:
    Test(int x, double y);
    int fun(char ch);
};

std::shared_ptr<Test> createTest(int x, double y);

#endif // TEST_H
