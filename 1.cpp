#include <iostream>
#include <sstream>
#include <string>
#include <stack> 
#include <stdexcept> 
#include <cmath> 

// Класс для хранения бинарного числа
class binary {
    // Размер бинарного числа
    static const int BINARY_SIZE = 8;
    // Максимальное десятичное число, которое можно представить в бинарном виде
    const int MAX_DECIMAL = pow(2, BINARY_SIZE) - 1;
    // Массив для хранения битов бинарного числа
    char bits[BINARY_SIZE];
    // Десятичное представление числа
    int decimal;

    // Функция для инвертирования всех битов (дополнение до 1)
    void negate() {
        for (unsigned int i = 0; i < BINARY_SIZE; i++) {
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
        decimal = decimal << shift;
    }

public:
    // Функция возведения числа в степень
    int pow(int base, unsigned int deg) {
        int result = 1;
        for (unsigned int i = 0; i < deg; i++) {
            result *= base;
        }
        return result;
    }

    // Конструктор по умолчанию
    binary() {
        for (int i = 0; i < BINARY_SIZE; i++) {
            bits[i] = 0;
        }
        decimal = 0;
    }

    // Конструктор с параметром - десятичное число
    binary(int _decimal) {
        if (abs(_decimal) > MAX_DECIMAL) {
            throw std::runtime_error("Decimal is too large!");
        }

        decimal = _decimal;
        bool negative = decimal < 0;

        int decimal_tmp = abs(_decimal);

        for (int i = BINARY_SIZE - 1; i >= 0; i--) {
            bits[i] = decimal_tmp % 2;
            decimal_tmp /= 2;
        }

        if (negative) {
            negate();
            *this = *this + binary(1);
            decimal--;
        }
    }

    // Оператор присваивания
    binary& operator=(const binary& other) {
        if (this != &other) {
            for (int i = 0; i < BINARY_SIZE; i++) {
                bits[i] = other.bits[i];
            }
        }
        this->decimal = other.decimal;

        return *this;
    }

    // Оператор вывода в поток
    friend std::ostream& operator<<(std::ostream& os, const binary& b) {
        for (int i = 0; i < BINARY_SIZE; i++) {
            os << (int)b.bits[i];
        }
        os << '(' << b.decimal << ')';
        return os;
    }

    // Оператор сложения
    binary operator+(const binary& other) const {
        binary result;

        int carry = 0;
        for (int i = BINARY_SIZE - 1; i >= 0; i--) {
            int sum = this->bits[i] + other.bits[i] + carry;
            result.bits[i] = sum % 2;
            carry = sum / 2;
        }

        result.decimal = other.decimal + decimal;

        if (abs(result.decimal) > MAX_DECIMAL) {
            throw std::runtime_error("Overflow...");
        }

        return result;
    }

    // Унарный оператор минус (инвертирование)
    binary operator-() const {
        binary result(*this);

        result.negate();
        result = result + binary(1);
        result.decimal = -decimal;

        return result;
    }

    // Оператор вычитания
    binary operator-(const binary& other) const {
        return *this + (-other);
    }

    // Оператор умножения
    binary operator*(const binary& other) const {
        binary result;
        for (int i = 0; i < BINARY_SIZE; ++i) {
            if (other.bits[BINARY_SIZE - 1 - i]) {
                binary temp(*this);
                temp.shift_bits(i);
                result = result + temp;
            }
        }

        if (abs(result.decimal) > MAX_DECIMAL) {
            throw std::runtime_error("Overflow...");
        }

        return result;
    }
};

// Узел связного списка
struct Node {
    // Данные типа binary
    binary data;
    // Указатель на следующий узел
    Node* next;

    // Конструктор узла
    Node(binary data) : data(data), next(nullptr) {}
};

// Реализация стека на основе связного списка
class Stack {
private:
    // Указатель на вершину стека
    Node* top;

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
    void push(binary value) {
        Node* newNode = new Node(value);
        newNode->next = top;
        top = newNode;
    }

    // Функция для удаления элемента из стека
    binary pop() {
        if (isEmpty()) {
            throw std::runtime_error("Stack is empty");
        }
        binary value = top->data;
        Node* temp = top;
        top = top->next;
        delete temp;
        return value;
    }

    // Функция для проверки, пуст ли стек
    bool isEmpty() const {
        return top == nullptr;
    }
};// Функция для обработки постфиксного выражения
void evaluatePostfix(const std::string& expression) {
    // Используем стандартный стек для хранения операндов
    std::stack<int> stack;
    std::istringstream iss(expression); // Создаем поток для разбора строки
    std::string token; // Переменная для хранения текущего токена

    // Проходим по каждому токену в выражении
    while (iss >> token) {
        // Проверяем, является ли токен числом (включая отрицательные числа)
        if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))) {
            stack.push(std::stoi(token)); // Преобразуем токен в число и помещаем его в стек
        } else if (token.size() == 1 && (token[0] == '+' || token[0] == '-' || token[0] == '*')) {
            // Проверяем, является ли токен оператором (+, -, *)
            if (stack.empty()) throw std::runtime_error("Invalid expression"); // Если стек пуст, выражение некорректно
            int operand2 = stack.top(); // Извлекаем второй операнд из стека
            stack.pop();

            if (stack.empty()) throw std::runtime_error("Invalid expression"); // Если стек пуст, выражение некорректно
            int operand1 = stack.top(); // Извлекаем первый операнд из стека
            stack.pop();

            int result;
            switch (token[0]) {
                case '+':
                    result = operand1 + operand2; // Выполняем сложение
                    break;
                case '-':
                    result = operand1 - operand2; // Выполняем вычитание
                    break;
                case '*':
                    result = operand1 * operand2; // Выполняем умножение
                    break;
                default:
                    throw std::runtime_error("Invalid operation"); // Некорректная операция
            }
            stack.push(result); // Помещаем результат обратно в стек
        } else {
            throw std::runtime_error("Invalid token: " + token); // Некорректный токен
        }
    }

    if (stack.empty()) throw std::runtime_error("Invalid expression"); // Если стек пуст, выражение некорректно

    int result = stack.top(); // Извлекаем результат из стека
    stack.pop();
    if (!stack.empty()) throw std::runtime_error("Invalid expression"); // Если после извлечения результата стек не пуст, выражение некорректно

    std::cout << "Result: " << result << std::endl; // Выводим результат
}

int main() {
    try {
        std::string expression;
        std::cout << "Enter postfix expression: ";
        std::getline(std::cin, expression); // Читаем постфиксное выражение от пользователя

        evaluatePostfix(expression); // Вызываем функцию для вычисления выражения
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl; // Обрабатываем возможные исключения и выводим сообщение об ошибке
    }

    return 0;
}