#include "matrix.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>

using namespace std;

// Helper for floating point comparison with tolerance
bool isApproxEqual(double a, double b, double tolerance = 1e-9) {
    return std::abs(a - b) <= tolerance;
}

// Helper for matrix comparison with tolerance (more robust than Matrix::operator==)
bool isMatrixApproxEqual(const Matrix& m1, const Matrix& m2, double tolerance = 1e-9) {
    if (m1.rows() != m2.rows() || m1.cols() != m2.cols()) {
        return false;
    }
    for (size_t i = 0; i < m1.rows(); ++i) {
        for (size_t j = 0; j < m1.cols(); ++j) {
            if (!isApproxEqual(m1.at(i, j), m2.at(i, j), tolerance)) {
                return false;
            }
        }
    }
    return true;
}

void test_constructors() {
    cout << "--- Testing Constructors ---" << endl;

    // 1. Default Constructor
    Matrix m0;
    cout << "Test 1: Default (0x0) - rows=" << m0.rows() << ", cols=" << m0.cols() << (m0.rows() == 0 && m0.cols() == 0 ? " [OK]" : " [FAIL]") << endl;

    // 2. Sized Constructor with value (3x2, value 5.0)
    Matrix m1(3, 2, 5.0);
    Matrix m1_expected({ {5.0, 5.0}, {5.0, 5.0}, {5.0, 5.0} });
    cout << "Test 2: Sized (3x2, 5.0) - " << (isMatrixApproxEqual(m1, m1_expected) ? "[OK]" : "[FAIL]") << endl;

    // 3. Initializer List Constructor (2x3)
    Matrix m2 = { {1.0, 2.0, 3.0}, {4.0, 5.0, 6.0} };
    cout << "Test 3: Init List (2x3) - " << (m2.rows() == 2 && m2.cols() == 3 && isApproxEqual(m2.at(1, 2), 6.0) ? "[OK]" : "[FAIL]") << endl;

    // 4. Diagonal Constructor (3x3 diagonal)
    Matrix m3({10.0, 20.0, 30.0});
    Matrix m3_expected = { {10.0, 0.0, 0.0}, {0.0, 20.0, 0.0}, {0.0, 0.0, 30.0} };
    cout << "Test 4: Diagonal (3x3) - " << (isMatrixApproxEqual(m3, m3_expected) ? "[OK]" : "[FAIL]") << endl;

    // 5. Copy Constructor
    Matrix m4 = m2;
    cout << "Test 5: Copy Ctor (m4=m2) - " << (m4 == m2 && &m4.at(0,0) != &m2.at(0,0) ? "[OK]" : "[FAIL]") << endl;

    // 6. Move Constructor
    Matrix m5 = std::move(m4);
    cout << "Test 6: Move Ctor (m5=move(m4)) - " << (m5 == m2 && m4.rows() == 0 ? "[OK]" : "[FAIL]") << endl;
    cout << m5 << endl;
}

void test_accessors() {
    cout << "--- Testing Accessors ---" << endl;
    Matrix m = { {1.1, 2.2}, {3.3, 4.4} };

    // Mutable Access and Modification
    m.at(0, 1) = 99.0;
    cout << "Test 7: Mutable at(0, 1) modification - " << (isApproxEqual(m.at(0, 1), 99.0) ? "[OK]" : "[FAIL]") << endl;

    // Const Access
    const Matrix& cm = m;
    cout << "Test 8: Const at(1, 0) access - " << (isApproxEqual(cm.at(1, 0), 3.3) ? "[OK]" : "[FAIL]") << endl;
}

void test_arithmetic() {
    cout << "--- Testing Arithmetic Operators ---" << endl;

    Matrix A = { {1, 2}, {3, 4} };
    Matrix B = { {10, 20}, {30, 40} };
    double s = 2.0;

    // 9. Matrix + Matrix
    Matrix C = A + B;
    Matrix C_expected = { {11, 22}, {33, 44} };
    cout << "Test 9: M + M - " << (isMatrixApproxEqual(C, C_expected) ? "[OK]" : "[FAIL]") << endl;

    // 10. Matrix * Matrix
    Matrix D = A * B; // {{1*10+2*30, 1*20+2*40}, {3*10+4*30, 3*20+4*40}} = {{70, 100}, {150, 220}}
    Matrix D_expected = { {70, 100}, {150, 220} };
    cout << "Test 10: M * M - " << (isMatrixApproxEqual(D, D_expected) ? "[OK]" : "[FAIL]") << endl;

    // 11. Matrix + Scalar
    Matrix E = A + s;
    Matrix E_expected = { {3, 4}, {5, 6} };
    cout << "Test 11: M + S - " << (isMatrixApproxEqual(E, E_expected) ? "[OK]" : "[FAIL]") << endl;

    // 12. Matrix *= Scalar (Compound Assignment)
    Matrix F = { {1, 1}, {1, 1} };
    F *= 3.0;
    Matrix F_expected = { {3, 3}, {3, 3} };
    cout << "Test 12: M *= S - " << (isMatrixApproxEqual(F, F_expected) ? "[OK]" : "[FAIL]") << endl;
    
    // 13. Matrix += Matrix (Compound Assignment)
    A += B; // A is now C_expected
    cout << "Test 13: M += M - " << (isMatrixApproxEqual(A, C_expected) ? "[OK]" : "[FAIL]") << endl;

    // 14. Matrix -= Matrix
    Matrix G = C - B; // Should equal the original A
    Matrix A_original = { {1, 2}, {3, 4} };
    cout << "Test 14: M - M - " << (isMatrixApproxEqual(G, A_original) ? "[OK]" : "[FAIL]") << endl;
}

void test_assignment_and_comparison() {
    cout << "--- Testing Assignment & Comparison ---" << endl;

    Matrix m1 = { {1, 2}, {3, 4} };
    Matrix m2 = { {1, 2}, {3, 4} };
    Matrix m3 = { {5, 6}, {7, 8} };
    Matrix m4;

    // 15. Comparison == and !=
    cout << "Test 15: M1 == M2 (Equal) - " << (m1 == m2 ? "[OK]" : "[FAIL]") << endl;
    cout << "Test 16: M1 != M3 (Not Equal) - " << (m1 != m3 ? "[OK]" : "[FAIL]") << endl;
    
    // 17. Copy Assignment
    m4 = m1;
    cout << "Test 17: Copy Assignment (M4=M1) - " << (m4 == m1 && &m4.at(0,0) != &m1.at(0,0) ? "[OK]" : "[FAIL]") << endl;

    // 18. Move Assignment
    Matrix m5 = { {10, 10} };
    Matrix m6 = { {20, 20} };
    m5 = std::move(m6); // m5 gets m6's data
    Matrix m5_expected = { {20, 20} };
    cout << "Test 18: Move Assignment (M5=move(M6)) - " << (m5 == m5_expected && m6.rows() == 0 ? "[OK]" : "[FAIL]") << endl;

    // 19. Self-assignment check (safe)
    Matrix m_self = { {1, 1} };
    m_self = m_self; 
    cout << "Test 19: Self-Assignment - " << (m_self.rows() == 1 && m_self.cols() == 2 ? "[OK]" : "[FAIL]") << endl;
}

void test_exceptions() {
    cout << "--- Testing Exception Handling ---" << endl;
    
    // Helper lambda to run a test and catch an exception
    auto run_test = [](const std::string& name, auto func, const std::type_info& expected_exception) {
        bool passed = false;
        try {
            func();
        } catch (const std::exception& e) {
            if (std::string(e.what()).find(expected_exception.name()) != std::string::npos || 
                typeid(e) == expected_exception) 
            {
                 passed = true;
            }
        } catch (...) {
            // Catch other exceptions
        }
        cout << name << " - " << (passed ? "[OK]" : "[FAIL]") << endl;
    };

    // 20. Invalid size construction (0x2)
    run_test("Test 20: 0 rows ctor", []{ Matrix m(0, 2, 0.0); }, typeid(std::invalid_argument));

    // 21. Ragged array in init list
    run_test("Test 21: Ragged init list", []{ Matrix m = {{1, 2}, {3, 4, 5}}; }, typeid(std::invalid_argument));

    // 22. Matrix + Matrix size mismatch
    run_test("Test 22: M+M size mismatch", []{ Matrix A(2, 2, 1); Matrix B(2, 3, 1); A + B; }, typeid(std::invalid_argument));

    // 23. Matrix * Matrix incompatible sizes
    run_test("Test 23: M*M incompatible", []{ Matrix A(2, 2, 1); Matrix B(3, 3, 1); A * B; }, typeid(std::invalid_argument));
    
    // 24. Access out of bounds (at)
    run_test("Test 24: at() out of bounds", []{ Matrix m(2, 2, 1); m.at(2, 0); }, typeid(std::out_of_range));
}

void test_output() {
    cout << "--- Testing Output Operator (<<) ---" << endl;
    Matrix m = { {1.0, 2.5}, {3.14159, -4.0} };
    cout << "Test 25: Output formatting (Should look aligned/clean):" << endl;
    // The actual test is visual inspection, but we ensure it compiles and runs.
    cout << m; 
    cout << "[OK - Visual Check Required]" << endl;
}

int main() {
    cout << "========================================" << endl;
    cout << "         Matrix Class Test Suite        " << endl;
    cout << "========================================" << endl;

    test_constructors();
    test_accessors();
    test_arithmetic();
    test_assignment_and_comparison();
    test_exceptions();
    test_output();
    
    cout << "========================================" << endl;
    cout << "All tests concluded." << endl;

    // Example of memory/exception safety check (should clean up correctly)
    try {
        Matrix m = { {1, 2}, {3, 4} };
        Matrix m_err(1000000000, 1000000000, 1.0); // This should throw std::bad_alloc or similar (not caught by our simple test)
    } catch (const std::exception& e) {
        cerr << "Caught exception in final check: " << e.what() << endl;
    }

    return 0;
}