#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "c++\utils.cpp"
#include "c++\string.cpp"

#include "c++\Error.hpp"
#include "c++\Arithmetic.hpp"

#include "c\file_utils.h"

extern "C" int test();

bool checkForCommand(std::string, int);
bool checkForArguments(int, int, int);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        Error::e1.printErrorMessage();
        return 1;
    }

    char* file_content = read_file(argv[1]);
    if (!file_content) {
        Error::e2.printErrorMessage();
        return 1;
    }

    int result = test();

    std::cout << "The result is: " << result << std::endl;

    std::string code(file_content);

    std::string substring = "";

    std::string findChars = "\n\"";

    for (size_t i = 0; i < code.size(); i++) {
        for (char find : findChars) {
            if (code[i] == find) {
                if (find == '\"') {
                    code[i] = '\'';
                } else if (find == '\n') {
                    code[i] = ';';
                } else {
                    code.erase(i, 1);
                    i--;
                }
                break;
            }
        }
    }
    
    int j = 1;
    bool worked = true;

    for (int i = 0; i < code.length(); i++) {
        substring += code[i];

        if (code.substr(i, 1) == ";") {
            worked = checkForCommand(substring, j);
            substring = "";
            j++;

            if (!worked) {
                break;
            }
        }
    }

    std::string str = "";

    if (worked) {
        str = substring.substr(0, substring.size()) + ";";
    }

    checkForCommand(str, j);

    return 0;
}

bool checkForCommand(std::string str, int line) {

    std::string substring = "";
    std::string String = "";
    std::string intStr = "";
    std::vector<std::string> strings;
    std::vector<std::string> intStrs;
    int arguments = 0;

    bool inString = false;
    bool inNumber = false;

    std::string function = "";
    bool isConcatenating = false;
    bool icString = false;
    bool icInt = false;

    bool setFunction = false;

    int brackets = 0;

    for (int i = 0; i < str.length(); i++)
    {
        if(inString){
            if (str[i] == '\'') {
                inString = false;
                if (isConcatenating && !strings.empty()) {
                    strings.back() += String;  // Concatenate to the previous string
                } else {
                    strings.push_back(String);  // Push new string if not concatenating
                    substring += "###STRING_ARGUMENT###";
                    arguments++;
                }
                String = "";
                icString = true;
            } else {
                if(str[i] == '\\' && str[i+1] == 'n') {
                    String += '\n';
                    i++;
                } else {
                    String += str[i];
                }
            }
        } else if(inNumber) {
            if(str[i] != ' ') {
                if(isdigit(str[i]) || str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/' || str[i] == '(') {
                    intStr += str[i];
                } else if(str[i] == ',' || str[i] == ')') {
                    if (str[i] == ')' && str[i + 1] == ';') {
                        if (true) {
                            for (size_t j = 0; j < intStr.length(); j++) {
                                if(!(isdigit(intStr[j]) || intStr[j] == '+' || intStr[j] == '-' || intStr[j] == '*' || intStr[j] == '/' || intStr[j] == '(' || intStr[j] == ')')) {
                                    Error::e6.printErrorMessageAtLine(line);
                                    return false;
                                }
                            }

                            if (isConcatenating && icString) {
                                strings.back() += evaluate(intStr);  // Concatenate strings
                            } else {
                                intStrs.push_back(evaluate(intStr));
                                substring += "###INT_ARGUMENT###";
                                arguments++;
                                icInt = true;
                            }

                            inNumber = false;
                            substring += str[i];

                            intStr = "";
                        }

                        if (str[i] == ')') {
                            brackets -= 1;
                        }
                    } else {
                        intStr += str[i];
                    }
                }
            }
        } else {
            if(str[i] == '(' && !setFunction) {
                function = substring;
            } else {
                brackets += 1;
            }
            
            if(str[i] != ' ') {
                if (str[i] == '\'') {
                    inString = true;
                } else if(isdigit(str[i])) {
                    inNumber = true;
                    i -= 1;
                } else if (str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/') {
                    isConcatenating = true;  // Set flag when + is encountered
                } else if (str[i] == ',') {
                    isConcatenating = false;
                    icString = false;
                    icInt = false;
                } else {
                    if(str[i] != '(' || !setFunction) {
                        substring += str[i];
                    } else {
                        intStr += str[i];
                    }
                }
            }

            if (function != "") {
                setFunction = true;
            }
        }
    }

    switch (str2int(function.c_str()))
    {
    case str2int("print"):
        if(checkForArguments(arguments, 1, line)) {
            if(substring == "print(###STRING_ARGUMENT###);") {
                std::cout << strings.at(0) + "\n";
            } else if (substring == "print(###INT_ARGUMENT###);") {
                std::cout << intStrs.at(0);
                std::cout << "\n";
            } else {
                Error::e5.printErrorMessageAtLine(line);
                return false;
            }
        } else {
            return false;
        }
        break;
    
    default:
        break;
    }

    return true;
}

bool checkForArguments(int arguments, int goalArguments, int line) {
    if(goalArguments == arguments) {
        return true;
    } else if (goalArguments > arguments) {
        Error::e3.printErrorMessageAtLine(line);
        return false;
    } else {
        Error::e4.printErrorMessageAtLine(line);
        return false;
    }
}