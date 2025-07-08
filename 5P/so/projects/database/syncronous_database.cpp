#include "data.h"
#include <atomic>
#include <barrier>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <mutex>
#include <semaphore>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <variant>
#include <vector>

using namespace std;

const int THREADS_AVAILABLE = thread::hardware_concurrency();
const int SEMAPHORE_MAX = THREADS_AVAILABLE - 2;

using variant_t = variant<string, bool, int, double>;

// Entidades de exclusão mútua
mutex barrier_mutex;
mutex console_mutex;
barrier<> *threads_barrier = nullptr;
counting_semaphore<6> read_operations_sem(SEMAPHORE_MAX);

// Variáveis de controle de exclusão mútua
atomic_int threads_not_executed = THREADS_AVAILABLE;
atomic_int threads_inside_barrier = 0;

// Tipo de linha
class table_line : public unordered_map<string, variant_t> {};

class table {
public:
  string table_name;
  vector<string> table_props;
  vector<table_line> table_lines;

  void init_table(vector<string> props, string table_name) {
    this->table_name = table_name;
    vector<string> props_complete = props;
    props_complete.insert(props_complete.begin(), "id");
    this->table_props = props_complete;
  }

  void insert(vector<variant_t> values) {
    if (values.size() > 0 && values.size() == this->table_props.size() - 1) {
      table_line new_line;
      new_line[this->table_props.at(0)] = (int)this->table_lines.size();
      for (int i = 1; i < this->table_props.size(); i++)
        new_line[table_props.at(i)] = values.at(i - 1);
      this->table_lines.push_back(new_line);
    }
  }

  void delete_where(string prop, variant_t expected_value) {
    for (auto it = this->table_lines.begin(); it != this->table_lines.end();) {
      if (it->at(prop) == expected_value)
        it = this->table_lines.erase(it);
      else
        ++it;
    }
  }

  void delete_all() {
    console_mutex.lock();
    this->table_lines.clear();
    cout << "\033[32mAll elements were successfully removed!\033[0m\n";
    console_mutex.unlock();
  }

  void update_to(string prop, variant_t expected_value,
                 unordered_map<string, variant_t> new_values) {
    vector<int> updated_lines;

    // Atualiza as propriedades e guarda índices que foram atualizados:
    for (int i = 0; i < this->table_lines.size(); i++) {
      table_line &line = this->table_lines.at(i);
      if (line.at(prop) == expected_value) {
        for (const auto &[prop_changed, new_value] : new_values) {
          line[prop_changed] = new_value;
        }
        updated_lines.push_back(i);
      }
    }

    // Leitura dos dados atualizados
    console_mutex.lock();
    for (int i : updated_lines) {
      const table_line &line = this->table_lines.at(i);
      cout << "(";
      visit([](auto &&value) { cout << value; }, line.at("id"));
      cout << ") Changed Props: | ";
      for (const auto &[prop_changed, new_value] : new_values) {
        cout << "\033[32m" << prop_changed << ": ";
        visit([](auto &&v) { cout << "\033[33m" << v << "\033[0m | "; },
              new_value);
      }
      cout << endl;
    }
    console_mutex.unlock();
  }

  void find_all() {

    if (!this->table_lines.empty()) {
      for (const auto &line : this->table_lines) {
        cout << "(";
        visit(
            [](auto &&value) {
              if constexpr (is_same_v<decay_t<decltype(value)>, int>)
                cout << (value + 1);
              else
                cout << value;
            },
            line.at("id"));
        cout << ") ";
        for (size_t j = 1; j < this->table_props.size(); j++) {
          cout << table_props.at(j) << ": ";
          visit([](auto &&v) { cout << v; }, line.at(table_props.at(j)));
          if (j < this->table_props.size() - 1)
            cout << " | ";
        }
        cout << endl;
        usleep(100000);
      }
    } else {
      cout << "\033[31mThere is no elements on table " << this->table_name
           << ".\033[0m\n";
    }
  }

  template <typename Fn>
  void find_where(string prop, Fn function, variant_t expected_value) {
    // Bloqueando a escrita no console por outras threads
    console_mutex.lock();

    for (const auto &line : this->table_lines) {
      if (function(line.at(prop), expected_value)) {
        cout << "(";
        visit([](auto &&value) { cout << (std::get<int>(value) + 1); },
              line.at("id"));
        cout << ") ";
        for (size_t j = 1; j < this->table_props.size(); j++) {
          cout << table_props.at(j) << ": ";
          visit([](auto &&value) { cout << value; },
                line.at(table_props.at(j)));
          if (j < this->table_props.size() - 1)
            cout << " | ";
        }
        cout << endl << endl;
      }
    }

    console_mutex.unlock();
  }
};

// Agora o map guarda shared_ptr<table>
template <typename T> class tables_map {
public:
  unordered_map<string, shared_ptr<T>> tables;

  void create_table(string table_name, vector<string> table_props) {
    shared_ptr<T> new_table = make_shared<T>();
    new_table->init_table(table_props, table_name);
    this->tables.insert({table_name, new_table});
    // cout << "\033[32mTabela " << table_name << " criada com
    // sucesso!\033[0m"<<endl<<endl;
  }
};

// Instância global do banco
tables_map<table> database;

template <typename Fn> void read_routine(int id, Fn function) {

  barrier<> *group_barrier = nullptr;

  read_operations_sem.acquire();

  console_mutex.lock();
  cout << "\033[32mThread " << "\033[33m" << id
       << "\033[32m acquired the semaphore!\033[0m\n";
  console_mutex.unlock();

  // Antes de começarem o processamento de leitura, todas as threads
  // entrarão juntas na região crítica, com uma barreira de sincronização
  barrier_mutex.lock();

  if (threads_inside_barrier == 0) {
    int barrier_capacity = SEMAPHORE_MAX < threads_not_executed
                               ? (int)(SEMAPHORE_MAX)
                               : (int)(threads_not_executed);

    delete threads_barrier;

    // Criação de nova barreira com o máximo de threads possível
    // (máximo ou disponível)
    threads_barrier = new barrier<>(barrier_capacity);
    threads_inside_barrier = barrier_capacity;
  }

  group_barrier = threads_barrier;
  threads_inside_barrier--;
  threads_not_executed--;

  barrier_mutex.unlock();

  // A thread atual espera o restante das threads chegar até aqui
  group_barrier->arrive_and_wait();

  console_mutex.lock();
  cout << endl
       << "\033[32mThread " << "\033[33m" << id
       << " \033[32m começando leitura!\033[0m" << endl;
  function();
  console_mutex.unlock();
  read_operations_sem.release();

  console_mutex.lock();
  cout << "\033[31mThread " << "\033[33m" << id
       << "\033[31m released the semaphore!\033[0m\n";
  console_mutex.unlock();
}

int main() {
  vector<string> stock_props = {
      "ticker", "company_name", "price", "market_cap", "volume",
  };

  database.create_table("stocks", stock_props);

  // Inserção dos dados
  for (const auto &company : companies) {
    database.tables.at("stocks")->insert(company);
  }

  // Criação de threads
  cout << "\033[33m" << THREADS_AVAILABLE
       << "\033[32m threads criadas com sucesso!\033[0m" << endl
       << endl;

  vector<thread> threads;
  for (int i = 0; i < THREADS_AVAILABLE; i++) {

    threads.emplace_back([i]() {
      read_routine(i + 1, []() { database.tables.at("stocks")->find_all(); });
    });
  }

  for (auto &t : threads) {
    t.join();
  }

  return EXIT_SUCCESS;
}
