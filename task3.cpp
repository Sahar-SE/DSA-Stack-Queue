#include <cctype>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

template <typename T>
class LinkedStack {
private:
    struct Node {
        T value;
        Node* next;

        explicit Node(const T& item, Node* nextNode)
            : value(item), next(nextNode) {}
    };

    Node* topNode = nullptr;

public:
    LinkedStack() = default;

    LinkedStack(const LinkedStack&) = delete;
    LinkedStack& operator=(const LinkedStack&) = delete;

    ~LinkedStack() {
        clear();
    }

    void push(const T& item) {
        topNode = new Node(item, topNode);
    }

    bool pop(T& item) {
        if (isEmpty()) {
            return false;
        }

        Node* removed = topNode;
        item = removed->value;
        topNode = removed->next;
        delete removed;
        return true;
    }

    bool top(T& item) const {
        if (isEmpty()) {
            return false;
        }

        item = topNode->value;
        return true;
    }

    bool isEmpty() const {
        return topNode == nullptr;
    }

    void clear() {
        while (topNode != nullptr) {
            Node* removed = topNode;
            topNode = topNode->next;
            delete removed;
        }
    }
};

int precedence(char operation) {
    if (operation == '~' || operation == '#') {
        return 3;
    }
    if (operation == '*' || operation == '/' || operation == '%') {
        return 2;
    }
    return 1;
}

bool isBinaryOperator(char character) {
    return character == '+' || character == '-' || character == '*' ||
           character == '/' || character == '%';
}

std::string infixToPostfix(const std::string& expression) {
    LinkedStack<char> operators;
    std::vector<std::string> output;
    bool expectOperand = true;
    bool foundToken = false;

    for (std::size_t index = 0; index < expression.size();) {
        const unsigned char character = static_cast<unsigned char>(expression[index]);

        if (std::isspace(character)) {
            ++index;
            continue;
        }

        if (std::isdigit(character) || character == '.') {
            if (!expectOperand) {
                throw std::invalid_argument("Missing operator between operands.");
            }

            const std::size_t start = index;
            bool foundDigit = false;
            bool foundDecimalPoint = false;
            while (index < expression.size()) {
                const unsigned char numberCharacter =
                    static_cast<unsigned char>(expression[index]);
                if (std::isdigit(numberCharacter)) {
                    foundDigit = true;
                    ++index;
                } else if (numberCharacter == '.' && !foundDecimalPoint) {
                    foundDecimalPoint = true;
                    ++index;
                } else {
                    break;
                }
            }

            if (!foundDigit) {
                throw std::invalid_argument("Invalid numeric operand.");
            }

            output.push_back(expression.substr(start, index - start));
            expectOperand = false;
            foundToken = true;
            continue;
        }

        const char current = static_cast<char>(character);
        if (current == '(') {
            if (!expectOperand) {
                throw std::invalid_argument("Missing operator before '('.");
            }
            operators.push(current);
            foundToken = true;
            ++index;
            continue;
        }

        if (current == ')') {
            if (expectOperand) {
                throw std::invalid_argument("Unexpected or empty parentheses.");
            }

            char operation;
            bool foundOpeningParenthesis = false;
            while (operators.pop(operation)) {
                if (operation == '(') {
                    foundOpeningParenthesis = true;
                    break;
                }
                output.push_back(operation == '~' ? "u-" :
                                 operation == '#' ? "u+" : std::string(1, operation));
            }
            if (!foundOpeningParenthesis) {
                throw std::invalid_argument("Mismatched parentheses.");
            }
            ++index;
            continue;
        }

        if (isBinaryOperator(current)) {
            if (expectOperand) {
                if (current != '+' && current != '-') {
                    throw std::invalid_argument("An operator is missing its left operand.");
                }
                operators.push(current == '-' ? '~' : '#');
                ++index;
                foundToken = true;
                continue;
            }

            char topOperation;
            while (operators.top(topOperation) && topOperation != '(' &&
                   precedence(topOperation) >= precedence(current)) {
                operators.pop(topOperation);
                output.push_back(topOperation == '~' ? "u-" :
                                 topOperation == '#' ? "u+" : std::string(1, topOperation));
            }
            operators.push(current);
            expectOperand = true;
            ++index;
            continue;
        }

        throw std::invalid_argument("Invalid character in expression.");
    }

    if (!foundToken) {
        throw std::invalid_argument("Expression is empty.");
    }
    if (expectOperand) {
        throw std::invalid_argument("Expression ends where an operand is required.");
    }

    char operation;
    while (operators.pop(operation)) {
        if (operation == '(') {
            throw std::invalid_argument("Mismatched parentheses.");
        }
        output.push_back(operation == '~' ? "u-" :
                         operation == '#' ? "u+" : std::string(1, operation));
    }

    std::string postfix;
    for (const std::string& token : output) {
        if (!postfix.empty()) {
            postfix += ' ';
        }
        postfix += token;
    }
    return postfix;
}

double evaluatePostfix(const std::string& postfix) {
    LinkedStack<double> values;
    std::size_t index = 0;

    while (index < postfix.size()) {
        while (index < postfix.size() && postfix[index] == ' ') {
            ++index;
        }
        if (index == postfix.size()) {
            break;
        }

        const std::size_t end = postfix.find(' ', index);
        const std::string token = postfix.substr(
            index, end == std::string::npos ? std::string::npos : end - index);
        index = end == std::string::npos ? postfix.size() : end + 1;

        if (token == "u-" || token == "u+") {
            double operand;
            if (!values.pop(operand)) {
                throw std::invalid_argument("Invalid postfix expression.");
            }
            values.push(token == "u-" ? -operand : operand);
            continue;
        }

        if (token.size() == 1 && isBinaryOperator(token[0])) {
            double right;
            double left;
            if (!values.pop(right) || !values.pop(left)) {
                throw std::invalid_argument("Invalid postfix expression.");
            }

            switch (token[0]) {
            case '+':
                values.push(left + right);
                break;
            case '-':
                values.push(left - right);
                break;
            case '*':
                values.push(left * right);
                break;
            case '/':
                if (right == 0.0) {
                    throw std::runtime_error("Division by zero.");
                }
                values.push(left / right);
                break;
            case '%':
                if (right == 0.0) {
                    throw std::runtime_error("Division by zero in modulus.");
                }
                values.push(std::fmod(left, right));
                break;
            }
            continue;
        }

        try {
            std::size_t parsedCharacters = 0;
            const double value = std::stod(token, &parsedCharacters);
            if (parsedCharacters != token.size()) {
                throw std::invalid_argument("Invalid number in postfix expression.");
            }
            values.push(value);
        } catch (const std::invalid_argument&) {
            throw std::invalid_argument("Invalid number in postfix expression.");
        }
    }

    double result;
    if (!values.pop(result) || !values.isEmpty()) {
        throw std::invalid_argument("Invalid postfix expression.");
    }
    return result;
}

int main() {
    std::string expression;
    std::cout << "Enter an infix expression: ";
    std::getline(std::cin, expression);

    try {
        const std::string postfix = infixToPostfix(expression);
        std::cout << "Postfix: " << postfix << '\n';
        std::cout << "Result: " << std::setprecision(15)
                  << evaluatePostfix(postfix) << '\n';
    } catch (const std::exception& error) {
        std::cout << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}