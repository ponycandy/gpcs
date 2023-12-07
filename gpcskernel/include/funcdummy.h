#pragma once

#include <iostream>
#include "tasktuple.h"
#include <map>
class FuncDummy
{
public:
    int num = 0;
    std::map<int, TaskTuple*> tuples;

};

