#include "Arithmetic.hpp"
#include "Error.cpp"

#include <iostream>
#include <sstream>
#include <stack>
#include <cctype>
#include <stdexcept>
#include <cmath>

// Function to perform basic arithmetic operations
int applyOp(int a, int b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': 
            if (b == 0) {
                Error::e7.printErrorMessage(); // Handle divide by zero
                return 0;
            }
            return a / b;
    }
    Error::e7.printErrorMessage();
    return 0; // In case of an unrecognized operator
}

// Function to evaluate an expression string
std::string evaluate(const std::string& expression) {
    std::stack<int> values;
    std::stack<char> ops;
    std::stringstream ss(expression);
    char ch;
    bool expectingNegative = true;

    while (ss >> ch) {
        if (isdigit(ch) || (ch == '-' && expectingNegative)) {
            ss.putback(ch);
            int value;
            ss >> value;
            values.push(value);
            expectingNegative = false;
        } else if (ch == '(') {
            ops.push(ch);
            expectingNegative = true;
        } else if (ch == ')') {
            while (!ops.empty() && ops.top() != '(') {
                int val2 = values.top(); values.pop();
                int val1 = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOp(val1, val2, op));
            }
            ops.pop(); // Remove '('
            expectingNegative = false;
        } else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
            while (!ops.empty() && (ops.top() == '*' || ops.top() == '/') && (ch == '+' || ch == '-')) {
                int val2 = values.top(); values.pop();
                int val1 = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOp(val1, val2, op));
            }
            ops.push(ch);
            expectingNegative = true;
        }
    }

    while (!ops.empty()) {
        int val2 = values.top(); values.pop();
        int val1 = values.top(); values.pop();
        char op = ops.top(); ops.pop();
        values.push(applyOp(val1, val2, op));
    }

    return std::to_string(values.top());
}