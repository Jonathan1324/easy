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

#include "c++\String.cpp"
#include "c++\Arithmetic.cpp"

#include "c\file_utils.h"

#include "Tokenizer\tokenizeToken.cpp"

enum class CompilerLanguages {
    Easy,
    Python,
    JavaScript,
    GW_BASIC,
    QuickBASIC,
    Fortran77,
    Fortran90
};

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

        Token arithmeticToken = tokenizeArithmetic(code, i);
        if (arithmeticToken.type != TokenType::UNKNOWN) {
            tokens.push_back(arithmeticToken);
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

class BoolLiteralNode : public ASTNode {
public:
    bool value;

    BoolLiteralNode(const bool& val) : value(val) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "BoolLiteralNode: " << value << "\n";
    }
};

class ArithmeticOperationNode : public ASTNode {
public:
    TokenType operation;

    ArithmeticOperationNode(const TokenType& val) : operation(val) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "ArithmeticOperationNode: ";

        switch (operation)
        {
        case TokenType::PLUS:
            std::cout << "PLUS - \"+\"";
            break;
        case TokenType::MINUS:
            std::cout << "MINUS - \"-\"";
            break;
        case TokenType::STAR:
            std::cout << "STAR - \"*\"";
            break;
        case TokenType::SLASH:
            std::cout << "SLASH - \"/\"";
            break;
        case TokenType::OPEN_PARENTHESIS:
            std::cout << "STAR - \"(\"";
            break;
        case TokenType::CLOSE_PARENTHESIS:
            std::cout << "SLASH - \")\"";
            break;
        default:
            break;
        }

        std::cout << "\n";
    }
};

class CommaNode : public ASTNode {
public:
    CommaNode() {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "CommaNode: ,\n";
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

class FunctionNode : public ASTNode {
public:
    std::string funcName;
    std::vector<std::unique_ptr<ASTNode>> arguments; // Der Ausdruck, der ausgegeben werden soll

    FunctionNode(std::string name, std::vector<std::unique_ptr<ASTNode>> args) : arguments(std::move(args)), funcName(name) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "FunctionNode: " << funcName << "\n";

        for (const std::unique_ptr<ASTNode>& argument : arguments) {
            argument->print(indent + 2);
        }
    }
};

// Knoten für die Variablendeklaration
class VarDeclarationNode : public ASTNode {
public:
    std::string varName; // Der Name der Variable
    std::vector<std::unique_ptr<ASTNode>> expressions;
    bool first;
    bool constant;

    VarDeclarationNode(const std::string& name, std::vector<std::unique_ptr<ASTNode>> expr, bool firstDecl, bool isConst)
        : varName(name), expressions(std::move(expr)), first(firstDecl), constant(isConst) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "VarDeclarationNode: " << varName << "\n";
        if(constant) {
            std::cout << std::string(indent + 2, ' ') << "Const: True\n";
        } else {
            std::cout << std::string(indent + 2, ' ') << "Const: False\n";
        }
        if(first) {
            std::cout << std::string(indent + 2, ' ') << "First Declaration: True\n";
        } else {
            std::cout << std::string(indent + 2, ' ') << "First Declaration: False\n";
        }
        
        for (const std::unique_ptr<ASTNode>& expression : expressions) {
            expression->print(indent + 2); // Drucke den Ausdruck
        }
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
    bool multiline;

    CommentNode(const std::string& val, const bool& multi) : comment(val, multi) {}

    void print(int indent = 0) const override {
        if(multiline) {
            std::cout << std::string(indent, ' ') << "CommentNode: *Multiple Lines*\n";
        } else {
            std::cout << std::string(indent, ' ') << "CommentNode: " << comment << "\n";
        }
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
                programNode->statements.push_back(std::make_unique<CommentNode>(currentToken().value, false));
                advance();
            } else if(currentToken().type == TokenType::MULTICOMMENT) {
                programNode->statements.push_back(std::make_unique<CommentNode>(currentToken().value, true));
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
            return nullptr;
        }

        if(currentToken().type == TokenType::COMMENT || currentToken().type == TokenType::MULTICOMMENT) {
            return nullptr;
        }

        /*
        if (currentToken().type == TokenType::PRINT) {
            return parsePrintStatement();
        }
        */

        if (currentToken().type == TokenType::VAR) {
            return parseIdentifier(true, false);
        }

        if(currentToken().type == TokenType::CONST) {
            bool isVar = false;

            advance();

            if(currentToken().type != TokenType::FUNC) {
                isVar = true;
            }

            tokens[current--];

            return parseIdentifier(isVar, true);
        }

        if (currentToken().type == TokenType::IDENTIFIER) {
            return parseIdentifier(false, false);
        }

        throw std::runtime_error("Unrecognized statement");
    }

    std::unique_ptr<ASTNode> parseIdentifier(bool var, bool isConst) {
        if(var) {
            advance();
        }

        advance();

        if(currentToken().type == TokenType::ASSIGNMENT || var) {
            tokens[current--];
            return parseVarDeclaration(var, isConst);
        } else {
            tokens[current--];
            return parseFunction(var, isConst);
        }
    }

    std::unique_ptr<ASTNode> parseFunction(bool var, bool isConst) {
        std::vector<std::unique_ptr<ASTNode>> expressions = std::vector<std::unique_ptr<ASTNode>>();

        std::string name = currentToken().value;

        advance();
        advance();

        bool isFinished = false;

        int brackets = 0;

        while(!isFinished && brackets >= 0) {
            if(currentToken().type == TokenType::OPEN_PARENTHESIS) {
                brackets += 1;
            } else if(currentToken().type == TokenType::CLOSE_PARENTHESIS) {
                brackets -= 1;
            }

            if(currentToken().type == TokenType::COMMA) {
                expressions.push_back(std::make_unique<CommaNode>());
                advance();
            } else {
                expressions.push_back(parseExpression());
            }

            if(brackets < 0) {
                expressions.pop_back();
                isFinished = true;
            }
        }

        return std::make_unique<FunctionNode>(name, std::move(expressions));
    }

    std::unique_ptr<ASTNode> parseVarDeclaration(bool var, bool isConst) {
        if (currentToken().type != TokenType::IDENTIFIER) {
            throw std::runtime_error("Expected variable name after 'var'");
        }
        std::string varName = currentToken().value;
        advance(); // Variable Name überspringen

        if (currentToken().type != TokenType::ASSIGNMENT) {
            throw std::runtime_error("Expected '=' after variable name");
        }
        advance(); // '=' Token überspringen

        std::vector<std::unique_ptr<ASTNode>> expressions = std::vector<std::unique_ptr<ASTNode>>();

        int brackets = 0;

        while(currentToken().type != TokenType::SEMICOLON && currentToken().type != TokenType::NEWLINE && brackets >= 0) {
            if(currentToken().type == TokenType::OPEN_PARENTHESIS) {
                brackets += 1;
            } else if(currentToken().type == TokenType::CLOSE_PARENTHESIS) {
                brackets -= 1;
            }

            std::unique_ptr<ASTNode> expression = parseExpression();

            expressions.push_back(std::move(expression));
            if(currentToken().type == TokenType::SEMICOLON || currentToken().type == TokenType::NEWLINE || brackets < 0) {
                break;
            }
        }

        return std::make_unique<VarDeclarationNode>(varName, std::move(expressions), var, isConst);
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
        } else if (currentToken().type == TokenType::BOOL_LITERAL) {
            if(currentToken().value == "true") {
                expression = std::make_unique<StringLiteralNode>("True");
            } else {
                expression = std::make_unique<StringLiteralNode>("False");
            }
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
            advance();

            return std::make_unique<IntLiteralNode>(intValue);

        } else if (currentToken().type == TokenType::BOOL_LITERAL) {
            std::string stringValue = currentToken().value;
            bool boolValue = false;
            if(stringValue == "true") {
                boolValue = true;
            }
            advance();
            return std::make_unique<BoolLiteralNode>(boolValue);
        } else if (currentToken().type == TokenType::PLUS || currentToken().type == TokenType::MINUS || currentToken().type == TokenType::STAR || currentToken().type == TokenType::SLASH || currentToken().type == TokenType::OPEN_PARENTHESIS || currentToken().type == TokenType::CLOSE_PARENTHESIS) {
            std::string stringValue = currentToken().value;
            TokenType type = currentToken().type;
            advance();
            return std::make_unique<ArithmeticOperationNode>(type);
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
        if (!(varDeclNode.expressions.empty())) {

            for (size_t i = 0; i < varDeclNode.expressions.size(); ++i) {
                const std::unique_ptr<ASTNode>& expression = varDeclNode.expressions[i];
                analyzeExpression(*expression);
            }
        } else {
            throw std::runtime_error("Error: Variable declaration must have an expression.");
        }
    }

    void analyzeExpression(const ASTNode& expression) {
        if (dynamic_cast<const StringLiteralNode*>(&expression)) {
            // Hier kannst du überprüfen, ob es ein StringLiteral ist
        } else if (dynamic_cast<const IntLiteralNode*>(&expression)) {
            // Hier kannst du überprüfen, ob es ein StringLiteral ist
        } else if (dynamic_cast<const BoolLiteralNode*>(&expression)) {
            // Hier kannst du überprüfen, ob es ein StringLiteral ist
        } else if (dynamic_cast<const ArithmeticOperationNode*>(&expression)) {
            // Hier kannst du überprüfen, ob es ein StringLiteral ist
        } else if (const auto* varNode = dynamic_cast<const VarNode*>(&expression)) {
            // Hier kannst du überprüfen, ob es eine gültige Variable ist
            // Zum Beispiel, ob die Variable deklariert wurde
            
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

    std::string generateCode(const CompilerLanguages language) {
        switch (language)
        {
        case CompilerLanguages::Easy:
            return Easy(programNode).generateCode();
        case CompilerLanguages::Python:
            return Python(programNode).generateCode();
        case CompilerLanguages::JavaScript:
            return JavaScript(programNode).generateCode();
        case CompilerLanguages::GW_BASIC:
            return "";
        case CompilerLanguages::QuickBASIC:
            return "";
        case CompilerLanguages::Fortran77:
            return "";
        case CompilerLanguages::Fortran90:
            return "";
        default:
            return "";
        }
    }

private:
    std::shared_ptr<ProgramNode> programNode;

    class Easy {
    public:
        Easy(const std::shared_ptr<ProgramNode>& programNode) : programNode(programNode) {}

        std::string generateCode() {
            std::string code = "";
            for (const auto& statement : programNode->statements) {
                if (auto functionNode = dynamic_cast<FunctionNode*>(statement.get())) {
                    code += generateFunctionCode(*functionNode);
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

        std::string newLine() {
            return "\n";
        }

        std::string generateFunctionCode(const FunctionNode& functionNode) {
            if (functionNode.funcName == "print") {
                return generatePrintCode(functionNode);
            }

            throw std::runtime_error("Error: Function not defined - " + functionNode.funcName);
        }

        std::string generatePrintCode(const FunctionNode& printNode) {
            if (printNode.arguments.empty()) {
                throw std::runtime_error("Error: print function requires at least one argument");
            }

            std::string code = "print ( ";

            bool onlyNumber = true;

            for (size_t i = 0; i < printNode.arguments.size(); ++i) {
                if (const auto* strNode = dynamic_cast<const StringLiteralNode*>(printNode.arguments.at(i).get())) {
                    code += "\"" + strNode->value + "\"";
                } else if (const auto* intNode = dynamic_cast<const IntLiteralNode*>(printNode.arguments.at(i).get())) {
                    code += std::to_string(intNode->value);
                } else if (auto boolNode = dynamic_cast<BoolLiteralNode*>(printNode.arguments[i].get())) {
                    onlyNumber = false;
                    if(boolNode->value == true) {
                        code += "True";
                    } else {
                        code += "False";
                    }
                } else if (const auto* varNode = dynamic_cast<const VarNode*>(printNode.arguments.at(i).get())) {
                    code += varNode->name;
                } else if (auto arithmeticOperationNode = dynamic_cast<const ArithmeticOperationNode*>(printNode.arguments.at(i).get())) {
                    if(arithmeticOperationNode->operation == TokenType::PLUS) {
                        code += " + ";
                    } else if(arithmeticOperationNode->operation == TokenType::MINUS) {
                        code += " - ";
                    } else if(arithmeticOperationNode->operation == TokenType::STAR) {
                        code += " * ";
                    } else if(arithmeticOperationNode->operation == TokenType::SLASH) {
                        code += " / ";
                    }  else if(arithmeticOperationNode->operation == TokenType::OPEN_PARENTHESIS) {
                        code += " ( ";
                    } else if(arithmeticOperationNode->operation == TokenType::CLOSE_PARENTHESIS) {
                        code += " ) ";
                    }
                } else {

                }
            }

            code += " )" + newLine();

            return code;
        }

        std::string generateVarDeclarationCode(const VarDeclarationNode& varDeclNode) {
            std::string code;
            if(varDeclNode.constant) {
                if(varDeclNode.first) {
                    code = "const " + varDeclNode.varName + " = ";
                } else {
                    throw std::runtime_error("Constants can't be changed: " + varDeclNode.varName);
                }
            } else {
                if(varDeclNode.first) {
                    code = "var " + varDeclNode.varName + " = ";
                } else {
                    code = varDeclNode.varName + " = ";
                }
            }

            for (size_t i = 0; i < varDeclNode.expressions.size(); ++i) {
                const std::unique_ptr<ASTNode>& expression = varDeclNode.expressions[i];

                if (auto strNode = dynamic_cast<const StringLiteralNode*>(expression.get())) {
                    code += "\"" + strNode->value + "\"";
                } else if (auto intNode = dynamic_cast<const IntLiteralNode*>(expression.get())) {
                    code += std::to_string(intNode->value);
                } else if (auto boolNode = dynamic_cast<const BoolLiteralNode*>(expression.get())) {
                    if(boolNode->value == true) {
                        code += "True";
                    } else {
                        code += "False";
                    }
                } else if (auto varNode = dynamic_cast<const VarNode*>(expression.get())) {
                    code += varNode->name;
                } else if (auto arithmeticOperationNode = dynamic_cast<const ArithmeticOperationNode*>(expression.get())) {
                    if(arithmeticOperationNode->operation == TokenType::PLUS) {
                        code += " + ";
                    } else if(arithmeticOperationNode->operation == TokenType::MINUS) {
                        code += " - ";
                    } else if(arithmeticOperationNode->operation == TokenType::STAR) {
                        code += " * ";
                    } else if(arithmeticOperationNode->operation == TokenType::SLASH) {
                        code += " / ";
                    } else if(arithmeticOperationNode->operation == TokenType::OPEN_PARENTHESIS) {
                        code += " ( ";
                    } else if(arithmeticOperationNode->operation == TokenType::CLOSE_PARENTHESIS) {
                        code += " ) ";
                    }
                }
            }

            /*
            if (auto strNode = dynamic_cast<const StringLiteralNode*>(varDeclNode.expression.get())) {
                code += "\"" + strNode->value + "\"";
            } else if (auto intNode = dynamic_cast<const IntLiteralNode*>(varDeclNode.expression.get())) {
                code += std::to_string(intNode->value);
            } else if (auto boolNode = dynamic_cast<const BoolLiteralNode*>(varDeclNode.expression.get())) {
                if(boolNode->value == true) {
                    code += "True";
                } else {
                    code += "False";
                }
            } else if (auto varNode = dynamic_cast<const VarNode*>(varDeclNode.expression.get())) {
                code += varNode->name;
            }
            */

            code += newLine();
            return code;
        }

        std::string generateCommentCode(const CommentNode& commentNode) {
            if(commentNode.multiline) {
                if(commentNode.comment[0] != ' ') {
                    return "/* " + commentNode.comment + newLine() + "*/" + newLine();
                } else {
                    return "/*" + commentNode.comment + newLine() + "*/" + newLine();
                }
            } else {
                if(commentNode.comment[0] != ' ') {
                    return "// " + commentNode.comment + newLine();
                } else {
                    return "//" + commentNode.comment + newLine();
                }
            }
        }
    };

    class Python {
    public:
        Python(const std::shared_ptr<ProgramNode>& programNode) : programNode(programNode) {}

        std::string generateCode() {
            std::string code = "";
            for (const auto& statement : programNode->statements) {
                if (auto functionNode = dynamic_cast<FunctionNode*>(statement.get())) {
                    code += generateFunctionCode(*functionNode);
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

        std::string newLine() {
            return "\n";
        }

        std::string generateFunctionCode(const FunctionNode& functionNode) {
            if (functionNode.funcName == "print") {
                return generatePrintCode(functionNode);
            }

            throw std::runtime_error("Error: Function not defined - " + functionNode.funcName);
        }

        std::string generatePrintCode(const FunctionNode& printNode) {
            if (printNode.arguments.empty()) {
                throw std::runtime_error("Error: print function requires at least one argument");
            }

            std::string code = "print ( ";

            bool onlyNumber = true;

            for (size_t i = 0; i < printNode.arguments.size(); ++i) {
                if (const auto* strNode = dynamic_cast<const StringLiteralNode*>(printNode.arguments.at(i).get())) {
                    code += "\"" + strNode->value + "\"";
                } else if (const auto* intNode = dynamic_cast<const IntLiteralNode*>(printNode.arguments.at(i).get())) {
                    code += std::to_string(intNode->value);
                } else if (auto boolNode = dynamic_cast<BoolLiteralNode*>(printNode.arguments[i].get())) {
                    onlyNumber = false;
                    if(boolNode->value == true) {
                        code += "True";
                    } else {
                        code += "False";
                    }
                } else if (auto arithmeticOperationNode = dynamic_cast<const ArithmeticOperationNode*>(printNode.arguments.at(i).get())) {
                    if(arithmeticOperationNode->operation == TokenType::PLUS) {
                        code += " + ";
                    } else if(arithmeticOperationNode->operation == TokenType::MINUS) {
                        code += " - ";
                    } else if(arithmeticOperationNode->operation == TokenType::STAR) {
                        code += " * ";
                    } else if(arithmeticOperationNode->operation == TokenType::SLASH) {
                        code += " / ";
                    }  else if(arithmeticOperationNode->operation == TokenType::OPEN_PARENTHESIS) {
                        code += " ( ";
                    } else if(arithmeticOperationNode->operation == TokenType::CLOSE_PARENTHESIS) {
                        code += " ) ";
                    }
                } else if (const auto* varNode = dynamic_cast<const VarNode*>(printNode.arguments.at(i).get())) {
                    code += varNode->name;
                } else {

                }
            }

            code += " )" + newLine();

            return code;
        }

        std::string generateVarDeclarationCode(const VarDeclarationNode& varDeclNode) {
            std::string code = varDeclNode.varName + " = ";

            /*
            if (auto strNode = dynamic_cast<const StringLiteralNode*>(varDeclNode.expression.get())) {
                code += "\"" + strNode->value + "\"";
            } else if (auto intNode = dynamic_cast<const IntLiteralNode*>(varDeclNode.expression.get())) {
                code += std::to_string(intNode->value);
            } else if (auto boolNode = dynamic_cast<const BoolLiteralNode*>(varDeclNode.expression.get())) {
                if(boolNode->value == true) {
                    code += "True";
                } else {
                    code += "False";
                }
            } else if (auto varNode = dynamic_cast<const VarNode*>(varDeclNode.expression.get())) {
                code += varNode->name;
            }
            */
            
            for (size_t i = 0; i < varDeclNode.expressions.size(); ++i) {
                const std::unique_ptr<ASTNode>& expression = varDeclNode.expressions[i];

                if (auto strNode = dynamic_cast<const StringLiteralNode*>(varDeclNode.expressions[i].get())) {
                    code += "\"" + strNode->value + "\"";
                } else if (auto intNode = dynamic_cast<const IntLiteralNode*>(varDeclNode.expressions[i].get())) {
                    code += std::to_string(intNode->value);
                } else if (auto boolNode = dynamic_cast<const BoolLiteralNode*>(varDeclNode.expressions[i].get())) {
                    if(boolNode->value == true) {
                        code += "True";
                    } else {
                        code += "False";
                    }
                } else if (auto varNode = dynamic_cast<const VarNode*>(varDeclNode.expressions[i].get())) {
                    code += varNode->name;
                } else if (auto arithmeticOperationNode = dynamic_cast<const ArithmeticOperationNode*>(expression.get())) {
                    if(arithmeticOperationNode->operation == TokenType::PLUS) {
                        code += " + ";
                    } else if(arithmeticOperationNode->operation == TokenType::MINUS) {
                        code += " - ";
                    } else if(arithmeticOperationNode->operation == TokenType::STAR) {
                        code += " * ";
                    } else if(arithmeticOperationNode->operation == TokenType::SLASH) {
                        code += " / ";
                    } else if(arithmeticOperationNode->operation == TokenType::OPEN_PARENTHESIS) {
                        code += " ( ";
                    } else if(arithmeticOperationNode->operation == TokenType::CLOSE_PARENTHESIS) {
                        code += " ) ";
                    }
                }
            }

            if(varDeclNode.constant) {
                code += "        # Constant";
            }

            code += newLine();
            return code;
        }

        std::string generateCommentCode(const CommentNode& commentNode) {
            if(commentNode.multiline) {
                if(commentNode.comment[0] != ' ') {
                    return "''' " + commentNode.comment + newLine() + "'''" + newLine();
                } else {
                    return "'''" + commentNode.comment + newLine() + "'''" + newLine();
                }
            } else {
                if(commentNode.comment[0] != ' ') {
                    return "# " + commentNode.comment + newLine();
                } else {
                    return "#" + commentNode.comment + newLine();
                }
            }
        }
    };

    class JavaScript {
    public:
        JavaScript(const std::shared_ptr<ProgramNode>& programNode) : programNode(programNode) {}

        std::string generateCode() {
            std::string code = "";
            for (const auto& statement : programNode->statements) {
                if (auto functionNode = dynamic_cast<FunctionNode*>(statement.get())) {
                    code += generateFunctionCode(*functionNode);
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

        std::string newLine() {
            return ";\n";
        }

        std::string generateFunctionCode(const FunctionNode& functionNode) {
            if (functionNode.funcName == "print") {
                return generatePrintCode(functionNode);
            }

            throw std::runtime_error("Error: Function not defined - " + functionNode.funcName);
        }

        std::string generatePrintCode(const FunctionNode& printNode) {
            if (printNode.arguments.empty()) {
                throw std::runtime_error("Error: print function requires at least one argument");
            }

            std::string code = "console.log ( ";

            bool onlyNumber = true;

            for (size_t i = 0; i < printNode.arguments.size(); ++i) {
                if (const auto* strNode = dynamic_cast<const StringLiteralNode*>(printNode.arguments.at(i).get())) {
                    code += "\"" + strNode->value + "\"";
                } else if (const auto* intNode = dynamic_cast<const IntLiteralNode*>(printNode.arguments.at(i).get())) {
                    code += std::to_string(intNode->value);
                } else if (auto arithmeticOperationNode = dynamic_cast<const ArithmeticOperationNode*>(printNode.arguments.at(i).get())) {
                    if(arithmeticOperationNode->operation == TokenType::PLUS) {
                        code += " + ";
                    } else if(arithmeticOperationNode->operation == TokenType::MINUS) {
                        code += " - ";
                    } else if(arithmeticOperationNode->operation == TokenType::STAR) {
                        code += " * ";
                    } else if(arithmeticOperationNode->operation == TokenType::SLASH) {
                        code += " / ";
                    }  else if(arithmeticOperationNode->operation == TokenType::OPEN_PARENTHESIS) {
                        code += " ( ";
                    } else if(arithmeticOperationNode->operation == TokenType::CLOSE_PARENTHESIS) {
                        code += " ) ";
                    }
                } else if (auto boolNode = dynamic_cast<BoolLiteralNode*>(printNode.arguments[i].get())) {
                    onlyNumber = false;
                    if(boolNode->value == true) {
                        code += "true";
                    } else {
                        code += "false";
                    }
                } else if (const auto* varNode = dynamic_cast<const VarNode*>(printNode.arguments.at(i).get())) {
                    code += varNode->name;
                } else {

                }
            }

            code += " )" + newLine();

            return code;
        }

        std::string generateVarDeclarationCode(const VarDeclarationNode& varDeclNode) {
            std::string code;
            
            if(varDeclNode.constant) {
                if(varDeclNode.first) {
                    code = "const " + varDeclNode.varName + " = ";
                } else {
                    throw std::runtime_error("Constants can't be changed: " + varDeclNode.varName);
                }
            } else {
                if(varDeclNode.first) {
                    code = "let " + varDeclNode.varName + " = ";
                } else {
                    code = varDeclNode.varName + " = ";
                }
            }

            for (size_t i = 0; i < varDeclNode.expressions.size(); ++i) {
                const std::unique_ptr<ASTNode>& expression = varDeclNode.expressions[i];

                if (auto strNode = dynamic_cast<const StringLiteralNode*>(varDeclNode.expressions[i].get())) {
                    code += "\"" + strNode->value + "\"";
                } else if (auto intNode = dynamic_cast<const IntLiteralNode*>(varDeclNode.expressions[i].get())) {
                    code += std::to_string(intNode->value);
                } else if (auto boolNode = dynamic_cast<const BoolLiteralNode*>(varDeclNode.expressions[i].get())) {
                    if(boolNode->value == true) {
                        code += "true";
                    } else {
                        code += "false";
                    }
                } else if (auto varNode = dynamic_cast<const VarNode*>(varDeclNode.expressions[i].get())) {
                    code += varNode->name;
                } else if (auto arithmeticOperationNode = dynamic_cast<const ArithmeticOperationNode*>(expression.get())) {
                    if(arithmeticOperationNode->operation == TokenType::PLUS) {
                        code += " + ";
                    } else if(arithmeticOperationNode->operation == TokenType::MINUS) {
                        code += " - ";
                    } else if(arithmeticOperationNode->operation == TokenType::STAR) {
                        code += " * ";
                    } else if(arithmeticOperationNode->operation == TokenType::SLASH) {
                        code += " / ";
                    } else if(arithmeticOperationNode->operation == TokenType::OPEN_PARENTHESIS) {
                        code += " ( ";
                    } else if(arithmeticOperationNode->operation == TokenType::CLOSE_PARENTHESIS) {
                        code += " ) ";
                    }
                }
            }

            /*
            if (auto strNode = dynamic_cast<const StringLiteralNode*>(varDeclNode.expression.get())) {
                code += "\"" + strNode->value + "\"";
            } else if (auto intNode = dynamic_cast<const IntLiteralNode*>(varDeclNode.expression.get())) {
                code += std::to_string(intNode->value);
            } else if (auto boolNode = dynamic_cast<const BoolLiteralNode*>(varDeclNode.expression.get())) {
                if(boolNode->value == true) {
                    code += "true";
                } else {
                    code += "false";
                }
            } else if (auto varNode = dynamic_cast<const VarNode*>(varDeclNode.expression.get())) {
                code += varNode->name;
            }
            */

            code += newLine();
            return code;
        }

        std::string generateCommentCode(const CommentNode& commentNode) {
            if(commentNode.multiline) {
                if(commentNode.comment[0] != ' ') {
                    return "/* " + commentNode.comment + newLine() + "*/" + newLine();
                } else {
                    return "/*" + commentNode.comment + newLine() + "*/" + newLine();
                }
            } else {
                if(commentNode.comment[0] != ' ') {
                    return "/* " + commentNode.comment + newLine() + "*/" + newLine();
                } else {
                    return "/*" + commentNode.comment + newLine() + "*/" + newLine();
                }
            }
        }
    };
};

class Interpreter {
public:
    std::unordered_map<std::string, std::string> variables;
    std::unordered_map<std::string, std::string> constants;

    void interpret(const ProgramNode& programNode) {
        for (const auto& statement : programNode.statements) {
            interpretStatement(statement);
        }
    }

private:
    void interpretStatement(const std::unique_ptr<ASTNode>& statement) {
        if (auto functionNode = dynamic_cast<FunctionNode*>(statement.get())) {
            interpretFunctionNode(*functionNode);
        } else if (auto varDeclNode = dynamic_cast<VarDeclarationNode*>(statement.get())) {
            interpretVarDeclaration(*varDeclNode);
        }
    }

    void interpretVarDeclaration(const VarDeclarationNode& varDeclNode) {
        // Hier interpretierst du den Ausdruck, um den Wert zu ermitteln
        std::string value;

        bool onlyNumber = true;

        for (size_t i = 0; i < varDeclNode.expressions.size(); ++i) {
            if (auto strNode = dynamic_cast<StringLiteralNode*>(varDeclNode.expressions[i].get())) {
                onlyNumber = false;
                value += strNode->value;
            } else if (auto intNode = dynamic_cast<IntLiteralNode*>(varDeclNode.expressions[i].get())) {
                value += std::to_string(intNode->value);
            }  else if (auto boolNode = dynamic_cast<BoolLiteralNode*>(varDeclNode.expressions[i].get())) {
                onlyNumber = false;
                if(boolNode->value == true) {
                    value += "True";
                } else {
                    value += "False";
                }
            } else if (auto arithmeticOperationNode = dynamic_cast<const ArithmeticOperationNode*>(varDeclNode.expressions[i].get())) {
                    if(arithmeticOperationNode->operation == TokenType::PLUS) {
                        value += "+";
                    } else if(arithmeticOperationNode->operation == TokenType::MINUS) {
                        value += "-";
                    } else if(arithmeticOperationNode->operation == TokenType::STAR) {
                        value += "*";
                    } else if(arithmeticOperationNode->operation == TokenType::SLASH) {
                        value += "/";
                    }  else if(arithmeticOperationNode->operation == TokenType::OPEN_PARENTHESIS) {
                        value += "(";
                    } else if(arithmeticOperationNode->operation == TokenType::CLOSE_PARENTHESIS) {
                        value += ")";
                    }
            } else if (auto varNode = dynamic_cast<VarNode*>(varDeclNode.expressions[i].get())) {
                // Hier müsstest du sicherstellen, dass die Variable bereits existiert und ihren Wert abrufen
                if (variables.find(varNode->name) != variables.end()) {
                    value += variables[varNode->name]; // Wert aus der Map abrufen
                } else {
                    throw std::runtime_error("Variable not found: " + varNode->name);
                }
            }
        }

        if(onlyNumber) {
            value = evaluate(value);
        }

        if(variables.find(varDeclNode.varName) != variables.end()) {
            if(varDeclNode.first) {
                throw std::runtime_error("Variable has already been defined: " + varDeclNode.varName);
            }

            if(constants.find(varDeclNode.varName) != variables.end() ) {
                throw std::runtime_error("Constants can't be changed: " + varDeclNode.varName);
            }
        } else {
            if(!varDeclNode.first) {
                throw std::runtime_error("Variable hasn't been defined: " + varDeclNode.varName);
            }
        }

        // Speichere den Wert in der Variablen Map
        variables[varDeclNode.varName] = value;

        if(varDeclNode.constant) {
            constants[varDeclNode.varName] = varDeclNode.varName;
        }
    }

    void interpretFunctionNode(const FunctionNode& functionNode) {
        if(functionNode.funcName == "print") {
            interpretPrintNode(functionNode);
        }
    }

    void interpretPrintNode(const FunctionNode& printNode) {
        std::string output;

        bool onlyNumber = true;

        for (size_t i = 0; i < printNode.arguments.size(); ++i) {
            if (auto strNode = dynamic_cast<StringLiteralNode*>(printNode.arguments[i].get())) {
                onlyNumber = false;
                output += strNode->value;
            } else if (auto intNode = dynamic_cast<IntLiteralNode*>(printNode.arguments[i].get())) {
                output += std::to_string(intNode->value);
            } else if (auto boolNode = dynamic_cast<BoolLiteralNode*>(printNode.arguments[i].get())) {
                onlyNumber = false;
                if(boolNode->value == true) {
                    output += "True";
                } else {
                    output += "False";
                }
            } else if (auto arithmeticOperationNode = dynamic_cast<const ArithmeticOperationNode*>(printNode.arguments[i].get())) {
                    if(arithmeticOperationNode->operation == TokenType::PLUS) {
                        output += "+";
                    } else if(arithmeticOperationNode->operation == TokenType::MINUS) {
                        output += "-";
                    } else if(arithmeticOperationNode->operation == TokenType::STAR) {
                        output += "*";
                    } else if(arithmeticOperationNode->operation == TokenType::SLASH) {
                        output += "/";
                    }  else if(arithmeticOperationNode->operation == TokenType::OPEN_PARENTHESIS) {
                        output += "(";
                    } else if(arithmeticOperationNode->operation == TokenType::CLOSE_PARENTHESIS) {
                        output += ")";
                    }
            } else if (auto varNode = dynamic_cast<VarNode*>(printNode.arguments[i].get())) {
                // Hier müsstest du sicherstellen, dass die Variable bereits existiert und ihren Wert abrufen
                if (variables.find(varNode->name) != variables.end()) {

                    onlyNumber = false;

                    if(isNumber(variables[varNode->name])) {
                        onlyNumber = true;
                    }

                    output += variables[varNode->name]; // Wert aus der Map abrufen
                } else {
                    throw std::runtime_error("Variable not found: " + varNode->name);
                }
            }
        }

        if(onlyNumber) {
            output = evaluate(output);
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

    // Compiler Languages
    bool easy = true;
    bool python = false;
    bool javascript = false;
    bool GW_BASIC = false;
    bool QuickBASIC = false;
    bool Fortran77 = false;
    bool Fortran90 = false;

    char *filename;

    std::string outputDirectory = "./a";

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--show-filecontent") == 0 || strcmp(argv[i], "--filecontent") == 0 || strcmp(argv[i], "--fc") == 0) {
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
        } else if (strcmp(argv[i], "--eas") == 0 || strcmp(argv[i], "--easy") == 0) {
            easy = true;
        } else if (strcmp(argv[i], "--py") == 0 || strcmp(argv[i], "--python") == 0) {
            python = true;
        } else if (strcmp(argv[i], "--js") == 0 || strcmp(argv[i], "--javascript") == 0) {
            javascript = true;
        } else if (strcmp(argv[i], "--f77") == 0 || strcmp(argv[i], "--fortran-77") == 0 || strcmp(argv[i], "--fortran77") == 0) {
            Fortran77 = true;
        } else if (strcmp(argv[i], "--f90") == 0 || strcmp(argv[i], "--fortran-90") == 0 || strcmp(argv[i], "--fortran90") == 0) {
            Fortran90 = true;
        } else if (strcmp(argv[i], "--f") == 0 || strcmp(argv[i], "--fortran") == 0) {
            Fortran77 = true;
            Fortran90 = true;
        } else if (strcmp(argv[i], "--gw-b") == 0 || strcmp(argv[i], "--gw-basic") == 0 || strcmp(argv[i], "--gwbasic") == 0) {
            Fortran77 = true;
        } else if (strcmp(argv[i], "--qb") == 0 || strcmp(argv[i], "--quick-basic") == 0 || strcmp(argv[i], "--quickbasic") == 0) {
            Fortran90 = true;
        } else if (strcmp(argv[i], "--b") == 0 || strcmp(argv[i], "--basic") == 0) {
            GW_BASIC = true;
            QuickBASIC = true;
        } else if (strcmp(argv[i], "--c-to-all") == 0 || strcmp(argv[i], "--compile-to-all") == 0) {
            easy = true;
            python = true;
            javascript = true;
            GW_BASIC = true;
            QuickBASIC = true;
            Fortran77 = true;
            Fortran90 = true;
        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "-output") == 0) {
            outputDirectory = argv[i + 1];
            ++i;
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
                case TokenType::STRING_LITERAL:
                    std::cout << "Token: STRING_LITERAL, Value: \"" << token.value << "\"\n";
                    break;
                case TokenType::INT_LITERAL:
                    std::cout << "Token: INT_LITERAL, Value: " << token.value << "\n";
                    break;
                case TokenType::BOOL_LITERAL:
                    std::cout << "Token: BOOL_LITERAL, Value: " << token.value << "\n";
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
                case TokenType::CONST:
                    std::cout << "Token: CONST, Value: " << token.value << "\n";
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
                case TokenType::PLUS:
                    std::cout << "Token: PLUS, Value: " << token.value << "\n";
                    break;
                case TokenType::MINUS:
                    std::cout << "Token: MINUS, Value: " << token.value << "\n";
                    break;
                case TokenType::STAR:
                    std::cout << "Token: STAR, Value: " << token.value << "\n";
                    break;
                case TokenType::SLASH:
                    std::cout << "Token: SLASH, Value: " << token.value << "\n";
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
        if(easy) {
            std::string compiled = compiler.generateCode(CompilerLanguages::Easy);
            if(compiled.empty()) {
                std::cerr << "Compiled Code is empty!!!";
            }

            writeToFile(outputDirectory + ".easy", compiled);

            if(debugShowCompiled) {
                std::cout << "\nCompiled Code to Easy:\n";
                std::cout << compiled;
            }
        }

        if(python) {
            std::string compiled = compiler.generateCode(CompilerLanguages::Python);
            if(compiled.empty()) {
                std::cerr << "Compiled Code is empty!!!";
            }

            writeToFile(outputDirectory + ".py", compiled);

            if(debugShowCompiled) {
                std::cout << "\nCompiled Code to Python:\n";
                std::cout << compiled;
            }
        }

        if(javascript) {
            std::string compiled = compiler.generateCode(CompilerLanguages::JavaScript);
            if(compiled.empty()) {
                std::cerr << "Compiled Code is empty!!!";
            }

            writeToFile(outputDirectory + ".js", compiled);

            if(debugShowCompiled) {
                std::cout << "\nCompiled Code to JavaScript:\n";
                std::cout << compiled;
            }
        }

        /*
        if(GW_BASIC) {
            std::string compiled = compiler.generateCode(CompilerLanguages::GW_BASIC);
            if(compiled.empty()) {
                std::cerr << "Compiled Code is empty!!!";
            }

            writeToFile(outputDirectory + ".bas", compiled);

            if(debugShowCompiled) {
                std::cout << "\nCompiled Code to GW-BASIC:\n";
                std::cout << compiled;
            }
        }

        if(QuickBASIC) {
            std::string compiled = compiler.generateCode(CompilerLanguages::QuickBASIC);
            if(compiled.empty()) {
                std::cerr << "Compiled Code is empty!!!";
            }

            writeToFile(outputDirectory + ".quick.bas", compiled);

            if(debugShowCompiled) {
                std::cout << "\nCompiled Code to QuickBASIC:\n";
                std::cout << compiled;
            }
        }

        if(Fortran77) {
            std::string compiled = compiler.generateCode(CompilerLanguages::Fortran77);
            if(compiled.empty()) {
                std::cerr << "Compiled Code is empty!!!";
            }

            writeToFile(outputDirectory + ".f", compiled);

            if(debugShowCompiled) {
                std::cout << "\nCompiled Code to Fortran 77:\n";
                std::cout << compiled;
            }
        }

        if(Fortran90) {
            std::string compiled = compiler.generateCode(CompilerLanguages::Fortran90);
            if(compiled.empty()) {
                std::cerr << "Compiled Code is empty!!!";
            }

            writeToFile(outputDirectory + ".f90", compiled);

            if(debugShowCompiled) {
                std::cout << "\nCompiled Code to Fortran 90:\n";
                std::cout << compiled;
            }
        }
        */
    }

    return 0;
}