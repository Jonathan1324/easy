#include <string>

enum class TokenType {
    // Variablen
    VAR,
    CONST,
    ASSIGNMENT,
    IDENTIFIER,
    
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