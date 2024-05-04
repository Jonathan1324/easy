#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

class Error {
public:
    static const Error e1;
    static const Error e2;
    static const Error e3;
    static const Error e4;
    static const Error e5;

    Error(int code, const std::string& message) : errorCode(code), errorMessage(message) {}
    
    int getCode() const { return errorCode; }
    std::string getMessage() const { return errorMessage; }
    
    void printErrorMessage() const {
        std::cout << errorMessage << "\nEXITCODE ::: " << "E:" << errorCode << std::endl;
    }

    void printErrorMessageAtLine(int line) const {
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

void checkForCommand(string str, int line);

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
    return 0;
}

void checkForCommand(string str, int line) {
    string substring = "";
    string String = "";
    vector<string> strings;
    int Arguments;

    bool inString = false;

    for (int i = 0; i < str.length(); i++) {
        if(inString){
            if (str[i] == '\'') {
                inString = false;
                strings.push_back(String);
                String = "";
                substring += "###STRING_ARGUMENT###";
            } else {
                if(str[i] == '\\' && str[i+1] == 'n') {
                    String += '\n';
                    i++;
                } else {
                    String += str[i];
                }
            }
        } else {

            if(str[i] != ' '){
                if (str[i] == '\'') {
                    inString = true;
                } else {
                    substring += str[i];
                }
            }
        }
    }

    if(substring == "print(###STRING_ARGUMENT###);") {
        cout << strings.at(0) + "\n";
    } else if (substring == "print();") {
        Error::e3.printErrorMessageAtLine(line);
    } else if (Arguments > 1) {
        Error::e4.printErrorMessageAtLine(line);
    } else {
        Error::e5.printErrorMessageAtLine(line);
    }
}