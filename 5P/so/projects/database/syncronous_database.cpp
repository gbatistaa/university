#include "data.h"
#include <barrier>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <semaphore>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <variant>
#include <vector>

using namespace std;

const int MAX_THREADS = thread::hardware_concurrency();

using variant_t = variant<string, bool, int, double>;

mutex console_mutex;
barrier syncronization_barrier(MAX_THREADS - 2);
counting_semaphore<6> read_operations_sem(MAX_THREADS - 2);

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
    console_mutex.lock();
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
    console_mutex.lock();

    if (!this->table_lines.empty()) {
      for (const auto &line : this->table_lines) {
        cout << "(";
        visit([](auto &&v) { cout << v; }, line.at("id"));
        cout << ") ";
        for (size_t j = 1; j < this->table_props.size(); j++) {
          cout << table_props.at(j) << ": ";
          visit([](auto &&v) { cout << v; }, line.at(table_props.at(j)));
          if (j < this->table_props.size() - 1)
            cout << " | ";
        }
        cout << endl;
        usleep(50000);
      }
    } else {
      cout << "\033[31mThere is no elements on table " << this->table_name
           << ".\033[0m\n";
    }
    console_mutex.unlock();
  }

  template <typename Fn>
  void find_where(string prop, Fn function, variant_t expected_value) {
    // Bloqueando a escrita no console por outras threads
    console_mutex.lock();

    for (const auto &line : this->table_lines) {
      if (function(line.at(prop), expected_value)) {
        cout << "(";
        visit([](auto &&value) { cout << value; }, line.at("id"));
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
    cout << "\033[32mTabela " << table_name << " criada com sucesso!\033[0m\n";
  }
};

// Instância global do banco
tables_map<table> database;

void read_routine(int id) {
  read_operations_sem.acquire();

  cout << "\033[32mThread " << "\033[33m" << id
       << "\033[32m acquired the semaphore!\033[0m\n";

  // Antes de começarem o processamento de leitura, todas as threads todas
  // entrarão juntas na região crítica, com uma barreira de sincronização
  syncronization_barrier.arrive_and_wait();

  database.tables.at("stocks")->find_all();

  read_operations_sem.release();

  cout << "\033[31mThread " << "\033[33m" << id
       << "\033[31m released the semaphore!\033[0m\n";
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
  vector<thread> threads;
  for (int i = 0; i < MAX_THREADS - 2; i++) {
    threads.emplace_back(read_routine, i + 1);
  }

  for (auto &t : threads) {
    t.join();
  }

  return EXIT_SUCCESS;
}
