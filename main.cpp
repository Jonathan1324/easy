#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <memory>
#include <stdexcept>

#include "c++\String.hpp"
#include "c++\Error.hpp"
#include "c++\Arithmetic.hpp"

#include "c\file_utils.h"

#include "Tokenizer\TokenTypes.cpp"

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

// ASTNode Basis Klasse
class ASTNode {
public:
    virtual ~ASTNode() = default; // Virtueller Destruktor
    virtual void print(int indent = 0) const = 0; // Virtuelle Methode für den Druck
};

// ProgramNode zur Speicherung der gesamten Programmstruktur
class ProgramNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements; // Alle Anweisungen des Programms

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "ProgramNode:\n";
        for (const auto& statement : statements) {
            statement->print(indent + 2); // Indentieren für untergeordnete Knoten
        }
    }
};

// PrintNode zur Speicherung der Print-Anweisung
class PrintNode : public ASTNode {
public:
    std::string value; // Der Wert, der ausgegeben werden soll

    PrintNode(const std::string& val) : value(val) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "PrintNode: " << value << "\n";
    }
};

// Parser-Klasse
class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

    std::unique_ptr<ProgramNode> parse() {
        auto programNode = std::make_unique<ProgramNode>();

        while (!isAtEnd()) {
            programNode->statements.push_back(parseStatement());

            if (currentToken().type == TokenType::NEWLINE || currentToken().type == TokenType::SEMICOLON) {
                advance(); // Überspringe NEWLINE oder SEMICOLON
            } else {
                throw std::runtime_error("Expected NEWLINE or SEMICOLON after statement");
            }
        }

        return programNode;
    }

private:
    const std::vector<Token>& tokens;
    size_t current;

    // Prüfen, ob wir am Ende des Token-Streams sind
    bool isAtEnd() const {
        return current >= tokens.size();
    }

    // Die aktuelle Token zurückgeben
    const Token& currentToken() const {
        return tokens[current];
    }

    // Den nächsten Token zurückgeben und den Index erhöhen
    const Token& advance() {
        return tokens[current++];
    }

    // Die nächste Anweisung parsen
    std::unique_ptr<ASTNode> parseStatement() {
        if (currentToken().type == TokenType::NEWLINE || currentToken().type == TokenType::SEMICOLON) {
            return nullptr; // Keine Anweisung zurückgeben
        }

        if (currentToken().type == TokenType::PRINT) {
            return parsePrintStatement();
        }

        throw std::runtime_error("Unrecognized statement");
    }

    // Eine Print-Anweisung parsen
    std::unique_ptr<ASTNode> parsePrintStatement() {
        advance(); // `print` Token überspringen
        if (currentToken().type != TokenType::OPEN_PARENTHESIS) {
            throw std::runtime_error("Expected '(' after 'print'");
        }
        advance(); // '(' Token überspringen

        if (currentToken().type != TokenType::STRING_LITERAL) {
            throw std::runtime_error("Expected string literal");
        }
        std::string stringValue = currentToken().value;
        advance(); // String literal Token überspringen

        if (currentToken().type != TokenType::CLOSE_PARENTHESIS) {
            throw std::runtime_error("Expected ')' after string literal");
        }
        advance(); // ')' Token überspringen

        // Rückgabe eines neuen PrintNode
        return std::make_unique<PrintNode>(stringValue);
    }
};

class SemanticAnalyzer {
public:
    void analyze(const ProgramNode& programNode) {
        for (const auto& statement : programNode.statements) {
            analyzeStatement(statement);
        }
    }

private:
    void analyzeStatement(const std::unique_ptr<ASTNode>& statement) {
        if (auto printNode = dynamic_cast<const PrintNode*>(statement.get())) {
            analyzePrintNode(*printNode);
        }
        // Hier kannst du weitere Anweisungen hinzufügen, wenn du sie implementierst.
    }

    void analyzePrintNode(const PrintNode& printNode) {
        // Überprüfen, ob der übergebene Wert ein String ist
        if (printNode.value.empty()) {
            throw std::runtime_error("Error: print statement must have a string value.");
        }
        // Hier kannst du weitere Bedingungen einfügen, falls notwendig.
    }
};

class Compiler {
public:
    explicit Compiler(const std::shared_ptr<ProgramNode>& programNode) : programNode(programNode) {}

    std::string generateCode() {
        std::string code = "";
        for (const auto& statement : programNode->statements) {
            if (auto printNode = dynamic_cast<PrintNode*>(statement.get())) {
                code += "print ( \"" + printNode->value + "\" ) ;\n";
            }
        }
        return code;
    }

private:
    std::shared_ptr<ProgramNode> programNode;
};

class Interpreter {
public:
    void interpret(const ProgramNode& programNode) {
        for (const auto& statement : programNode.statements) {
            interpretStatement(statement);
        }
    }

private:
    void interpretStatement(const std::unique_ptr<ASTNode>& statement) {
        if (auto printNode = dynamic_cast<PrintNode*>(statement.get())) {
            interpretPrintNode(*printNode);
        }
    }

    void interpretPrintNode(const PrintNode& printNode) {
        std::cout << printNode.value << std::endl; // Ausgabe des Strings
    }
};

void writeToFile(const std::string& filename, const std::string& content) {
    std::ofstream outFile(filename); // Datei im Schreibmodus öffnen

    if (!outFile) { // Überprüfen, ob die Datei geöffnet werden konnte
        std::cerr << "Error writing to file: " << filename << std::endl;
        return;
    }

    outFile << content; // Inhalt in die Datei schreiben
    outFile.close(); // Datei schließen
}

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

    bool debugShowTokens = false;
    bool debugShowAST = false;
    bool debugShowCompiled = false;

    bool compile = true;
    bool interpret = true;

    std::string code(file_content);

    code += ";";

    std::vector<Token> tokens = tokenize(code);

    if(debugShowTokens) {
        std::cout << "Tokens\n";
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
    }

    Parser parser(tokens);
    std::unique_ptr<ProgramNode> programNode = parser.parse();

    SemanticAnalyzer semanticAnalyzer;
    try {
        semanticAnalyzer.analyze(*programNode);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << '\n';
        return 1; // ERROR
    }

    if(debugShowAST) {
        std::cout << "\nAST Structure:\n";
        programNode->print();
    }

    Interpreter interpreter;

    if(interpret) {
        interpreter.interpret(*programNode);
    }

    Compiler compiler(std::move(programNode));

    if(compile) {
        std::string compiled = compiler.generateCode();

        writeToFile("output.easy", compiled);

        if(debugShowCompiled) {
            std::cout << "\nCompiled Code:\n";
            std::cout << compiled;
        }
    }

    return 0;
}