#include <iostream>
#include <string>

#include "TokenTypes.cpp"

// Funktion zur Erkennung von Literal-Token
Token tokenizeLiteral(const std::string& code, size_t& i) {
    if (std::isdigit(code[i])) {
        // Integer-Literal erkennen
        std::string number;
        while (i < code.length() && std::isdigit(code[i])) {
            number += code[i];
            ++i;
        }
        return {TokenType::INT_LITERAL, number}; // Erstelle INT_LITERAL Token
    } else if (code[i] == '"') {
        std::string stringLiteral;
        ++i; // Überspringt das öffnende "
        while (i < code.length() && code[i] != '"') {
            stringLiteral += code[i];
            ++i;
        }
        if (i < code.length() && code[i] == '"') {
            ++i; // Überspringt das schließende "
        }
        return {TokenType::STRING_LITERAL, stringLiteral};
    } else if (code.substr(i, 4) == "true" && !(std::isalnum(code[i + 5])) || code.substr(i, 4) == "True" && !(std::isalnum(code[i + 5]))) {
        i += 4;
        return {TokenType::BOOL_LITERAL, "true"};
    } else if (code.substr(i, 5) == "false" && !(std::isalnum(code[i + 6])) || code.substr(i, 5) == "False" && !(std::isalnum(code[i + 6]))) {
        i += 5;
        return {TokenType::BOOL_LITERAL, "false"};
    }
    return {TokenType::UNKNOWN, ""}; // Rückgabe UNKNOWN, wenn kein Token erkannt wurde
}

Token tokenizeSymbol(const std::string& code, size_t& i) {
    if (code[i] == '(') {
        ++i; // Index erhöhen
        return {TokenType::OPEN_PARENTHESIS, "("};
    }
    if (code[i] == ')') {
        ++i; // Index erhöhen
        return {TokenType::CLOSE_PARENTHESIS, ")"};
    }
    if (code[i] == '{') {
        ++i; // Index erhöhen
        return {TokenType::OPEN_BRACE, "("};
    }
    if (code[i] == '}') {
        ++i; // Index erhöhen
        return {TokenType::CLOSE_BRACE, ")"};
    }
    if (code[i] == ',') {
        ++i; // Index erhöhen
        return {TokenType::COMMA, ","};
    }
    return {TokenType::UNKNOWN, ""}; // Rückgabe UNKNOWN, wenn kein Token erkannt wurde
}

Token tokenizeKeyword(const std::string& code, size_t& i) {
    if (code.substr(i, 3) == "var") {
        i += 3;
        return {TokenType::VAR, "var"};
    } else if (code.substr(i, 5) == "const") {
        i += 5;
        return {TokenType::CONST, "const"};
    } else if (code.substr(i, 4) == "func") {
        i += 4;
        return {TokenType::FUNC, "func"};
    }
    return {TokenType::UNKNOWN, ""};
}

// Funktion zur Erkennung von Bezeichnern
Token tokenizeIdentifier(const std::string& code, size_t& i) {
    std::string identifier;
    while (i < code.length() && (std::isalnum(code[i]) || code[i] == '_')) {
        identifier += code[i];
        ++i;
    }
    return {TokenType::IDENTIFIER, identifier};
}

// Funktion zur Erkennung von Zuweisungen
Token tokenizeAssignment(const std::string& code, size_t& i) {
    if (code[i] == '=') {
        ++i; // Index erhöhen
        return {TokenType::ASSIGNMENT, "="};
    }
    return {TokenType::UNKNOWN, ""}; // Rückgabe UNKNOWN, wenn kein Token erkannt wurde
}

Token tokenizeComment(const std::string& code, size_t& i) {
    if (code[i] == '#' && code[i + 1] == '#' || code[i] == '/' && code[i + 1] == '*') {
        std::string comment = "\n";
        i += 2;
        bool doWhile = true;
        while (doWhile) {
            if(code[i] != '\n') {
                comment += code[i];
            }
            ++i;

            if(i >= code.length() || code[i] == '#' && code[i + 1] == '#' || code[i] == '*' && code[i + 1] == '/') {
                doWhile = false;
            }
        }
        if (i < code.length() && code[i] == '#' && code[i + 1] == '#' || i < code.length() && code[i] == '*' && code[i + 1] == '/') {
            i += 2;
        }

        return {TokenType::MULTICOMMENT, comment};
    } else if (code[i] == '#' || code[i] == '/' && code[i + 1] == '/') {
        if(code[i] == '/') {
            ++i;
        }
        std::string comment;
        ++i;
        while (i < code.length() && code[i] != '\n') {
            comment += code[i];
            ++i;
        }
        if (i < code.length() && code[i] == '\n') {
            ++i;
        }
        return {TokenType::COMMENT, comment};
    }
    return {TokenType::UNKNOWN, ""};
}

Token tokenizeArithmetic(const std::string& code, size_t& i) {
    if (code[i] == '+') {
        ++i; // Index erhöhen
        return {TokenType::PLUS, "+"};
    } else if (code[i] == '-') {
        ++i; // Index erhöhen
        return {TokenType::MINUS, "-"};
    } else if (code[i] == '*') {
        ++i; // Index erhöhen
        return {TokenType::STAR, "*"};
    } else if (code[i] == '/') {
        ++i; // Index erhöhen
        return {TokenType::SLASH, "/"};
    }

    return {TokenType::UNKNOWN, std::to_string(code[i])};
}

Token tokenizeLogicalOperators(const std::string& code, size_t& i) {
    if (code[i] == '&' && code[i + 1] == '&') {
        i+=2;
        return {TokenType::AND, "AND"};
    } else if (code[i] == '|' && code[i + 1] == '|') {
        i+=2;
        return {TokenType::OR, "OR"};
    } else if (code[i] == '!') {
        ++i;
        return {TokenType::NOT, "!"};
    }

    return {TokenType::UNKNOWN, std::to_string(code[i])};
}

Token tokenizeComparisonOperators(const std::string& code, size_t& i) {
    if (code[i] == '=' && code[i + 1] == '=') {
        i+=2;
        return {TokenType::EQUALS, "=="};
    } else if (code[i] == '!' && code[i + 1] == '=') {
        i+=2;
        return {TokenType::NOT_EQUALS, "!="};
    } else if (code[i] == '>') {
        ++i;
        return {TokenType::GREATER_THAN, ">"};
    } else if (code[i] == '<') {
        ++i;
        return {TokenType::LESS_THAN, "<"};
    } else if (code[i] == '>' && code[i + 1] == '=') {
        i+=2;
        return {TokenType::GREATER_OR_EQUAL, ">="};
    } else if (code[i] == '<' && code[i + 1] == '=') {
        i+=2;
        return {TokenType::LESS_OR_EQUAL, "<="};
    }

    return {TokenType::UNKNOWN, std::to_string(code[i])};
}

Token tokenizeControlStructures(const std::string& code, size_t& i) {
    if (code.substr(i, 2) == "if") {
        i += 2;
        return {TokenType::IF, "if"};
    } else if (code.substr(i, 4) == "else") {
        i += 4;
        return {TokenType::ELSE, "else"};
    } else if (code.substr(i, 5) == "while") {
        i += 5;
        return {TokenType::WHILE, "while"};
    } else if (code.substr(i, 3) == "for") {
        i += 3;
        return {TokenType::FOR, "for"};
    } else if (code.substr(i, 6) == "return") {
        i += 6;
        return {TokenType::RETURN, "return"};
    }

    return {TokenType::UNKNOWN, std::to_string(code[i])};
}