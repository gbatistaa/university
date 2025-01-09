#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>

using namespace std;

template <typename Value> class hashmap {
private:
  size_t map_size;

  typedef struct key_value {
    string key;
    Value value;
  } key_value;

  class linked {
  public:
    key_value value;
    linked *next;
  };

  int hash_function(string key, int list_size) {
    int hash_index = 0;
    for (char letter : key)
      hash_index += letter;
    return hash_index % list_size;
  }

public:
  hashmap() {
    map = NULL;
    map_size = 0;
  };

  key_value *map;

  int hash_insert(string key, Value value) {
    map_size++;
    if (map == NULL) {
      map = (key_value *)malloc(sizeof(key_value));
      cout << "struct size: " << sizeof(key_value) << endl;
    }
    int hash_index = hash_function(key, map_size);

    return EXIT_SUCCESS;
  }
};

int main() {
  hashmap<int> hashmap1;
  hashmap1.hash_insert("string key", 78);
  return EXIT_SUCCESS;
}
