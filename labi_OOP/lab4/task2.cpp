#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>

// base class comparable
class Comparable {
public:
    virtual ~Comparable() = default;

    // comparison interface
    virtual int key() const = 0;

    virtual bool operator<(const Comparable& other) const = 0;
    virtual bool operator>(const Comparable& other) const = 0;
    virtual bool operator<=(const Comparable& other) const = 0;
    virtual bool operator>=(const Comparable& other) const = 0;
    virtual bool operator==(const Comparable& other) const = 0;
    virtual bool operator!=(const Comparable& other) const = 0;
};



// ==================== FIRST DERIVED CLASS ====================
class Number : public Comparable {
private:
    int value;

public:
    explicit Number(int v) : value(v) {}

    int key() const override {
        return value;
    }

    bool operator<(const Comparable& other) const override {
        return key() < other.key();
    }

    bool operator>(const Comparable& other) const override {
        return key() > other.key();
    }

    bool operator<=(const Comparable& other) const override {
        return key() <= other.key();
    }

    bool operator>=(const Comparable& other) const override {
        return key() >= other.key();
    }

    bool operator==(const Comparable& other) const override {
        return key() == other.key();
    }

    bool operator!=(const Comparable& other) const override {
        return key() != other.key();
    }

    void print() const {
        std::cout << "Number(" << value << ")";
    }
};



// ==================== SECOND DERIVED CLASS (DYNAMIC MEMORY) ====================
class StringWrapper : public Comparable {
private:
    char* text;
    int length;

public:
    explicit StringWrapper(const char* s) {
        length = std::strlen(s);
        text = new char[length + 1];
        std::strcpy(text, s);
    }

    // destructor for dynamic memory
    ~StringWrapper() override {
        delete[] text;
    }

    // сopy constructor (rule of three)
    StringWrapper(const StringWrapper& other) {
        length = other.length;
        text = new char[length + 1];
        std::strcpy(text, other.text);
    }

    int key() const override {
        return length;      // KEY = str len
    }

    bool operator<(const Comparable& other) const override {
        return key() < other.key();
    }

    bool operator>(const Comparable& other) const override {
        return key() > other.key();
    }

    bool operator<=(const Comparable& other) const override {
        return key() <= other.key();
    }

    bool operator>=(const Comparable& other) const override {
        return key() >= other.key();
    }

    bool operator==(const Comparable& other) const override {
        return key() == other.key();
    }

    bool operator!=(const Comparable& other) const override {
        return key() != other.key();
    }

    void print() const {
        std::cout << "String(\"" << text << "\")";
    }
};



// test
int main() {
    Number n1(10);
    Number n2(3);

    StringWrapper s1("hello");       // length = 5
    StringWrapper s2("programming"); // length = 11

    std::cout << "--- Individual comparisons ---\n";
    std::cout << "n1 > s1 : " << (n1 > s1) << "\n";
    std::cout << "s1 < n2 : " << (s1 < n2) << "\n";
    std::cout << "s2 >= n1: " << (s2 >= n1) << "\n";

    // vector of base pointers
    std::vector<Comparable*> vec;

    vec.push_back(new Number(7));
    vec.push_back(new StringWrapper("cat"));    // length 3
    vec.push_back(new Number(20));
    vec.push_back(new StringWrapper("computer")); // length 8
    vec.push_back(new Number(1));

    // sort
    std::sort(vec.begin(), vec.end(),
        [](Comparable* a, Comparable* b) {
            return *a < *b;
        }
    );

    std::cout << "\n--- Sorted vector (by key) ---\n";
    for (auto ptr : vec) {
        std::cout << ptr->key() << " ";
    }
    std::cout << "\n";

    // delete
    for (auto ptr : vec)
        delete ptr;

    return 0;
}
