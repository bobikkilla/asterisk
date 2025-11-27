#include "matrix.h"
#include <stdexcept>
#include <iomanip>
#include <cmath>

// =================================================================
// Support Functions
// =================================================================

// Allocates dynamic memory for the matrix data
void Matrix::allocate() {
    data_ = new double[rows_ * cols_];
}

// Deep copies data from another matrix
void Matrix::copyFrom(const Matrix& other) {
    rows_ = other.rows_;
    cols_ = other.cols_;
    allocate();
    for (size_t i = 0; i < rows_ * cols_; ++i) {
        data_[i] = other.data_[i];
    }
}

// =================================================================
// Constructors and Destructor
// =================================================================

// Default constructor (empty matrix)
Matrix::Matrix() : rows_(0), cols_(0), data_(nullptr) {}

// Sized constructor with initial value
Matrix::Matrix(size_t r, size_t c, double val) : rows_(r), cols_(c) {
    if (r == 0 || c == 0) throw std::invalid_argument("Matrix size must be greater than zero.");
    allocate();
    for (size_t i = 0; i < r * c; ++i) data_[i] = val;
}

// Initializer list constructor (2D matrix)
Matrix::Matrix(std::initializer_list<std::initializer_list<double>> list) {
    if (list.size() == 0 || list.begin()->size() == 0)
        throw std::invalid_argument("Empty initializer list or rows.");

    rows_ = list.size();
    cols_ = list.begin()->size();

    // Check for rows of different sizes
    for (const auto& row : list) {
        if (row.size() != cols_)
            throw std::invalid_argument("Rows have different sizes.");
    }

    allocate();
    size_t i = 0;
    for (const auto& row : list) {
        size_t j = 0;
        for (double val : row) {
            data_[i * cols_ + j] = val;
            j++;
        }
        i++;
    }
}

// Initializer list constructor (Diagonal matrix)
Matrix::Matrix(std::initializer_list<double> diag) {
    rows_ = cols_ = diag.size();
    if (rows_ == 0) return; // Empty diagonal list creates an empty matrix

    allocate();
    // Initialize all to 0
    for (size_t i = 0; i < rows_ * cols_; ++i)
        data_[i] = 0.0;

    // Fill diagonal elements
    size_t i = 0;
    for (double val : diag) {
        // Fix: Use i, then explicitly increment i in the loop body
        data_[i * cols_ + i] = val; 
        i++; 
    }
}

// Copy constructor
Matrix::Matrix(const Matrix& other) {
    copyFrom(other);
}

// Move constructor
Matrix::Matrix(Matrix&& other) noexcept 
    : rows_(other.rows_), cols_(other.cols_), data_(other.data_) 
{
    // Steal resources and nullify the source
    other.data_ = nullptr;
    other.rows_ = other.cols_ = 0;
}

// Copy assignment operator
Matrix& Matrix::operator=(const Matrix& other) {
    if (this != &other) {
        // Self-assignment check
        delete[] data_; // Deallocate existing resources
        copyFrom(other); // Allocate and copy new resources
    }
    return *this;
}

// Move assignment operator
Matrix& Matrix::operator=(Matrix&& other) noexcept {
    if (this != &other) {
        delete[] data_; // Deallocate existing resources

        // Steal resources
        data_ = other.data_;
        rows_ = other.rows_;
        cols_ = other.cols_;

        // Nullify the source
        other.data_ = nullptr;
        other.rows_ = other.cols_ = 0;
    }
    return *this;
}

// Destructor
Matrix::~Matrix() {
    delete[] data_;
}

// =================================================================
// Accessors
// =================================================================

// Mutable access (bounds check)
double& Matrix::at(size_t r, size_t c) {
    if (r >= rows_ || c >= cols_)
        throw std::out_of_range("Index out of bounds in Matrix::at()");
    return data_[r * cols_ + c];
}

// Const access (bounds check)
const double& Matrix::at(size_t r, size_t c) const {
    if (r >= rows_ || c >= cols_)
        throw std::out_of_range("Index out of bounds in Matrix::at() const");
    return data_[r * cols_ + c];
}

// =================================================================
// Arithmetic Operators (Matrix-Matrix)
// =================================================================

Matrix Matrix::operator+(const Matrix& rhs) const {
    if (rows_ != rhs.rows_ || cols_ != rhs.cols_)
        throw std::invalid_argument("Size mismatch for Matrix addition.");
    Matrix res(rows_, cols_, 0.0);
    for (size_t i = 0; i < rows_ * cols_; ++i)
        res.data_[i] = data_[i] + rhs.data_[i];
    return res;
}

Matrix Matrix::operator-(const Matrix& rhs) const {
    if (rows_ != rhs.rows_ || cols_ != rhs.cols_)
        throw std::invalid_argument("Size mismatch for Matrix subtraction.");
    Matrix res(rows_, cols_, 0.0);
    for (size_t i = 0; i < rows_ * cols_; ++i)
        res.data_[i] = data_[i] - rhs.data_[i];
    return res;
}

Matrix Matrix::operator*(const Matrix& rhs) const {
    if (cols_ != rhs.rows_)
        throw std::invalid_argument("Incompatible sizes for Matrix multiplication: A.cols != B.rows.");
    Matrix res(rows_, rhs.cols_, 0.0);
    
    // Standard matrix multiplication O(n^3)
    for (size_t i = 0; i < rows_; ++i) { // iterate over rows of the resulting matrix
        for (size_t j = 0; j < rhs.cols_; ++j) { // iterate over columns of the resulting matrix
            double sum = 0.0;
            for (size_t k = 0; k < cols_; ++k) { // iterate over elements for dot product
                sum += at(i, k) * rhs.at(k, j);
            }
            res.at(i, j) = sum;
        }
    }
    return res;
}

// =================================================================
// Arithmetic Operators (Matrix-Scalar)
// =================================================================

Matrix Matrix::operator+(double s) const {
    Matrix r(*this);
    for (size_t i = 0; i < rows_ * cols_; ++i) r.data_[i] += s;
    return r;
}
Matrix Matrix::operator-(double s) const {
    Matrix r(*this);
    for (size_t i = 0; i < rows_ * cols_; ++i) r.data_[i] -= s;
    return r;
}
Matrix Matrix::operator*(double s) const {
    Matrix r(*this);
    for (size_t i = 0; i < rows_ * cols_; ++i) r.data_[i] *= s;
    return r;
}

// =================================================================
// Compound Assignment Operators
// =================================================================

// Matrix-Matrix
Matrix& Matrix::operator+=(const Matrix& r){ return *this = *this + r; }
Matrix& Matrix::operator-=(const Matrix& r){ return *this = *this - r; }
Matrix& Matrix::operator*=(const Matrix& r){ return *this = *this * r; }

// Matrix-Scalar
Matrix& Matrix::operator+=(double s){ for(size_t i=0; i<rows_*cols_; ++i) data_[i] += s; return *this; }
Matrix& Matrix::operator-=(double s){ for(size_t i=0; i<rows_*cols_; ++i) data_[i] -= s; return *this; }
Matrix& Matrix::operator*=(double s){ for(size_t i=0; i<rows_*cols_; ++i) data_[i] *= s; return *this; }

// =================================================================
// Comparison Operators
// =================================================================

bool Matrix::operator==(const Matrix& rhs) const {
    if (rows_ != rhs.rows_ || cols_ != rhs.cols_) return false;
    for (size_t i = 0; i < rows_*cols_; ++i)
        // Check for exact equality
        if (data_[i] != rhs.data_[i]) return false; 
    return true;
}

bool Matrix::operator!=(const Matrix& rhs) const {
    return !(*this == rhs);
}

// =================================================================
// Output and Size
// =================================================================

// Output stream operator
std::ostream& operator<<(std::ostream& os, const Matrix& m) {
    // Set fixed-point notation and precision for cleaner output
    os << std::fixed << std::setprecision(4); 
    for (size_t i = 0; i < m.rows_; ++i) {
        for (size_t j = 0; j < m.cols_; ++j)
            os << std::setw(10) << m.at(i,j) << " ";
        os << '\n';
    }
    os << std::defaultfloat; // Reset stream settings
    return os;
}

size_t Matrix::rows() const { return rows_; }
size_t Matrix::cols() const { return cols_; }