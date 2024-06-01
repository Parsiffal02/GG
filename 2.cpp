#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <cmath>

// Класс для представления 32-битного бинарного числа
class Binary {
    static const int BINARY_SIZE = 32; // Размер бинарного числа
    char bits[BINARY_SIZE]; // Массив для хранения битов бинарного числа
    int decimal; // Десятичное представление числа

    // Функция для инвертирования всех битов (дополнение до 1)
    void negate() {
        for (int i = 0; i < BINARY_SIZE; i++) {
            bits[i] = 1 - bits[i];
        }
    }

    // Функция для сдвига битов влево на заданное количество позиций
    void shift_bits(unsigned int shift) {
        for (int i = 0; i < BINARY_SIZE - shift; i++) {
            bits[i] = bits[i + shift];
        }
        for (int i = BINARY_SIZE - shift; i < BINARY_SIZE; i++) {
            bits[i] = 0;
        }
        decimal <<= shift;
    }

public:
    // Конструктор по умолчанию
    Binary() : decimal(0) {
        for (int i = 0; i < BINARY_SIZE; i++) {
            bits[i] = 0;
        }
    }

    // Конструктор с параметром - десятичное число
    Binary(int _decimal) : decimal(_decimal) {
        bool negative = decimal < 0;
        int abs_decimal = std::abs(decimal);

        for (int i = BINARY_SIZE - 1; i >= 0; i--) {
            bits[i] = abs_decimal % 2;
            abs_decimal /= 2;
        }

        if (negative) {
            negate();
            *this = *this + Binary(1);
            decimal--;
        }
    }

    // Оператор присваивания
    Binary& operator=(const Binary& other) {
        if (this != &other) {
            for (int i = 0; i < BINARY_SIZE; i++) {
                bits[i] = other.bits[i];
            }
            decimal = other.decimal;
        }
        return *this;
    }

    // Оператор вывода в поток
    friend std::ostream& operator<<(std::ostream& os, const Binary& b) {
        for (int i = 0; i < BINARY_SIZE; i++) {
            os << (int)b.bits[i];
        }
        os << " (" << b.decimal << ")";
        return os;
    }

    // Оператор сложения
    Binary operator+(const Binary& other) const {
        Binary result;
        int carry = 0;
        for (int i = BINARY_SIZE - 1; i >= 0; i--) {
            int sum = this->bits[i] + other.bits[i] + carry;
            result.bits[i] = sum % 2;
            carry = sum / 2;
        }
        result.decimal = this->decimal + other.decimal;
        return result;
    }

    // Унарный оператор минус (инвертирование)
    Binary operator-() const {
        Binary result(*this);
        result.negate();
        result = result + Binary(1);
        result.decimal = -decimal;
        return result;
    }

    // Оператор вычитания
    Binary operator-(const Binary& other) const {
        return *this + (-other);
    }

    // Оператор умножения
    Binary operator*(const Binary& other) const {
        Binary result;
        for (int i = 0; i < BINARY_SIZE; ++i) {
            if (other.bits[BINARY_SIZE - 1 - i]) {
                Binary temp(*this);
                temp.shift_bits(i);
                result = result + temp;
            }
        }
        return result;
    }
};

// Узел связного списка
struct Node {
    Binary data; // Данные типа Binary
    Node* next; // Указатель на следующий узел

    // Конструктор узла
    Node(Binary data) : data(data), next(nullptr) {}
};

// Реализация стека на основе связного списка
class Stack {
private:
    Node* top; // Указатель на вершину стека

public:
    // Конструктор по умолчанию
    Stack() : top(nullptr) {}

    // Деструктор для очистки памяти
    ~Stack() {
        while (!isEmpty()) {
            pop();
        }
    }

    // Функция для добавления элемента в стек
    void push(Binary value) {
        Node* newNode = new Node(value);
        newNode->next = top;
        top = newNode;
    }

    // Функция для удаления элемента из стека
    Binary pop() {
        if (isEmpty()) {
            throw std::runtime_error("Stack is empty");
        }
        Binary value = top->data;
        Node* temp = top;
        top = top->next;
        delete temp;
        return value;
    }

    // Функция для проверки, пуст ли стек
    bool isEmpty() const {
        return top == nullptr;
    }
};

// Функция для обработки постфиксного выражения
Binary evaluatePostfix(const std::string& expression) {
    std::istringstream iss(expression);
    std::string token;
    Stack stack;

    while (iss >> token) {
        if (token == "+" || token == "-" || token == "*") { 
            if (stack.isEmpty()) throw std::runtime_error("Invalid expression");
            Binary b = stack.pop();

            if (stack.isEmpty()) throw std::runtime_error("Invalid expression");
            Binary a = stack.pop();

            if (token == "+") stack.push(a + b);
            else if (token == "-") stack.push(a - b);
            else if (token == "*") stack.push(a * b);
            
        } else { 
            try { 
                int value = std::stoi(token); 
                stack.push(Binary(value));
            } catch (...) { 
                throw std::runtime_error("Invalid token: " + token);
            }
        }
    }

    if (stack.isEmpty()) throw std::runtime_error("Invalid expression");
    
    Binary result = stack.pop();
    
    if (!stack.isEmpty()) throw std::runtime_error("Invalid expression");

    return result;
}

int main() {
    try {
        
        std::string expression;
        
        std::cout << "Enter a postfix expression: ";
        
        std::getline(std::cin, expression);

        
        Binary result = evaluatePostfix(expression);

        
        std::cout << "Result: " << result << std::endl;

        
    } catch (const std::exception& e) {
        
        std::cerr << "Error: " << e.what() << std::endl;
        
    }

    
    return 0;
}