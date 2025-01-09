#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

template <typename Value> class hashmap {
private:
  size_t map_size;
  size_t elements_num;
  float density;

  typedef struct key_value {
    string key;
    Value value;
  } key_value;

  class linked_pair {
  public:
    key_value pair;
    linked_pair *next;
  };

  bool isGreaterThan(float a, float b, float e = 1e-6) {
    return (a > b) && fabs(a - b) > e;
  }

  int resize_hash() { return EXIT_SUCCESS; }

  int hash_function(string key, int list_size) {
    int hash_index = 0;
    for (char letter : key)
      hash_index += letter;
    return hash_index % list_size;
  }

  // Hash Map constructor:
public:
  hashmap() {
    map = nullptr;
    map_size = 0;
  };

  linked_pair *map;

  int hash_insert(string key, Value value) {
    elements_num++;

    // Storing the new key value pair:
    key_value kv;
    kv.key = key;
    kv.value = value;

    linked_pair *new_pair = (linked_pair *)malloc(sizeof(linked_pair));
    new_pair->pair = kv;
    new_pair->next = nullptr;

    // Case when the hashmap has 0 elements
    if (map == nullptr) {
      map_size++;
      map = new_pair;
    } else {
      density = (float)elements_num / map_size;

      // Condition to verify if the resizing is needed (more than 75%):
      cout << map_size << endl;
      if (isGreaterThan(density, 0.75)) {
        // Doubleing the size of the hashmap array:
        size_t prev_map_size = map_size;
        map_size *= 2;
        linked_pair *new_map =
            (linked_pair *)calloc(map_size, sizeof(linked_pair));

        cout << "Rodou" << endl;

        // Re-hashing all the elements:
        for (int i = 0; i < prev_map_size; i++) {
          int hash_index = hash_function(map[i].pair.key, map_size);

          // Colision treatment:
          if (new_map[hash_index].pair.key != "") {
            linked_pair *curr_pair = &new_map[hash_index];
            while (curr_pair->next != nullptr) {
              cout << "current pair: " << curr_pair << endl;
              curr_pair = curr_pair->next;
            }
            curr_pair->next = new_pair;
          } else {
            new_map[hash_index] = *new_pair;
          }
        }
        map = new_map;
      } else {
      }
      cout << map_size << endl;
    }
    return EXIT_SUCCESS;
  }
};

int main() {
  hashmap<int> hashmap1;
  hashmap1.hash_insert("kleber", 78);
  hashmap1.hash_insert("gabriel", 20);
  hashmap1.hash_insert("julia", 51);
  hashmap1.hash_insert("samara", 8);
  return EXIT_SUCCESS;
}
