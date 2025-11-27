#include <valarray>
#include <initializer_list>
#include <cmath>
#include <stdexcept>
#include <iostream>

class Vector3D : public std::valarray<double> {
public:
    // Default constructor
    Vector3D() : std::valarray<double>(3) {
        (*this)[0] = (*this)[1] = (*this)[2] = 0.0;
    }

    // Constructor with values
    Vector3D(double x, double y, double z) : std::valarray<double>(3) {
        (*this)[0] = x;
        (*this)[1] = y;
        (*this)[2] = z;
    }

    // Initializer list ctor
    Vector3D(const std::initializer_list<double>& list) : std::valarray<double>(list) {
        if (size() != 3)
            throw std::invalid_argument("Vector3D must have exactly 3 elements");
    }

    // Copy constructor
    Vector3D(const Vector3D& other)
        : std::valarray<double>(3)   // for safety 
    {
        if (other.size() != 3)
            throw std::logic_error("Copy source is not size 3");

        for (int i = 0; i < 3; ++i)
            (*this)[i] = other[i];
    }

    // Assignment operator
    Vector3D& operator=(const Vector3D& other) {
        if (this == &other) return *this;
        if (other.size() != 3)
            throw std::invalid_argument("Assignment: size must be 3");

        resize(3);
        for (int i = 0; i < 3; ++i)
            (*this)[i] = other[i];

        return *this;
    }

    // Addition
    Vector3D operator+(const Vector3D& other) const {
        Vector3D result;
        for (int i = 0; i < 3; ++i)
            result[i] = (*this)[i] + other[i];
        return result;
    }

    // Compound addition
    Vector3D& operator+=(const Vector3D& other) {
        for (int i = 0; i < 3; ++i)
            (*this)[i] += other[i];
        return *this;
    }

    // Multiplication by scalar
    Vector3D operator*(double scalar) const {
        Vector3D result;
        for (int i = 0; i < 3; ++i)
            result[i] = (*this)[i] * scalar;
        return result;
    }

    // Norm
    double norm(double p) const {
        if (p <= 0)
            throw std::invalid_argument("norm: p must be > 0");

        double sum = 0;
        for (int i = 0; i < 3; ++i)
            sum += std::pow(std::abs((*this)[i]), p);

        return std::pow(sum, 1.0 / p);
    }

    friend Vector3D operator*(double scalar, const Vector3D& v);
};

// scalar * vector
Vector3D operator*(double scalar, const Vector3D& v) {
    return v * scalar;
}

// output
std::ostream& operator<<(std::ostream& os, const Vector3D& v) {
    os << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
    return os;
}


// ========== TEST ===========
int main() {
    try {
        Vector3D u;
        Vector3D v{1, 2, 3};
        Vector3D w(v);
        std::cout << "default vector : " << u <<std::endl;
        Vector3D z = 3 * w;
        z = w;
        z = 2 * w + v * 3;
        z += w;

        std::cout << "Result vector: " << z << std::endl;
        std::cout << "Euclidean norm: " << z.norm(2) << std::endl;

        Vector3D a(1.0, 2.0, 3.0);
        Vector3D b = {4.0, 5.0, 6.0};

        std::cout << "a + b = " << (a + b) << std::endl;
        std::cout << "2.5 * a = " << (2.5 * a) << std::endl;
        std::cout << "a.norm(1) = " << a.norm(1) << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
    }
}
