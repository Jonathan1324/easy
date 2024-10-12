#include <iostream>
#include <string>

#include "TokenTypes.cpp"

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

Token tokenizeKeyword(const std::string& code, size_t& i) {
    if (code.substr(i, 3) == "var") {
        i += 3;
        return {TokenType::VAR, "var"};
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