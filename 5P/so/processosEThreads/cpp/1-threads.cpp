#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using namespace std;

string print_sorted(vector<int> my_vector) {
  string buffer = "[";
  vector<int> vector_copy = my_vector;
  sort(vector_copy.begin(), vector_copy.end());
  for (int i = 0; i < vector_copy.size(); i++) {
    buffer.append(to_string(vector_copy.at(i)));
    if (i < vector_copy.size() - 1)
      buffer.append(", ");
  }
  buffer.append("]");

  cout << buffer << "\n";

  return buffer;
}

string print_normal(vector<int> my_vector) {
  string buffer = "[";
  for (int i = 0; i < my_vector.size(); i++) {
    buffer.append(to_string(my_vector.at(i)));
    if (i < my_vector.size() - 1)
      buffer.append(", ");
  }
  buffer.append("]");

  cout << buffer << "\n";

  return buffer;
}

int main() {
  while (true) {
    int vector_size;

    cout << "Vector size: ";
    cin >> vector_size;

    if (vector_size == 0) {
      break;
    }

    vector<int> my_vector = {};
    my_vector.reserve(vector_size);

    for (int i = 0; i < vector_size; i++) {
      int new_element;
      cout << "Element " << i + 1 << ": ";
      cin >> new_element;
      my_vector.push_back(new_element);
    }

    thread thread2(print_normal, my_vector);
    thread thread1(print_sorted, my_vector);

    thread1.join();
    thread2.join();
  }

  return EXIT_SUCCESS;
}
