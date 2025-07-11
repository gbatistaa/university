#include <chrono>             // Para this_thread::sleep_for, chrono::seconds
#include <condition_variable> // Para condition_variable
#include <cstdlib>            // Para rand()
#include <ctime>              // Para time(NULL)
#include <iostream>           // Para cout, endl
#include <mutex>              // Para mutex, lock_guard
#include <queue>              // Para queue
#include <thread>             // Para thread

using namespace std;

mutex cout_mutex;

void bem_vindo() {
  cout << "**********************************************" << endl;
  cout << "**  Bem vindo à Barbearia dos Dorminhocos!  **" << endl;
  cout << "**********************************************" << endl << endl;
}

void print_sync(const string &texto) {
  lock_guard<mutex> lock(cout_mutex);
  cout << texto << endl;
}

string vermelho(string texto) { return "\033[1;31m" + texto + "\033[0m"; }

string verde(string texto) { return "\033[1;32m" + texto + "\033[0m"; }

string amarelo(string texto) { return "\033[1;33m" + texto + "\033[0m"; }

string azul(string texto) { return "\033[1;34m" + texto + "\033[0m"; }

void barbeiro(string nome, queue<int> &clientes, mutex &mtx) {
  print_sync(verde(nome + " está pronto para cortar cabelos"));

  while (true) {
    unique_lock<mutex> lock(mtx);

    if (!clientes.empty()) {
      int cliente_id = clientes.front();
      clientes.pop();
      lock.unlock();
      string cortando = amarelo(nome + " está cortando o cabelo do cliente " +
                                to_string(cliente_id));
      print_sync(cortando);
      cout << endl;
      int tempo = rand() % 6 + 5; // 5 a 10 segundos
      this_thread::sleep_for(chrono::seconds(tempo));
      string liberado =
          azul("- - - Cliente " + to_string(cliente_id) + " liberado - - -");
      print_sync(liberado);
      cout << endl;
    } else {
      lock.unlock();
      string dormindo = vermelho(nome + " dormindo...");
      print_sync(dormindo);
      this_thread::sleep_for(chrono::seconds(1));
    }
  }
}

void cliente_generator(queue<int> &clientes, int limite, mutex &mtx) {
  int cliente_id = 1;

  while (true) {
    int tempo = rand() % 3 + 4; // 4 a 6 segundos
    this_thread::sleep_for(chrono::seconds(tempo));

    lock_guard<mutex> lock(mtx); // Protege o acesso à fila

    if (clientes.size() < limite) {
      clientes.push(cliente_id);
      string chegou =
          verde("- - - Cliente " + to_string(cliente_id) + " chegou - - -");
      print_sync(chegou);
      cout << endl;
      cliente_id++;
    } else {
      string cheia =
          vermelho("- - - Fila cheia. Cliente " + to_string(cliente_id) +
                   " não entrou na fila - - -");
      print_sync(cheia);
      cout << endl;
    }
  }
}

int main() {
  bem_vindo();

  queue<int> clientes;
  int limite = 10;
  mutex mtx;

  // Inicializa semente para números aleatórios
  srand(time(NULL));

  // Iniciar a thread do barbeiro 1
  thread t1(barbeiro, string("Barbeiro 1"), ref(clientes), ref(mtx));

  // Iniciar a thread do barbeiro 2
  thread t2(barbeiro, string("Barbeiro 2"), ref(clientes), ref(mtx));

  // Iniciar a thread para gerar clientes
  thread t3(cliente_generator, ref(clientes), limite, ref(mtx));

  // Esperar a thread terminar
  t1.join();
  t2.join();
  t3.join();

  cout << "Fim do dia!" << endl;

  return 0;
}
