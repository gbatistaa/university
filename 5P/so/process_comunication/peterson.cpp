#include <atomic>
#include <cstdlib>
#include <iostream>
#include <thread>

// Usando alias para chrono e this_thread
using namespace std;
using std::cout;
using std::endl;
using std::this_thread::sleep_for;

int main() {
  atomic<bool> interested[2];
  atomic<int> turn;

  auto enterRegion = [&](int process) {
    int other = 1 - process;
    interested[process] = true;
    turn = process;
    while (interested[other] && turn == process) {
      // busy wait
    }
  };

  auto leaveRegion = [&](int process) { interested[process] = false; };

  auto criticalSection = [&](int process) {
    for (int i = 0; i < 5; ++i) {
      enterRegion(process);

      cout << "Processo " << process << " na região crítica. Iteração " << i + 1
           << endl;

      sleep_for(100ms);

      leaveRegion(process);

      sleep_for(50ms);
    }
  };

  thread t0(criticalSection, 0);
  thread t1(criticalSection, 1);

  t0.join();
  t1.join();

  return EXIT_SUCCESS;
}
