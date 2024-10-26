#ifndef String_HPP
#define String_HPP

#include <iostream>
#include <string>
#include <vector>

bool isNumber(const std::string& s);

constexpr unsigned int str2int(const char* str, int h = 0)
{
    return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}

#endif