#include <iostream>

using namespace std;

int min(int a, int b) {
  return (a<b)?a:b;
}

int max(int a, int b) {
  return (a>b)?a:b;
}

int missing_numerals(int number);

int main() {
  int number;
  cout << "Введите число, цифры которого стоят в порядке возрастания: ";
  cin >> number;
  cout << "Количество пропущенных цифр в числе = " << missing_numerals(number) << endl;
  return 0;
}

int missing_numerals(int number) {
  if(number < 10) {
    return 0;
  }
  int count_missing = 0;
  int first_digit = number % 10;
  number /= 10;
  if(number == 0) {
    return 0;
  }
  int second_digit = number % 10; 
  count_missing = max(first_digit,second_digit) - min(first_digit,second_digit) - 1;
  
  if(second_digit == first_digit) {
    count_missing = 0;
  }
  return count_missing + missing_numerals(number);
}