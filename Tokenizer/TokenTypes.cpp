#include <string>

enum class TokenType {
    // Variables and functions
    VAR,
    CONST,
    ASSIGNMENT,
    IDENTIFIER,
    FUNC,

    // Literal tokens
    STRING_LITERAL,
    INT_LITERAL,
    BOOL_LITERAL,

    // Arithmetic
    PLUS,
    MINUS,
    STAR,
    SLASH,

    // Logical operators
    AND,
    OR,
    NOT,

    // Comparison operators
    EQUALS,
    NOT_EQUALS,
    GREATER_THAN,
    LESS_THAN,
    GREATER_OR_EQUAL,
    LESS_OR_EQUAL,

    // Control structures
    IF,
    ELSE,
    WHILE,                  // NOT USED
    FOR,                    // NOT USED
    RETURN,                 // NOT USED

    // Blocks and symbols
    OPEN_PARENTHESIS,
    CLOSE_PARENTHESIS,
    OPEN_BRACE,
    CLOSE_BRACE,
    COMMA,

    // Line end
    SEMICOLON,
    NEWLINE,

    // Comments
    COMMENT,
    MULTICOMMENT,

    // Unknown token
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;

    Token(TokenType t, const std::string& v) : type(t), value(v) {}
};

std::string TokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::INT_LITERAL: return "INT_LITERAL";
        case TokenType::BOOL_LITERAL: return "BOOL_LITERAL";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::VAR: return "VAR";
        case TokenType::CONST: return "CONST";
        case TokenType::ASSIGNMENT: return "ASSIGNMENT";
        case TokenType::COMMENT: return "COMMENT";
        case TokenType::MULTICOMMENT: return "MULTICOMMENT";
        case TokenType::OPEN_PARENTHESIS: return "OPEN_PARENTHESIS";
        case TokenType::CLOSE_PARENTHESIS: return "CLOSE_PARENTHESIS";
        case TokenType::OPEN_BRACE: return "OPEN_BRACE";
        case TokenType::CLOSE_BRACE: return "CLOSE_BRACE";
        case TokenType::COMMA: return "COMMA";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::SLASH: return "SLASH";
        case TokenType::AND: return "AND";
        case TokenType::OR: return "OR";
        case TokenType::NOT: return "NOT";
        case TokenType::EQUALS: return "EQUALS";
        case TokenType::NOT_EQUALS: return "NOT_EQUALS";
        case TokenType::GREATER_THAN: return "GREATER_THAN";
        case TokenType::LESS_THAN: return "LESS_THAN";
        case TokenType::GREATER_OR_EQUAL: return "GREATER_OR_EQUAL";
        case TokenType::LESS_OR_EQUAL: return "LESS_OR_EQUAL";
        case TokenType::IF: return "IF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::WHILE: return "WHILE";
        case TokenType::FOR: return "FOR";
        case TokenType::RETURN: return "RETURN";
        default: return "UNKNOWN";
    }
}

void printTokens(std::vector<Token> tokens) {
    for (const auto& token : tokens) {
        switch (token.type) {
            case TokenType::STRING_LITERAL:
            case TokenType::INT_LITERAL:
            case TokenType::BOOL_LITERAL:
            case TokenType::IDENTIFIER:
            case TokenType::VAR:
            case TokenType::CONST:
            case TokenType::COMMENT:
            case TokenType::MULTICOMMENT:
            case TokenType::ASSIGNMENT:
            case TokenType::RETURN:
                // Tokens with value
                std::cout << "Token: " << TokenTypeToString(token.type) << ", Value: " << token.value << "\n";
                break;

            case TokenType::OPEN_PARENTHESIS:
            case TokenType::CLOSE_PARENTHESIS:
            case TokenType::OPEN_BRACE:
            case TokenType::CLOSE_BRACE:
            case TokenType::COMMA:
            case TokenType::SEMICOLON:
            case TokenType::NEWLINE:
            case TokenType::PLUS:
            case TokenType::MINUS:
            case TokenType::STAR:
            case TokenType::SLASH:
            case TokenType::AND:
            case TokenType::OR:
            case TokenType::NOT:
            case TokenType::EQUALS:
            case TokenType::NOT_EQUALS:
            case TokenType::GREATER_THAN:
            case TokenType::LESS_THAN:
            case TokenType::GREATER_OR_EQUAL:
            case TokenType::LESS_OR_EQUAL:
            case TokenType::IF:
            case TokenType::ELSE:
            case TokenType::WHILE:
            case TokenType::FOR:
                // Tokens without value
                std::cout << "Token: " << TokenTypeToString(token.type) << "\n";
                break;

            default:
                std::cout << "Token: UNKNOWN, Value: " << token.value << "\n";
                break;
        }
    }
}