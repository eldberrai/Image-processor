#pragma once
#include <string>
#include <iostream>
#include <map>
#include <vector>

class Parser {
public:
    std::string info_in;
    std::string info_out;
    std::vector<std::pair<std::string, std::vector<float>>> filters;
};

Parser Parse(int argc, char** argv);