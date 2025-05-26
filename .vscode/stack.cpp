#include <iostream>

template <typename T>
struct Node {
    T data;
    Node* next;
};

template <typename T>
class Stack {
private:
    Node<T>* top;

public:
    Stack() {
        top = nullptr;
    }

    ~Stack() {
        while (!isEmpty())
            pop();
    }

    void push(T value) {
        Node<T>* newNode = new Node<T>();
        newNode->data = value;
        newNode->next = top;
        top = newNode;
    }

    void pop() {
        if (isEmpty()) {
            std::cout << "Stack Underflow\n";
            return;
        }
        Node<T>* temp = top;
        top = top->next;
        delete temp;
    }

    T peek() {
        if (isEmpty()) {
            std::cout << "Stack is Empty\n";
            return T(); // Return default-constructed value
        }
        return top->data;
    }

    bool isEmpty() {
        return top == nullptr;
    }

    void display() {
        if (isEmpty()) {
            std::cout << "Stack is Empty\n";
            return;
        }
        Node<T>* temp = top;
        std::cout << "Stack elements:\n";
        while (temp != nullptr) {
            std::cout << temp->data << " ";
            temp = temp->next;
        }
        std::cout << "\n";
    }
};
