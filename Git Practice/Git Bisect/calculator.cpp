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
    std::cout << "Calculator Program" << std::endl;
    std::cout << std::endl;
    
    int pass_count = 0;
    int fail_count = 0;
    
    if (add(5, 3) == 8) {
        std::cout << "PASS: add(5, 3) = 8" << std::endl;
        pass_count++;
    } else {
        std::cout << "FAIL: add(5, 3) = " << add(5, 3) << " (expected 8)" << std::endl;
        fail_count++;
    }
    
    if (subtract(5, 3) == 2) {
        std::cout << "PASS: subtract(5, 3) = 2" << std::endl;
        pass_count++;
    } else {
        std::cout << "FAIL: subtract(5, 3) = " << subtract(5, 3) << " (expected 2)" << std::endl;
        fail_count++;
    }
    
    if (multiply(5, 3) == 15) {
        std::cout << "PASS: multiply(5, 3) = 15" << std::endl;
        pass_count++;
    } else {
        std::cout << "FAIL: multiply(5, 3) = " << multiply(5, 3) << " (expected 15)" << std::endl;
        fail_count++;
    }
    
    if (divide(6, 3) == 2) {
        std::cout << "PASS: divide(6, 3) = 2" << std::endl;
        pass_count++;
    } else {
        std::cout << "FAIL: divide(6, 3) = " << divide(6, 3) << " (expected 2)" << std::endl;
        fail_count++;
    }
    
    std::cout << std::endl;
    std::cout << "Total: " << pass_count << " passed, " << fail_count << " failed" << std::endl;
    
    return fail_count;
}
