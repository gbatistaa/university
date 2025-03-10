#include <chrono>
#include <cstdlib>
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <unistd.h>

using namespace std;
using namespace std::chrono;

class Data {
public:
  string *bytes = nullptr;
  int size = 0;
};

class DataList {
public:
  Data *datas = nullptr;
  int size = 0;
};

double getMemoryUsageMB() {
  ifstream fp("/proc/self/statm");
  long rss = 0L;
  string dummy;
  if (fp) {
    fp >> dummy >> rss;
  }
  return rss * sysconf(_SC_PAGESIZE) / (1024.0 * 1024.0);
}

class HuffmanNode {
public:
  char this_character;
  int this_frequency;
  HuffmanNode *this_left;
  HuffmanNode *this_right;

  HuffmanNode(char character, int frequency) {
    this_character = character;
    this_frequency = frequency;
    this_left = nullptr;
    this_right = nullptr;
  }
};

class PriorityQueue {
private:
  struct QueueNode {
    HuffmanNode *this_node;
    QueueNode *this_next;
  };
  QueueNode *this_front;

public:
  PriorityQueue() { this_front = nullptr; }

  void this_insert(HuffmanNode *newNode) {
    QueueNode *newQueueNode = new QueueNode{newNode, nullptr};
    if (!this_front ||
        newNode->this_frequency < this_front->this_node->this_frequency) {
      newQueueNode->this_next = this_front;
      this_front = newQueueNode;
    } else {
      QueueNode *current = this_front;
      while (current->this_next &&
             current->this_next->this_node->this_frequency <=
                 newNode->this_frequency) {
        current = current->this_next;
      }
      newQueueNode->this_next = current->this_next;
      current->this_next = newQueueNode;
    }
  }

  HuffmanNode *this_extract_min() {
    if (!this_front)
      return nullptr;
    HuffmanNode *minNode = this_front->this_node;
    QueueNode *temp = this_front;
    this_front = this_front->this_next;
    delete temp;
    return minNode;
  }

  bool this_is_empty() { return this_front == nullptr; }
};

class HuffmanTree {
private:
  HuffmanNode *this_root;

  void this_generate_codes(HuffmanNode *node, string code, string codes[256]) {
    if (!node)
      return;
    if (!node->this_left && !node->this_right) {
      codes[(unsigned char)node->this_character] = code;
    }
    if (node->this_left) {
      this_generate_codes(node->this_left, code + "0", codes);
    }
    if (node->this_right) {
      this_generate_codes(node->this_right, code + "1", codes);
    }
  }

public:
  HuffmanTree() { this_root = nullptr; }

  void this_build(const string &text) {
    int frequency[256] = {0};
    for (char ch : text) {
      frequency[(unsigned char)ch]++;
    }

    PriorityQueue queue;
    for (int i = 0; i < 256; i++) {
      if (frequency[i] > 0) {
        queue.this_insert(new HuffmanNode((char)i, frequency[i]));
      }
    }

    while (!queue.this_is_empty()) {
      HuffmanNode *left = queue.this_extract_min();
      if (queue.this_is_empty()) {
        this_root = left;
        break;
      }
      HuffmanNode *right = queue.this_extract_min();
      HuffmanNode *merged =
          new HuffmanNode('\0', left->this_frequency + right->this_frequency);
      merged->this_left = left;
      merged->this_right = right;
      queue.this_insert(merged);
    }
  }

  string this_compress(Data data) {
    string codes[256];
    this_generate_codes(this_root, "", codes);
    string data_compress;
    for (int i = 0; i < data.size; i++) {
      for (int j = 0; j < data.bytes[i].length(); j++) {
        data_compress += codes[(unsigned char)data.bytes[i].at(j)];
      }
    }
    return data_compress;
  }
};

string RLE(Data data) {
  int qty = 1;
  string curr_byte = data.bytes[0], data_compress = "";
  for (int i = 1; i < data.size; i++) {
    if (data.bytes[i] == data.bytes[i - 1]) {
      qty++;
    } else {
      stringstream ss;
      ss << hex << setw(2) << setfill('0') << qty;
      data_compress += ss.str() + curr_byte;
      qty = 1, curr_byte = data.bytes[i];
    }
  }
  stringstream ss;
  ss << hex << setw(2) << setfill('0') << qty;
  data_compress += ss.str() + curr_byte;

  return data_compress;
}

int read_file(ifstream &input, DataList *&data_list) {
  string line = "";

  // Allocating memoy for the datas list:
  getline(input, line);
  int data_list_size = stoi(line);
  data_list->size = data_list_size;
  data_list->datas = new Data[data_list_size];

  // Allocating memory for each data set:
  for (int i = 0; getline(input, line); i++) {
    istringstream iss(line);
    iss >> data_list->datas[i].size;
    data_list->datas[i].bytes = new string[data_list->datas[i].size];

    int j = 0;
    while (iss >> data_list->datas[i].bytes[j]) {
      j++;
    }
  }

  return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {

  string output_string = "";
  double ram_before = getMemoryUsageMB();
  auto start = high_resolution_clock::now();

  ifstream input(argv[1]);
  ofstream output(argv[2]);

  if (!input.is_open()) {
    cerr << "Erro ao abrir input" << endl;
    return EXIT_FAILURE;
  }
  cout << "Input aberto com sucesso!" << endl;

  if (!output.is_open()) {
    cerr << "Erro ao abrir output" << endl;
    return EXIT_FAILURE;
  }
  cout << "Output aberto com sucesso!\n" << endl;

  DataList *data_list = new DataList();

  read_file(input, data_list);

  for (int i = 0; i < data_list->size; i++) {
    string compress_rle = RLE(data_list->datas[i]);
    output_string += to_string(i) + "->RLE=" + compress_rle + "\n";
  }

  output << output_string;

  auto end = high_resolution_clock::now();
  duration<double> duration = end - start;

  double ram_after = getMemoryUsageMB();

  cout << "Execution time: " << duration.count() << " s" << endl;
  cout << "Memory Usage: " << (ram_after - ram_before) << " MB" << endl;

  return EXIT_SUCCESS;
}
