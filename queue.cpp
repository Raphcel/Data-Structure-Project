#include <iostream>

template <typename T>
struct Node {
    T data;
    Node* next;
};

template <typename T>
class Queue {
private:
    Node<T>* front;
    Node<T>* rear;

public:
    Queue() {
        front = rear = nullptr;
    }

    ~Queue() {
        while (!isEmpty())
            dequeue();
    }

    void enqueue(T value) {
        Node<T>* newNode = new Node<T>();
        newNode->data = value;
        newNode->next = nullptr;

        if (rear == nullptr) {
            front = rear = newNode;
            return;
        }

        rear->next = newNode;
        rear = newNode;
    }

    void dequeue() {
        if (isEmpty()) {
            std::cout << "Queue Underflow\n";
            return;
        }

        Node<T>* temp = front;
        front = front->next;

        if (front == nullptr)
            rear = nullptr;

        delete temp;
    }

    T peek() {
        if (isEmpty()) {
            std::cout << "Queue is Empty\n";
            return T(); // Return default-constructed value
        }
        return front->data;
    }

    bool isEmpty() {
        return front == nullptr;
    }

    void display() {
        if (isEmpty()) {
            std::cout << "Queue is Empty\n";
            return;
        }

        Node<T>* temp = front;
        std::cout << "Queue elements:\n";
        while (temp != nullptr) {
            std::cout << temp->data << " ";
            temp = temp->next;
        }
        std::cout << "\n";
    }
};
