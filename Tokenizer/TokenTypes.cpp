#include <string>

enum class TokenType {
    // Variablen
    VAR,
    CONST,
    ASSIGNMENT,
    IDENTIFIER,

    FUNC,
    
    // Literaltoken
    STRING_LITERAL,
    INT_LITERAL,
    BOOL_LITERAL,

    // Arithmetik
    PLUS,
    MINUS,
    STAR,
    SLASH,
    
    // Symboltoken
    OPEN_PARENTHESIS,
    CLOSE_PARENTHESIS,

    COMMA,

    // Zeilen Ende
    SEMICOLON,
    NEWLINE,
    
    // Zusätzliche Token
    COMMENT,
    MULTICOMMENT,
    UNKNOWN
};

struct Token {
    TokenType type; // Typ des Tokens
    std::string value; // Wert des Tokens

    // Konstruktor für Token
    Token(TokenType t, const std::string& v) : type(t), value(v) {}
};

void printTokens(std::vector<Token> tokens) {
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
}