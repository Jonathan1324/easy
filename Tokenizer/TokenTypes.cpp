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