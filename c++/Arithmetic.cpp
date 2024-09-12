#include "arithmetic.hpp"
#include "Error.hpp"

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
        case '/': return a / b;
    }
    Error::e7.printErrorMessage();
}

// Function to evaluate an expression string
std::string evaluate(const std::string& expression) {
    std::stack<int> values;
    std::stack<char> ops;
    std::stringstream ss(expression);
    char ch;

    while (ss >> ch) {
        if (isdigit(ch)) {
            ss.putback(ch);
            int value;
            ss >> value;
            values.push(value);
        } else if (ch == '(') {
            ops.push(ch);
        } else if (ch == ')') {
            while (!ops.empty() && ops.top() != '(') {
                int val2 = values.top(); values.pop();
                int val1 = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOp(val1, val2, op));
            }
            ops.pop(); // Remove '('
        } else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
            while (!ops.empty() && (ops.top() == '*' || ops.top() == '/') && (ch == '+' || ch == '-')) {
                int val2 = values.top(); values.pop();
                int val1 = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOp(val1, val2, op));
            }
            ops.push(ch);
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