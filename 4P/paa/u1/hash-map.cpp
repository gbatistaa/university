#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

template <typename Value> class hashmap {
private:
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

  int re_hash() {
    // Doubleing the size of the hashmap array:
    size_t prev_map_size = map_size;
    map_size = (size_t)map_size * sqrt(5);
    linked_pair *new_map = (linked_pair *)calloc(map_size, sizeof(linked_pair));

    // Re-hashing all the elements:
    for (int i = 0; i < prev_map_size; i++) {

      // Ignoring the empty spaces:
      if (map[i].pair.key == "")
        continue;

      int hash_index = hash_function(map[i].pair.key, map_size);

      // Copying the current previous map element:
      linked_pair *copy_pair = (linked_pair *)malloc(sizeof(linked_pair));
      copy_pair->pair.key = map[i].pair.key;
      copy_pair->pair.value = map[i].pair.value;
      copy_pair->next = map[i].next;

      // Colision treatment:
      if (new_map[hash_index].pair.key != "") {
        linked_pair *curr_pair = &new_map[hash_index];
        // Scanning until it reaches the last element of the linked list:
        while (curr_pair->next != nullptr) {
          curr_pair = curr_pair->next;
        }
        curr_pair->next = copy_pair;
      } else {
        new_map[hash_index] = *copy_pair;
      }
    }
    map = new_map;

    return EXIT_SUCCESS;
  }

  int hash_function(string key, int list_size) {
    int hash_index = 0;
    for (char letter : key)
      hash_index += letter;
    return hash_index % list_size;
  }

public:
  size_t map_size;

  // Hash Map constructor:
  hashmap() {
    elements_num = 0;
    map_size = 2;
    map = (linked_pair *)calloc(map_size, sizeof(linked_pair));
  };

  linked_pair *map;

  linked_pair *hash_insert(string key, Value value) {
    elements_num++;

    // Storing the new key value pair:
    key_value kv;
    kv.key = key;
    kv.value = value;

    linked_pair *new_pair = (linked_pair *)malloc(sizeof(linked_pair));
    new_pair->pair = kv;
    new_pair->next = nullptr;

    // Case when the hashmap has 0 elements
    density = (float)elements_num / map_size;

    // Condition to verify if the resizing is needed (more than 75%):
    if (isGreaterThan(density, 0.75)) {
      re_hash();
    }

    int hash_index = hash_function(key, map_size);

    // Case that the hash index collides:
    if (map[hash_index].pair.key != "") {
      linked_pair *curr_pair = &map[hash_index];
      while (curr_pair->next != nullptr && curr_pair->pair.key != key) {
        curr_pair = curr_pair->next;
      }
      if (curr_pair->next == nullptr) {
        curr_pair->next = new_pair;
      } else {
        curr_pair->next->pair.value = value;
      }
    } else {
      map[hash_index] = *new_pair;
    }
    return map;
  }

  int print_hash_map() {
    for (int i = 0; i < map_size; i++) {
      if (map[i].pair.key != "") {
        cout << map[i].pair.key << " | " << map[i].pair.value << endl;
        linked_pair *curr_pair = &map[i];

        // Scanning all of the collided pairs:
        while (curr_pair->next != nullptr) {
          cout << curr_pair->next->pair.key << " | "
               << curr_pair->next->pair.value << endl;
          curr_pair = curr_pair->next;
        }
      }
    }
    return EXIT_SUCCESS;
  }

  Value at(string key) {
    int hash_index = hash_function(key, map_size);

    if (map[hash_index].pair.key == key)
      return map[hash_index].pair.value;
    else {
      linked_pair *curr_pair = &map[hash_index];
      try {
        if (curr_pair->next != nullptr) {
          while (curr_pair->next->pair.key != key) {
            if (curr_pair->next == nullptr)
              break;
            else
              curr_pair = curr_pair->next;
          }
          return curr_pair->next->pair.value;
        } else
          throw;
      } catch (const exception &e) {
        cerr << e.what() << endl;
      }
    }
    return EXIT_FAILURE;
  }
};

int main() {
  hashmap<int> hashmap1;
  hashmap1.hash_insert("kleber", 78);
  hashmap1.hash_insert("gabriel", 20);
  hashmap1.hash_insert("julia", 51);
  hashmap1.hash_insert("samara", 8);
  hashmap1.hash_insert("caio", 16);
  hashmap1.hash_insert("roberta", 192);
  hashmap1.hash_insert("victor", 48);
  hashmap1.hash_insert("sofhia", 91);

  hashmap1.print_hash_map();

  return EXIT_SUCCESS;
}
