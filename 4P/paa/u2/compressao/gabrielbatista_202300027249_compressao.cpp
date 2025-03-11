#include <chrono>
#include <fstream>
#include <iostream>
#include <string>

// Struct for Huffman tree nodes
struct node {
  int freq;
  unsigned char symbol; // Only for leaf nodes
  node *left;
  node *right;
  bool is_leaf;
};

// Struct to store Huffman codes
struct code {
  int length;
  unsigned char bits[32]; // Enough for 256 bits (32 bytes * 8)
};

// Global variables to track memory usage
size_t total_memory_allocated = 0;

// Custom memory allocation to track usage
node *allocate_node() {
  node *ptr = new node;
  total_memory_allocated += sizeof(node);
  return ptr;
}

// Convert a 2-character hex string to a byte
unsigned char from_hex(const std::string &hex) {
  unsigned char b = 0;
  for (int i = 0; i < 2; i++) {
    char c = hex[i];
    int val;
    if (c >= '0' && c <= '9')
      val = c - '0';
    else if (c >= 'A' && c <= 'F')
      val = 10 + (c - 'A');
    else if (c >= 'a' && c <= 'f')
      val = 10 + (c - 'a');
    else
      val = 0; // Invalid input fallback
    b = (b << 4) | val;
  }
  return b;
}

// Print a byte as a 2-character hex string to the output stream
void print_hex(std::ofstream &out_file, unsigned char b) {
  char hex_digits[] = "0123456789ABCDEF";
  out_file << hex_digits[(b >> 4) & 0xF];
  out_file << hex_digits[b & 0xF];
}

// Min-heap insertion for Huffman tree construction
void insert_heap(node **heap, int &heap_size, node *new_node) {
  heap[heap_size] = new_node;
  int idx = heap_size;
  heap_size++;
  while (idx > 0) {
    int parent_idx = (idx - 1) / 2;
    if (heap[parent_idx]->freq > heap[idx]->freq) {
      node *temp = heap[parent_idx];
      heap[parent_idx] = heap[idx];
      heap[idx] = temp;
      idx = parent_idx;
    } else {
      break;
    }
  }
}

// Extract the minimum frequency node from the heap
node *extract_min(node **heap, int &heap_size) {
  if (heap_size == 0)
    return nullptr;
  node *min_node = heap[0];
  heap[0] = heap[heap_size - 1];
  heap_size--;
  int idx = 0;
  while (true) {
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;
    int smallest = idx;
    if (left < heap_size && heap[left]->freq < heap[smallest]->freq) {
      smallest = left;
    }
    if (right < heap_size && heap[right]->freq < heap[smallest]->freq) {
      smallest = right;
    }
    if (smallest == idx)
      break;
    node *temp = heap[idx];
    heap[idx] = heap[smallest];
    heap[smallest] = temp;
    idx = smallest;
  }
  return min_node;
}

// Assign Huffman codes recursively
void assign_codes(node *curr_node, int bit_pos, unsigned char *current_bits,
                  code *codes) {
  if (curr_node->is_leaf) {
    codes[curr_node->symbol].length = bit_pos;
    for (int i = 0; i < (bit_pos + 7) / 8; i++) {
      codes[curr_node->symbol].bits[i] = current_bits[i];
    }
  } else {
    int byte_idx = bit_pos / 8;
    int bit_idx = bit_pos % 8;
    unsigned char mask = 1 << bit_idx;
    // Left child: append 0
    current_bits[byte_idx] &= ~mask;
    assign_codes(curr_node->left, bit_pos + 1, current_bits, codes);
    // Right child: append 1
    current_bits[byte_idx] |= mask;
    assign_codes(curr_node->right, bit_pos + 1, current_bits, codes);
  }
}

// Append a Huffman code to the bitstream
void add_code(const code &code, unsigned char *compressed, int &byte_pos,
              int &bit_pos) {
  for (int i = 0; i < code.length; i++) {
    int bit = (code.bits[i / 8] >> (i % 8)) & 1;
    compressed[byte_pos] |= (bit << bit_pos);
    bit_pos++;
    if (bit_pos == 8) {
      byte_pos++;
      bit_pos = 0;
      if (byte_pos < 10000)
        compressed[byte_pos] = 0;
    }
  }
}

// Print compression rate with two decimal places to the output stream
void print_rate(std::ofstream &out_file, double rate) {
  int integer_part = (int)rate;
  double fractional = rate - integer_part;
  int decimal_part = (int)(fractional * 100);
  out_file << integer_part << ".";
  if (decimal_part < 10)
    out_file << "0";
  out_file << decimal_part;
}

int main(int argc, char *argv[]) {
  // Check if correct number of arguments are provided
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>\n";
    return 1;
  }

  // Open input file
  std::ifstream in_file(argv[1]);
  if (!in_file) {
    std::cerr << "Error: Cannot open input file " << argv[1] << "\n";
    return 1;
  }

  // Open output file
  std::ofstream out_file(argv[2]);
  if (!out_file) {
    std::cerr << "Error: Cannot open output file " << argv[2] << "\n";
    in_file.close();
    return 1;
  }

  int n;
  in_file >> n;

  // Start measuring execution time
  auto start_time = std::chrono::high_resolution_clock::now();

  for (int seq_idx = 0; seq_idx < n; seq_idx++) {
    int length;
    in_file >> length;
    unsigned char sequence[10000];
    total_memory_allocated +=
        sizeof(unsigned char) * 10000; // Track memory for sequence array
    for (int i = 0; i < length; i++) {
      std::string hex;
      in_file >> hex;
      sequence[i] = from_hex(hex);
    }

    // RLE Compression
    unsigned char rle_output[20000]; // Worst case: 2 * length
    total_memory_allocated +=
        sizeof(unsigned char) * 20000; // Track memory for rle_output array
    int rle_size = 0;
    int pos = 0;
    while (pos < length) {
      unsigned char current_byte = sequence[pos];
      int count = 0;
      while (pos < length && sequence[pos] == current_byte && count < 255) {
        pos++;
        count++;
      }
      rle_output[rle_size++] = (unsigned char)count;
      rle_output[rle_size++] = current_byte;
    }

    // Huffman Compression
    int freq[256] = {0};
    total_memory_allocated += sizeof(int) * 256; // Track memory for freq array
    for (int i = 0; i < length; i++) {
      freq[sequence[i]]++;
    }

    node *heap[256];
    total_memory_allocated +=
        sizeof(node *) * 256; // Track memory for heap array
    int heap_size = 0;
    for (int i = 0; i < 256; i++) {
      if (freq[i] > 0) {
        node *new_node =
            allocate_node(); // Use custom allocation to track memory
        new_node->freq = freq[i];
        new_node->symbol = (unsigned char)i;
        new_node->left = nullptr;
        new_node->right = nullptr;
        new_node->is_leaf = true;
        insert_heap(heap, heap_size, new_node);
      }
    }

    while (heap_size > 1) {
      node *node1 = extract_min(heap, heap_size);
      node *node2 = extract_min(heap, heap_size);
      node *combined = allocate_node(); // Use custom allocation to track memory
      combined->freq = node1->freq + node2->freq;
      combined->symbol = 0;
      combined->left = node1;
      combined->right = node2;
      combined->is_leaf = false;
      insert_heap(heap, heap_size, combined);
    }
    node *huffman_root = (heap_size > 0) ? heap[0] : nullptr;

    code huffman_codes[256] = {0};
    total_memory_allocated +=
        sizeof(code) * 256; // Track memory for huffman_codes array
    unsigned char current_bits[32] = {0};
    total_memory_allocated +=
        sizeof(unsigned char) * 32; // Track memory for current_bits array
    if (huffman_root) {
      assign_codes(huffman_root, 0, current_bits, huffman_codes);
    }

    unsigned char huffman_output[10000] = {0};
    total_memory_allocated +=
        sizeof(unsigned char) * 10000; // Track memory for huffman_output array
    int byte_pos = 0;
    int bit_pos = 0;
    for (int i = 0; i < length; i++) {
      add_code(huffman_codes[sequence[i]], huffman_output, byte_pos, bit_pos);
    }
    int huffman_size = (bit_pos == 0) ? byte_pos : byte_pos + 1;

    // Calculate compression rates
    double rle_rate = (rle_size / (double)length) * 100.0;
    double huffman_rate = (huffman_size / (double)length) * 100.0;

    // Output results to file
    out_file << seq_idx << " -> ";
    if (huffman_size < rle_size) {
      out_file << "HUF (";
      print_rate(out_file, huffman_rate);
      out_file << "%) = ";
      for (int i = 0; i < huffman_size; i++) {
        print_hex(out_file, huffman_output[i]);
      }
      out_file << "\n";
    } else if (rle_size < huffman_size) {
      out_file << "RLE (";
      print_rate(out_file, rle_rate);
      out_file << "%) = ";
      for (int i = 0; i < rle_size; i++) {
        print_hex(out_file, rle_output[i]);
      }
      out_file << "\n";
    } else {
      // HUF first
      out_file << "HUF (";
      print_rate(out_file, huffman_rate);
      out_file << "%) = ";
      for (int i = 0; i < huffman_size; i++) {
        print_hex(out_file, huffman_output[i]);
      }
      out_file << "\n" << seq_idx << " -> RLE (";
      print_rate(out_file, rle_rate);
      out_file << "%) = ";
      for (int i = 0; i < rle_size; i++) {
        print_hex(out_file, rle_output[i]);
      }
      out_file << "\n";
    }
  }

  // Stop measuring execution time
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      end_time - start_time);
  double execution_time_ms =
      duration.count() / 1000.0; // Convert to milliseconds

  // Log execution time and memory usage to the output file
  out_file << "\n[Log]\n";
  out_file << "Execution Time: " << execution_time_ms << " ms\n";
  out_file << "Estimated Memory Usage: " << total_memory_allocated
           << " bytes\n";

  // Close files
  in_file.close();
  out_file.close();

  return 0;
}
