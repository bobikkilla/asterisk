#include <iostream>
#include <vector>

using namespace std;


void bobik(int just_number); // main function

int kubik(int number); // just func for cube

int main() {
	// main cycle
  for (int i = 10; i <= 100; i++) {
    bobik(i);
  }
  return 0;
}

int kubik(int number) { return number * number * number; }

void bobik(int just_number) {
  vector<int> just_array;
  int count = 0;
  bool status = true;
  int just_number_in_cube = kubik(just_number); 
  int copy_cube_for_output = just_number_in_cube; // init variables

  while (just_number_in_cube != 0) { 
    just_array.insert(just_array.begin() + count, just_number_in_cube % 10);
    just_number_in_cube = just_number_in_cube / 10;
    count++;
  } // number - > array of digits

  for (int i = 0; i < (just_array.size() / 2); i++) {
    if (just_array[i] != just_array[just_array.size() - 1 - i]) { 
      status = false;
      break;
    }
  } // cycle for valid check

  cout << just_number << "\t" << copy_cube_for_output << "\t" << (status ? "TRUE":"FALSE") << endl;
}
