#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <set>
#include <cstring>

#include "c++\String.hpp"
#include "c++\Error.hpp"
#include "c++\Arithmetic.hpp"

#include "c\file_utils.h"

#include "Tokenizer\tokenizeToken.cpp"

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

        Token commentToken = tokenizeComment(code, i);
        if (commentToken.type != TokenType::UNKNOWN) {
            tokens.push_back({TokenType::NEWLINE, "\\n"});
            tokens.push_back(commentToken);
            continue;
        }

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

        Token keywordToken = tokenizeKeyword(code, i);
        if (keywordToken.type != TokenType::UNKNOWN) {
            tokens.push_back(keywordToken);
            continue;
        }

        Token assignmentToken = tokenizeAssignment(code, i);
        if (assignmentToken.type != TokenType::UNKNOWN) {
            tokens.push_back(assignmentToken);
            continue;
        }

        Token identifiertToken = tokenizeIdentifier(code, i);
        if (identifiertToken.type != TokenType::UNKNOWN) {
            tokens.push_back(identifiertToken);
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
            if(statement != nullptr) {
                statement->print(indent + 2); // Indentieren für untergeordnete Knoten
            }
        }
    }
};

class IntLiteralNode : public ASTNode {
public:
    int value;

    IntLiteralNode(int val) : value(val) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "IntLiteralNode: " << value << "\n";
    }
};

class StringLiteralNode : public ASTNode {
public:
    std::string value;

    StringLiteralNode(const std::string& val) : value(val) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "StringLiteralNode: " << value << "\n";
    }
};

class PrintNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> expression; // Der Ausdruck, der ausgegeben werden soll

    PrintNode(std::unique_ptr<ASTNode> expr) : expression(std::move(expr)) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "PrintNode:\n";
        expression->print(indent + 2); // Ausgabe des Ausdrucks
    }
};

// Knoten für die Variablendeklaration
class VarDeclarationNode : public ASTNode {
public:
    std::string varName; // Der Name der Variable
    std::unique_ptr<ASTNode> expression; // Ausdruck, der der Variable zugewiesen wird

    VarDeclarationNode(const std::string& name, std::unique_ptr<ASTNode> expr)
        : varName(name), expression(std::move(expr)) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "VarDeclarationNode: " << varName << "\n";
        expression->print(indent + 2); // Drucke den Ausdruck
    }
};

class VarNode : public ASTNode {
public:
    std::string name; // Der Name der Variable

    VarNode(const std::string& name) : name(name) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "VarNode: " << name << "\n";
    }
};

class CommentNode : public ASTNode {
public:
    std::string comment;

    CommentNode(const std::string& val) : comment(val) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "CommentNode: " << comment << "\n";
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
            } else if(currentToken().type == TokenType::COMMENT) {
                programNode->statements.push_back(std::make_unique<CommentNode>(currentToken().value));
                advance();
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

        if(currentToken().type == TokenType::COMMENT) {
            return nullptr;
        }

        if (currentToken().type == TokenType::PRINT) {
            return parsePrintStatement();
        }

        if (currentToken().type == TokenType::VAR) {
            return parseVarDeclaration();
        }

        throw std::runtime_error("Unrecognized statement");
    }

    std::unique_ptr<ASTNode> parseVarDeclaration() {
        advance(); // var Token überspringen

        if (currentToken().type != TokenType::IDENTIFIER) {
            throw std::runtime_error("Expected variable name after 'var'");
        }
        std::string varName = currentToken().value;
        advance(); // Variable Name überspringen

        if (currentToken().type != TokenType::ASSIGNMENT) {
            throw std::runtime_error("Expected '=' after variable name");
        }
        advance(); // '=' Token überspringen

        auto expression = parseExpression(); // Hier den Ausdruck parsen

        return std::make_unique<VarDeclarationNode>(varName, std::move(expression));
    }

    // Eine Print-Anweisung parsen
    std::unique_ptr<ASTNode> parsePrintStatement() {
        advance(); // `print` Token überspringen
        if (currentToken().type != TokenType::OPEN_PARENTHESIS) {
            throw std::runtime_error("Expected '(' after 'print'");
        }
        advance(); // '(' Token überspringen

        std::unique_ptr<ASTNode> expression;

        if (currentToken().type == TokenType::STRING_LITERAL) {
            expression = std::make_unique<StringLiteralNode>(currentToken().value);
            advance(); // String literal Token überspringen
        } else if (currentToken().type == TokenType::INT_LITERAL) {
            expression = std::make_unique<StringLiteralNode>(currentToken().value);
            advance(); // String literal Token überspringen
        } else if (currentToken().type == TokenType::IDENTIFIER) { // Hier ID für die Variablen
            expression = std::make_unique<VarNode>(currentToken().value);
            advance(); // Identifier Token überspringen
        } else {
            throw std::runtime_error("Expected string literal or variable name");
        }

        if (currentToken().type != TokenType::CLOSE_PARENTHESIS) {
            throw std::runtime_error("Expected ')' after argument");
        }
        advance(); // ')' Token überspringen

        return std::make_unique<PrintNode>(std::move(expression)); // Rückgabe eines neuen PrintNode
    }

    std::unique_ptr<ASTNode> parseExpression() {
        if (currentToken().type == TokenType::STRING_LITERAL) {
            std::string stringValue = currentToken().value;
            advance(); // String literal Token überspringen
            return std::make_unique<StringLiteralNode>(stringValue);
        } else if (currentToken().type == TokenType::INT_LITERAL) {
            std::string stringValue = currentToken().value;
            int intValue = std::stoi(stringValue);
            advance(); // String literal Token überspringen
            return std::make_unique<IntLiteralNode>(intValue);
        } else if (currentToken().type == TokenType::IDENTIFIER) {
            std::string varName = currentToken().value;
            advance(); // Identifier Token überspringen
            return std::make_unique<VarNode>(varName); // Erstelle einen VarNode, um auf die Variable zuzugreifen
        }
        throw std::runtime_error("Expected expression");
    }
};

class SemanticAnalyzer {
public:
    void analyze(const ProgramNode& programNode) {
        // Leeres Map für die Variablen
        variables.clear();
        for (const auto& statement : programNode.statements) {
            analyzeStatement(statement);
        }
    }

private:
    std::unordered_map<std::string, std::string> variables; // Map für Variablen
    std::set<std::string> declaredVariables;

    void analyzeStatement(const std::unique_ptr<ASTNode>& statement) {
        if (auto printNode = dynamic_cast<const PrintNode*>(statement.get())) {
            analyzePrintNode(*printNode);
        } else if (auto varDeclNode = dynamic_cast<const VarDeclarationNode*>(statement.get())) {
            analyzeVarDeclarationNode(*varDeclNode);
        } else if (auto intLiteralNode = dynamic_cast<const IntLiteralNode*>(statement.get())) {
            
        } else if (auto stringLiteralNode = dynamic_cast<const StringLiteralNode*>(statement.get())) {
            
        } else if (auto varNode = dynamic_cast<const VarNode*>(statement.get())) {
            
        } else {
            //throw std::runtime_error("Unrecognized statement");
        }
    }

    bool isVariableDeclared(const std::string& varName) {
        // Hier sollte der Code stehen, der überprüft, ob die Variable deklariert wurde
        return declaredVariables.count(varName) > 0; // Beispiel mit std::set oder std::unordered_set
    }

     void analyzeVarDeclarationNode(const VarDeclarationNode& varDeclNode) {
        // Überprüfen, ob der Variablenname ein gültiger Bezeichner ist
        if (varDeclNode.varName.empty()) {
            throw std::runtime_error("Error: Variable name cannot be empty.");
        }

        // Hier analysierst du den Ausdruck, der der Variable zugewiesen wird
        if (varDeclNode.expression) {
            // Hier könnte eine Methode aufgerufen werden, um den Ausdruck zu analysieren
            analyzeExpression(*varDeclNode.expression);
        } else {
            throw std::runtime_error("Error: Variable declaration must have an expression.");
        }
    }

    void analyzeExpression(const ASTNode& expression) {
        if (dynamic_cast<const StringLiteralNode*>(&expression)) {
            // Hier kannst du überprüfen, ob es ein StringLiteral ist
        } else if (dynamic_cast<const IntLiteralNode*>(&expression)) {
            // Hier kannst du überprüfen, ob es ein StringLiteral ist
        } else if (const auto* varNode = dynamic_cast<const VarNode*>(&expression)) {
            // Hier kannst du überprüfen, ob es eine gültige Variable ist
            // Zum Beispiel, ob die Variable deklariert wurde
            if (!isVariableDeclared(varNode->name)) {
                throw std::runtime_error("Error: Variable '" + varNode->name + "' is not declared.");
            }
        } else {
            throw std::runtime_error("Error: Unsupported expression type.");
        }
    }

    void analyzePrintNode(const PrintNode& printNode) {
        // Überprüfen, ob der Ausdruck ein StringLiteralNode ist
        if (auto strNode = dynamic_cast<const StringLiteralNode*>(printNode.expression.get())) {
            // Der Ausdruck ist ein gültiger StringLiteralNode
            return; // Nichts weiter zu tun
        }

        // Überprüfen, ob der Ausdruck eine Variable (VarNode) ist
        if (auto varNode = dynamic_cast<const VarNode*>(printNode.expression.get())) {
            return; // Nichts weiter zu tun
        }

        throw std::runtime_error("Error: print statement must have a valid string expression.");
    }
};

class Compiler {
public:
    explicit Compiler(const std::shared_ptr<ProgramNode>& programNode) : programNode(programNode) {}

    std::string generateCode() {
        std::string code = "";
        for (const auto& statement : programNode->statements) {
            if (auto printNode = dynamic_cast<PrintNode*>(statement.get())) {
                code += generatePrintCode(*printNode);
            } else if (auto varDeclNode = dynamic_cast<VarDeclarationNode*>(statement.get())) {
                code += generateVarDeclarationCode(*varDeclNode);
            } else if (auto commentNode = dynamic_cast<CommentNode*>(statement.get())) {
                code += generateCommentCode(*commentNode);
            }
        }
        return code;
    }

private:
    std::shared_ptr<ProgramNode> programNode;

    std::string generatePrintCode(const PrintNode& printNode) {
        // Überprüfen, ob der Ausdruck eine Variable ist oder ein Literal
        if (auto strNode = dynamic_cast<const StringLiteralNode*>(printNode.expression.get())) {
            return "print ( \"" + strNode->value + "\" )\n"; // Ausgabe eines String-Literals
        } else if (auto varNode = dynamic_cast<const VarNode*>(printNode.expression.get())) {
            return "print ( " + varNode->name + " )\n"; // Ausgabe einer Variablen
        }

        throw std::runtime_error("Error: Unsupported print expression");
    }

    std::string generateVarDeclarationCode(const VarDeclarationNode& varDeclNode) {
        std::string code = "var " + varDeclNode.varName + " = ";
        
        // Überprüfen, ob der Ausdruck ein StringLiteralNode ist
        if (auto strNode = dynamic_cast<const StringLiteralNode*>(varDeclNode.expression.get())) {
            code += "\"" + strNode->value + "\""; // Wert des String-Literals
        } else if (auto intNode = dynamic_cast<const IntLiteralNode*>(varDeclNode.expression.get())) {
            code += std::to_string(intNode->value); // Wert des Int-Literals
        } else if (auto varNode = dynamic_cast<const VarNode*>(varDeclNode.expression.get())) {
            code += varNode->name; // Wert einer Variablen
        }

        code += "\n"; // Zeilenumbruch hinzufügen
        return code;
    }

    std::string generateCommentCode(const CommentNode& commentNode) {
        std::string code = "//" + commentNode.comment + "\n";

        return code;
    }
};

class Interpreter {
public:
    std::unordered_map<std::string, std::string> variables;

    void interpret(const ProgramNode& programNode) {
        for (const auto& statement : programNode.statements) {
            interpretStatement(statement);
        }
    }

private:
    void interpretStatement(const std::unique_ptr<ASTNode>& statement) {
        if (auto printNode = dynamic_cast<PrintNode*>(statement.get())) {
            interpretPrintNode(*printNode);
        } else if (auto varDeclNode = dynamic_cast<VarDeclarationNode*>(statement.get())) {
            interpretVarDeclaration(*varDeclNode);
        }
    }

    void interpretVarDeclaration(const VarDeclarationNode& varDeclNode) {
        // Hier interpretierst du den Ausdruck, um den Wert zu ermitteln
        std::string value;
        if (auto strNode = dynamic_cast<StringLiteralNode*>(varDeclNode.expression.get())) {
            value = strNode->value; // Den Wert aus dem StringLiteralNode abrufen
        } else if (auto intNode = dynamic_cast<IntLiteralNode*>(varDeclNode.expression.get())) {
            value = std::to_string(intNode->value); // Den Wert aus dem StringLiteralNode abrufen
        } else if (auto varNode = dynamic_cast<VarNode*>(varDeclNode.expression.get())) {
            // Hier müsstest du sicherstellen, dass die Variable bereits existiert und ihren Wert abrufen
            if (variables.find(varNode->name) != variables.end()) {
                value = variables[varNode->name]; // Wert aus der Map abrufen
            } else {
                throw std::runtime_error("Variable not found: " + varNode->name);
            }
        }

        // Speichere den Wert in der Variablen Map
        variables[varDeclNode.varName] = value;
    }

    void interpretPrintNode(const PrintNode& printNode) {
        std::string output;

        if (auto varNode = dynamic_cast<VarNode*>(printNode.expression.get())) {
            // Hier ist der Zugriff auf die Variable zu implementieren
            if (variables.find(varNode->name) != variables.end()) {
                output = variables[varNode->name]; // Variable aus dem Speicher abrufen
            } else {
                throw std::runtime_error("Variable not found: " + varNode->name);
            }
        } else if (auto strNode = dynamic_cast<StringLiteralNode*>(printNode.expression.get())) {
            output = strNode->value; // String-Wert
        } else if (auto intNode = dynamic_cast<IntLiteralNode*>(printNode.expression.get())) {
            output = std::to_string(intNode->value); // String-Wert
        } else {
            throw std::runtime_error("Unrecognized expression in print statement");
        }

        std::cout << output << std::endl; // Ausgabe des Strings
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
        //Error::e1.printErrorMessage();
        return 1;
    }

    // Debug flags
    bool debugShowFile = false;
    bool debugShowTokens = false;
    bool debugShowAST = false;
    bool debugShowCompiled = false;

    // Operational flags
    bool compile = true;
    bool interpret = true;

    char *filename;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--show-filecontent") == 0 || strcmp(argv[i], "--filecontent") == 0 || strcmp(argv[i], "--f") == 0) {
            debugShowFile = true;
        } else if (strcmp(argv[i], "--show-tokens") == 0 || strcmp(argv[i], "--tokens") == 0 || strcmp(argv[i], "--t") == 0) {
            debugShowTokens = true;
        } else if (strcmp(argv[i], "--show-ast") == 0 || strcmp(argv[i], "--ast") == 0 || strcmp(argv[i], "--a") == 0) {
            debugShowAST = true;
        } else if (strcmp(argv[i], "--show-compiled") == 0 || strcmp(argv[i], "--compiled") == 0 || strcmp(argv[i], "--c") == 0) {
            debugShowCompiled = true;
        } else if (strcmp(argv[i], "--debug") == 0 || strcmp(argv[i], "--d") == 0) {
            debugShowTokens = true;
            debugShowAST = true;
        } else if (strcmp(argv[i], "-c") == 0) {
            compile = true;
        } else if (strcmp(argv[i], "-i") == 0) {
            interpret = true;
        } else {
            filename = argv[i];
        }
    }

    char* file_content = read_file(filename);
    if (!file_content) {
        //Error::e2.printErrorMessage();
        return 1;
    }

    std::string code(file_content);

    code += ";";

    if(debugShowFile) {
        std::cout << "File Content:\n";
        std::cout << code << "\n\n";
    }

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
                    case TokenType::INT_LITERAL:
                    std::cout << "Token: INT_LITERAL, Value: \"" << token.value << "\"\n";
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
                case TokenType::VAR:
                    std::cout << "Token: VAR, Value: " << token.value << "\n";
                    break;
                case TokenType::IDENTIFIER:
                    std::cout << "Token: IDENTIFIER, Value: " << token.value << "\n";
                    break;
                case TokenType::ASSIGNMENT:
                    std::cout << "Token: ASSIGNMENT, Value: " << token.value << "\n";
                    break;
                case TokenType::COMMENT:
                    std::cout << "Token: COMMENT, Value: " << token.value << "\n";
                    break;
                default:
                    std::cout << "Token: UNKNOWN, Value: " << token.value << "\n";
                    break;
            }
        }
        std::cout << "\n";
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
        std::cout << "AST Structure:\n";
        programNode->print();
        std::cout << "\n";
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