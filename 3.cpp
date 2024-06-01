#include <iostream>  // Подключение библиотеки для ввода-вывода
#include <stdexcept> // Подключение библиотеки для обработки исключений
#include <string>    // Подключение библиотеки для работы со строками
#include <sstream>   // Подключение библиотеки для работы с потоками строк
#include <cmath>     // Подключение библиотеки для математических операций

// Класс для представления 32-битного бинарного числа
class Binary {
    static const int BINARY_SIZE = 32; // Размер бинарного числа (32 бита)
    char bits[BINARY_SIZE]; // Массив для хранения битов бинарного числа
    int decimal; // Десятичное представление числа

    // Функция для инвертирования всех битов (дополнение до 1)
    void negate() {
        for (int i = 0; i < BINARY_SIZE; i++) {
            bits[i] = 1 - bits[i]; // Инвертирование каждого бита
        }
    }

    // Функция для сдвига битов влево на заданное количество позиций
    void shift_bits(unsigned int shift) {
        for (int i = 0; i < BINARY_SIZE - shift; i++) {
            bits[i] = bits[i + shift]; // Сдвиг битов влево
        }
        for (int i = BINARY_SIZE - shift; i < BINARY_SIZE; i++) {
            bits[i] = 0; // Заполнение освободившихся битов нулями
        }
        decimal <<= shift; // Сдвиг десятичного представления влево
    }

public:
    // Конструктор по умолчанию
    Binary() : decimal(0) {
        for (int i = 0; i < BINARY_SIZE; i++) {
            bits[i] = 0; // Инициализация всех битов нулями
        }
    }

    // Конструктор с параметром - десятичное число
    Binary(int _decimal) : decimal(_decimal) {
        bool negative = decimal < 0; // Проверка, является ли число отрицательным
        int abs_decimal = std::abs(decimal); // Абсолютное значение числа

        for (int i = BINARY_SIZE - 1; i >= 0; i--) {
            bits[i] = abs_decimal % 2; // Преобразование числа в двоичный вид
            abs_decimal /= 2;
        }

        if (negative) {
            negate(); // Инвертирование битов для отрицательных чисел
            *this = *this + Binary(1); // Добавление единицы для получения дополнительного кода
            decimal--; // Коррекция десятичного значения
        }
    }

    // Оператор присваивания
    Binary& operator=(const Binary& other) {
        if (this != &other) { // Проверка на самоприсваивание
            for (int i = 0; i < BINARY_SIZE; i++) {
                bits[i] = other.bits[i]; // Копирование битов
            }
            decimal = other.decimal; // Копирование десятичного значения
        }
        return *this;
    }

    // Оператор вывода в поток
    friend std::ostream& operator<<(std::ostream& os, const Binary& b) {
        for (int i = 0; i < BINARY_SIZE; i++) {
            os << (int)b.bits[i]; // Вывод битов в поток
        }
        os << " (" << b.decimal << ")"; // Вывод десятичного представления числа
        return os;
    }

    // Оператор сложения
    Binary operator+(const Binary& other) const {
        Binary result;
        int carry = 0; // Перенос при сложении битов
        for (int i = BINARY_SIZE - 1; i >= 0; i--) {
            int sum = this->bits[i] + other.bits[i] + carry; // Сложение битов и переноса
            result.bits[i] = sum % 2; // Результирующий бит
            carry = sum / 2; // Новый перенос
        }
        result.decimal = this->decimal + other.decimal; // Сложение десятичных значений
        return result;
    }

    // Унарный оператор минус (инвертирование)
    Binary operator-() const {
        Binary result(*this); // Создание копии текущего объекта
        result.negate(); // Инвертирование битов
        result = result + Binary(1); // Добавление единицы для получения дополнительного кода
        result.decimal = -decimal; // Инвертирование десятичного значения
        return result;
    }

    // Оператор вычитания
    Binary operator-(const Binary& other) const {
        return *this + (-other); // Вычитание через сложение с инверсией второго операнда
    }

    // Оператор умножения
    Binary operator*(const Binary& other) const {
        Binary result;
        for (int i = 0; i < BINARY_SIZE; ++i) {
            if (other.bits[BINARY_SIZE - 1 - i]) { // Проверка бита множителя на единицу
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
        Node* newNode = new Node(value);  // Создание нового узла
        newNode->next = top; // Установка указателя на следующий узел на текущий верхний узел
        top = newNode; // Обновление верхнего элемента стека
    }

    // Функция для удаления элемента из стека
    Binary pop() { 
        if (isEmpty()) {  // Проверка, пуст ли стек
            throw std::runtime_error("Stack is empty");  // Выброс исключения, если стек пуст
        }
        Binary value = top->data; // Получение значения верхнего элемента
        Node* temp = top; // Временное сохранение указателя на верхний элемент
        top = top->next;  // Обновление верхнего элемента на следующий
        delete temp;  // Освобождение памяти временного узла
        return value; // Возвращение значения удаленного элемента
    }

    // Функция для проверки, пуст ли стек
    bool isEmpty() const { 
        return top == nullptr;  // Возврат true, если верхний элемент равен nullptr (стек пуст)
    }
};

// Функция для обработки постфиксного выражения
Binary evaluatePostfix(const std::string& expression) {
    std::istringstream iss(expression);  // Создание потока ввода из строки выражения
    std::string token; // Переменная для хранения текущего токена
    Stack stack; // Создание стека

    while (iss >> token) {  // Цикл по каждому токену в выражении
        if (token == "+" || token == "-" || token == "*") {  // Если токен оператор
            if (stack.isEmpty()) throw std::runtime_error("Invalid expression"); // Проверка наличия операндов
            Binary b = stack.pop();  // Извлечение второго операнда

            if (stack.isEmpty()) throw std::runtime_error("Invalid expression"); // Проверка наличия первого операнда
            Binary a = stack.pop();  // Извлечение первого операнда

            if (token == "+") stack.push(a + b);  // Выполнение операции сложения и добавление результата в стек
            else if (token == "-") stack.push(a - b); // Выполнение операции вычитания и добавление результата в стек
            else if (token == "*") stack.push(a * b); // Выполнение операции умножения и добавление результата в стек
            
        } else {  // Если токен операнд
            try { 
                int value = std::stoi(token);  // Преобразование токена в целое число 
                stack.push(Binary(value));// Добавление операнда в стек
            } catch (...) { 
                throw std::runtime_error("Invalid token: " + token); // Обработка ошибки по некорректному операнду
            }
        }
    }

    if (stack.isEmpty()) throw std::runtime_error("Invalid expression"); // Проверка корректности выражения
    
    Binary result = stack.pop(); // Получение конечного результата
    
    if (!stack.isEmpty()) throw std::runtime_error("Invalid expression"); // Проверка корректности выражения

    return result; // Возвращение итогового результата
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