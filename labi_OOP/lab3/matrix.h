#pragma once
#include <iostream>
#include <initializer_list>
#include <iomanip> // Included for friend function definition

class Matrix {
private:
    size_t rows_, cols_;
    double* data_;

    // Helper functions for memory management
    void allocate();
    void copyFrom(const Matrix& other);

public:
    // --- Constructors ---
    // 1. default constructor
    Matrix();

    // 2. constructor with (rows, cols, value)
    Matrix(size_t rows, size_t cols, double value);

    // 3. constructor initializer_list (matrix from 2D list)
    Matrix(std::initializer_list<std::initializer_list<double>> list);

    // 4. constructor diagonal matrix
    Matrix(std::initializer_list<double> diag);

    // 5. copy construtor (Rule of Five)
    Matrix(const Matrix& other);

    // 6. move construcotr (Rule of Five)
    Matrix(Matrix&& other) noexcept;

    // --- Assignment Operators ---
    // 7. copy assing operator (Rule of Five)
    Matrix& operator=(const Matrix& other);
    // 8. move assignment operator (Rule of Five)
    Matrix& operator=(Matrix&& other) noexcept;

    // 9. destructor (Rule of Five)
    ~Matrix();

    // --- Accessors ---
    // specific element access (mutable)
    double& at(size_t r, size_t c);
    // specific element access (const)
    const double& at(size_t r, size_t c) const;

    // --- Arithmetic Operators (Matrix-Matrix) ---
    Matrix operator+(const Matrix& rhs) const;
    Matrix operator-(const Matrix& rhs) const;
    Matrix operator*(const Matrix& rhs) const;

    // --- Arithmetic Operators (Matrix-Scalar) ---
    Matrix operator+(double scalar) const;
    Matrix operator-(double scalar) const;
    Matrix operator*(double scalar) const;

    // --- Compound Assignment Operators (Matrix-Matrix) ---
    Matrix& operator+=(const Matrix& rhs);
    Matrix& operator-=(const Matrix& rhs);
    Matrix& operator*=(const Matrix& rhs);

    // --- Compound Assignment Operators (Matrix-Scalar) ---
    Matrix& operator+=(double scalar);
    Matrix& operator-=(double scalar);
    Matrix& operator*=(double scalar);

    // --- Comparison Operators ---
    bool operator==(const Matrix& rhs) const;
    bool operator!=(const Matrix& rhs) const;

    // --- Output ---
    friend std::ostream& operator<<(std::ostream& os, const Matrix& m);

    // --- Size Accessors ---
    size_t rows() const;
    size_t cols() const;
};