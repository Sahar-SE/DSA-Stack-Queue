#include <iostream>
#include <string>

class CharStack {
private:
    struct Node {
        char value;
        Node* next;

        explicit Node(char character, Node* nextNode = nullptr)
            : value(character), next(nextNode) {}
    };

    Node* head = nullptr;

public:
    CharStack() = default;

    CharStack(const CharStack&) = delete;
    CharStack& operator=(const CharStack&) = delete;

    ~CharStack() {
        clear();
    }

    void push(char character) {
        head = new Node(character, head);
    }

    bool pop(char& character) {
        if (isEmpty()) {
            return false;
        }

        Node* removed = head;
        character = removed->value;
        head = removed->next;
        delete removed;
        return true;
    }

    bool top(char& character) const {
        if (isEmpty()) {
            return false;
        }

        character = head->value;
        return true;
    }

    bool isEmpty() const {
        return head == nullptr;
    }

    void display() const {
        for (Node* current = head; current != nullptr; current = current->next) {
            std::cout << current->value << ' ';
        }
        std::cout << '\n';
    }

    void clear() {
        while (!isEmpty()) {
            Node* removed = head;
            head = head->next;
            delete removed;
        }
    }
};

bool isMatchingPair(char opening, char closing) {
    return (opening == '(' && closing == ')') ||
           (opening == '[' && closing == ']') ||
           (opening == '{' && closing == '}');
}

bool checkBalance(const std::string& expression) {
    CharStack stack;

    for (char character : expression) {
        if (character == '(' || character == '[' || character == '{') {
            stack.push(character);
        } else if (character == ')' || character == ']' || character == '}') {
            char opening;
            if (!stack.pop(opening) || !isMatchingPair(opening, character)) {
                stack.clear();
                return false;
            }
        }
    }

    const bool balanced = stack.isEmpty();
    stack.clear();
    return balanced;
}

int main() {
    const std::string expressions[] = {
        "(A+B)",
        "{A+[B*C]}",
        "(A+B]",
        "((A+B)",
        "{[()]}",
        "A+B*C",
        "([A+B])"
    };

    for (const std::string& expression : expressions) {
        std::cout << expression << ": "
                  << (checkBalance(expression) ? "Balanced" : "Not Balanced")
                  << '\n';
    }

    return 0;
}
