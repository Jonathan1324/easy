#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cctype>

#include "c++\String.hpp"
#include "c++\Error.hpp"
#include "c++\Arithmetic.hpp"

#include "c\file_utils.h"

enum class TokenType {
    // Funktionstoken
    PRINT,
    
    // Literaltoken
    STRING_LITERAL,
    
    // Symboltoken
    OPEN_PARENTHESIS,
    CLOSE_PARENTHESIS,

    // Zeilen Ende
    SEMICOLON,
    NEWLINE,
    
    // Zusätzliche Token
    UNKNOWN
};

struct Token {
    TokenType type; // Typ des Tokens
    std::string value; // Wert des Tokens

    // Konstruktor für Token
    Token(TokenType t, const std::string& v) : type(t), value(v) {}
};

// Funktion zur Erkennung von Funktionstoken
Token tokenizeFunction(const std::string& code, size_t& i) {
    if (code.substr(i, 5) == "print") {
        i += 5; // Index um 5 erhöhen
        return {TokenType::PRINT, "print"};
    }
    return {TokenType::UNKNOWN, ""}; // Rückgabe UNKNOWN, wenn kein Token erkannt wurde
}

// Funktion zur Erkennung von Literal-Token
Token tokenizeLiteral(const std::string& code, size_t& i) {
    if (code[i] == '"') {
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
    return {TokenType::UNKNOWN, ""}; // Rückgabe UNKNOWN, wenn kein Token erkannt wurde
}

// Hauptfunktion zur Tokenisierung
std::vector<Token> tokenize(const std::string& code) {
    std::vector<Token> tokens;
    size_t i = 0;

    while (i < code.length()) {
        // SPACE, SEMICOLON and NEWLINE
        if (std::isspace(code[i])) {
            // Skip Spaces
            if (code[i] == '\n') {
                tokens.push_back({TokenType::NEWLINE, "\\n"});
            }
            ++i;
            continue;
        } else if (code[i] == ';') {
            tokens.push_back({TokenType::SEMICOLON, ";"});
            ++i;
            continue;
        }


        // TOKENS

        Token functionToken = tokenizeFunction(code, i);
        if (functionToken.type != TokenType::UNKNOWN) {
            tokens.push_back(functionToken);
            continue;
        }

        Token literalToken = tokenizeLiteral(code, i);
        if (literalToken.type != TokenType::UNKNOWN) {
            tokens.push_back(literalToken);
            continue;
        }

        Token symbolToken = tokenizeSymbol(code, i);
        if (symbolToken.type != TokenType::UNKNOWN) {
            tokens.push_back(symbolToken);
            continue;
        }

        tokens.push_back({TokenType::UNKNOWN, std::string(1, code[i])});
        ++i;
    }

    return tokens;
}

int main() {
    std::string code =
    "print(\"Hello, World!\")\nprint(\"Hi World 2!\")";

    std::vector<Token> tokens = tokenize(code);

    for (const auto& token : tokens) {
        switch (token.type) {
            case TokenType::PRINT:
                std::cout << "Token: PRINT, Value: " << token.value << "\n";
                break;
            case TokenType::STRING_LITERAL:
                std::cout << "Token: STRING_LITERAL, Value: \"" << token.value << "\"\n";
                break;
            case TokenType::OPEN_PARENTHESIS:
                std::cout << "Token: OPEN_PARENTHESIS, Value: " << token.value << "\n";
                break;
            case TokenType::CLOSE_PARENTHESIS:
                std::cout << "Token: CLOSE_PARENTHESIS, Value: " << token.value << "\n";
                break;
            case TokenType::SEMICOLON:
                std::cout << "Token: SEMICOLON, Value: " << token.value << "\n";
                break;
            case TokenType::NEWLINE:
                std::cout << "Token: NEWLINE, Value: " << token.value << "\n";
                break;
            default:
                std::cout << "Token: UNKNOWN, Value: " << token.value << "\n";
                break;
        }
    }

    return 0;
}