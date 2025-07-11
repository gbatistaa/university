#include <atomic>
#include <condition_variable>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#define QUEUE_CAPACITY 10

using namespace std;

mutex sales_queue_mutex;

condition_variable empty;
condition_variable full;

atomic<int> queue_size;

typedef struct sale {
  string product_name;
  float price;
} Sale;

const int THREADS_NUM = thread::hardware_concurrency();

queue<Sale> sales_queue;

void producer(int id, Sale sale) {
  unique_mutex<mutex> lock(sales_queue_mutex);

  while (sales_queue.size() < QUEUE_CAPACITY) {
    full.wait(lock, []() { cout << "\033[31mA"; });
  }

  return;
}

int main() {
  cout << "Hello World!" << endl;
  cout << sales_queue.empty() << endl;

  return EXIT_SUCCESS;
}
