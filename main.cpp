#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "utils.cpp"

using namespace std;

class Error {
public:
    static const Error e1;
    static const Error e2;
    static const Error e3;
    static const Error e4;
    static const Error e5;
    static const Error e6;

    Error(int code, const std::string& message) : errorCode(code), errorMessage(message) {}
    
    int getCode() const { return errorCode; }
    std::string getMessage() const { return errorMessage; }
    
    void printErrorMessage() const {
        std::cout << errorMessage << "\nEXITCODE ::: " << "E:" << errorCode << std::endl;
    }

    void printErrorMessageAtLine(int line) const {
        std::cout << "\n";
        std::cout << "Line : " << line << "\n" << errorMessage << "\nEXITCODE ::: " << "E:" << errorCode << std::endl;
    }

private:
    int errorCode;
    std::string errorMessage;
};

const Error Error::e1 = Error(1, "Not enough Arguments");
const Error Error::e2 = Error(2, "File not found");
const Error Error::e3 = Error(3, "Not enough Arguments");
const Error Error::e4 = Error(4, "Too many Arguments");
const Error Error::e5 = Error(5, "Argument doesn't have the right type");
const Error Error::e6 = Error(6, "Not allowed characters");

void checkForCommand(string, int);
bool checkForArguments(int, int, int);

int main(int argc, char* argv[]) {

    string code = "";

    if (argc < 2) {
        Error::e1.printErrorMessage();
        return 1;
    }

    ifstream myfile (argv[1]);

    if ( myfile.is_open() ) {
        while ( myfile.good() ) {
            char mychar = myfile.get();
            code += mychar;
        }
    } else {
        Error::e2.printErrorMessage();
        return 0;
    }

    string substring = "";

    string findChars = "\n\"";

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

    for (int i = 0; i < code.length(); i++) {
        substring += code[i];

        if (code.substr(i, 1) == ";") {
            checkForCommand(substring, j);
            substring = "";
            j++;
        }
    }
    checkForCommand(substring.substr(0, substring.size()-1) + ";", j);

    return 0;
}

void checkForCommand(string str, int line) {
    string substring = "";
    string String = "";
    string intStr = "";
    vector<string> strings;
    vector<int> ints;
    int arguments = 0;

    bool inString = false;
    bool inNumber = false;

    string function = "";

    for (int i = 0; i < str.length(); i++)
    {
        if(inString){
            if (str[i] == '\'') {
                inString = false;
                strings.push_back(String);
                String = "";
                substring += "###STRING_ARGUMENT###";
                arguments++;
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
                if(isdigit(str[i])) {
                    intStr += str[i];
                } else if(str[i] == ',' || str[i] == ')') {
                    for (size_t j = 0; j < intStr.length(); j++)
                    {
                        if(!(isdigit(str[i]))) {
                            Error::e6.printErrorMessageAtLine(line);
                            return;
                        }
                    }

                    inNumber = false;
                    ints.push_back(1);
                    intStr = "";
                    substring += "###INT_ARGUMENT###";
                    substring += str[i];
                    arguments++;
                }
            }
        } else {

            if(str[i] == '(' && function == "") {
                function = substring;
            }

            if(str[i] != ' ') {
                if (str[i] == '\'') {
                    inString = true;
                } else if(isdigit(str[i])) {
                    inNumber = true;
                } else {
                    substring += str[i];
                }
            }
        }
    }

    switch (str2int(function.c_str()))
    {
    case str2int("print"):
        if(checkForArguments(arguments, 1, line)) {
            if(substring == "print(###STRING_ARGUMENT###);") {
                cout << strings.at(0) + "\n";
            } else {
                Error::e5.printErrorMessageAtLine(line);
            }
        }
        break;
    
    default:
        break;
    }
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