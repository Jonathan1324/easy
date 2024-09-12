#include "arithmetic.hpp"

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
    throw std::invalid_argument("Invalid operator");
}

// Function to evaluate an expression string
int evaluate(const std::string& expression) {
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

    return values.top();
}