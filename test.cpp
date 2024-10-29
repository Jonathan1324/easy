#include <iostream>
#include <cstddef> // Für size_t

// Deklaration der externen Rust-Funktion
extern "C" {
    size_t add(size_t left, size_t right);
}

int main() { 
    size_t result = add(5, 3);  // Aufruf der Rust-Funktion
    std::cout << "Das Ergebnis ist: " << result << std::endl;  // Ausgabe des Ergebnisses
    return 0;
}
