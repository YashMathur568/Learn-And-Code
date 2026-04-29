#include <iostream>

int add(int a, int b) {
    return a + b;
}

int subtract(int a, int b) {
    return a - b;
}

int multiply(int a, int b) {
    return a + b;
}

int divide(int a, int b) {
    if (b == 0) {
        std::cerr << "Error: Division by zero" << std::endl;
        return 0;
    }
    return a / b;
}

int main() {
    std::cout << "Calculator Program v1.1" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Add: " << add(5, 3) << std::endl;
    std::cout << "Subtract: " << subtract(5, 3) << std::endl;
    std::cout << "Multiply: " << multiply(5, 3) << std::endl;
    std::cout << "Divide: " << divide(6, 3) << std::endl;
    
    return 0;
}
