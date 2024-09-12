#ifndef ERROR_HPP
#define ERROR_HPP

#include <string>
#include <iostream>

class Error {
public:
    static const Error e1;
    static const Error e2;
    static const Error e3;
    static const Error e4;
    static const Error e5;
    static const Error e6;
    static const Error e7;

    Error(int code, const std::string& message);
    
    int getCode() const;
    std::string getMessage() const;
    
    void printErrorMessage() const;
    void printErrorMessageAtLine(int line) const;

private:
    int errorCode;
    std::string errorMessage;
};

#endif