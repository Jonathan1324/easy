#include "Error.hpp"


const Error Error::e1 = Error(1, "Not enough Arguments");
const Error Error::e2 = Error(2, "File not found");
const Error Error::e3 = Error(3, "Not enough Arguments");
const Error Error::e4 = Error(4, "Too many Arguments");
const Error Error::e5 = Error(5, "Argument doesn't have the right type");
const Error Error::e6 = Error(6, "Not allowed characters");


Error::Error(int code, const std::string& message) : errorCode(code), errorMessage(message) {}

int Error::getCode() const { return errorCode; }

std::string Error::getMessage() const { return errorMessage; }

void Error::printErrorMessage() const {
    std::cout << errorMessage << "\nEXITCODE ::: " << "E:" << errorCode << std::endl;
}

void Error::printErrorMessageAtLine(int line) const {
    std::cout << "\n";
    std::cout << "Line : " << line << "\n" << errorMessage << "\nEXITCODE ::: " << "E:" << errorCode << std::endl;
}