#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <random>
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
    map_size = (size_t)map_size * 2;
    linked_pair *new_map = (linked_pair *)calloc(map_size, sizeof(linked_pair));

    // Re-hashing all the elements:
    for (int i = 0; i < prev_map_size; i++) {

      // Ignoring the empty spaces:
      if (map[i].pair.key == "")
        continue;

      // Copying the current previous map element:
      linked_pair *curr_copy = &map[i];
      while (curr_copy != nullptr) {

        int hash_index = hash_function(curr_copy->pair.key, map_size);
        linked_pair *curr_pair = &new_map[hash_index];

        // Verifying if already has element in the hash index:
        if (curr_pair->pair.key != "") {
          linked_pair *copy_pair = (linked_pair *)malloc(sizeof(linked_pair));
          copy_pair->pair = curr_copy->pair;
          copy_pair->next = nullptr;

          // Scanning until it reaches the last element of the linked list:
          while (curr_pair->next != nullptr) {
            curr_pair = curr_pair->next;
          }
          curr_pair->next = copy_pair;
        } else {
          curr_pair->pair = curr_copy->pair;
          curr_pair->next = nullptr;
        }

        // Passing to the next element of the previous map colision linked list:
        curr_copy = curr_copy->next;
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
        cout << map[i].pair.key << " | " << map[i].pair.value;
        linked_pair *curr_pair = &map[i];

        // Scanning all of the collided pairs:
        while (curr_pair->next != nullptr && curr_pair->next->pair.key != "") {
          cout << " --> " << curr_pair->next->pair.key << " | "
               << curr_pair->next->pair.value;
          curr_pair = curr_pair->next;
        }
        cout << endl;
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
        while (curr_pair != nullptr) {
          if (curr_pair->pair.key == key) {
            return curr_pair->pair.value;
          }
          curr_pair = curr_pair->next;
        }

        // If reaches a null pointer the key were not created:
        throw runtime_error("Key not found");
      } catch (const exception &e) {
        cerr << e.what() << endl;
      }
    }
    return Value();
  }
};

string generate_random_string(size_t length) {
  const string charset =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  random_device rd;
  mt19937 generator(rd());
  uniform_int_distribution<> distribution(0, charset.size() - 1);

  string random_string;
  for (size_t i = 0; i < length; ++i) {
    random_string += charset[distribution(generator)];
  }
  return random_string;
}

int main() {
  hashmap<int> hashmap1;

  size_t hash_map_size;
  cout << "Type the random generated strings quantity: ";
  cin >> hash_map_size;

  random_device rd;
  mt19937 generator(rd());
  uniform_int_distribution<> length_dist(6, 20);
  uniform_int_distribution<> value_dist(1, 1000);

  for (int i = 0; i < hash_map_size; ++i) {
    size_t random_length = length_dist(generator);
    string random_string = generate_random_string(random_length);
    int random_value = value_dist(generator);
    hashmap1.hash_insert(random_string, random_value);
  }

  hashmap1.print_hash_map();

  return EXIT_SUCCESS;
}
