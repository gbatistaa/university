#include "data.h"
#include <atomic>
#include <condition_variable>
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
const int MAX_READERS = THREADS_AVAILABLE - 2;

using variant_t = variant<string, bool, int, double>;

// Sistema de sincronização corrigido
mutex console_mutex;
mutex control_mutex;          // Controla acesso às variáveis de estado
condition_variable reader_cv; // Para leitores esperarem
condition_variable writer_cv; // Para escritores esperarem

atomic<int> active_readers{0};     // Leitores ativos
atomic<int> waiting_readers{0};    // Leitores esperando
atomic<int> waiting_writers{0};    // Escritores esperando
atomic<bool> writer_active{false}; // Se há escritor ativo

counting_semaphore<> reader_semaphore(MAX_READERS);

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

  void insert(vector<variant_t> values, int thread_id) {
    if (values.size() > 0 && values.size() == this->table_props.size() - 1) {
      table_line new_line;
      new_line[this->table_props.at(0)] = (int)this->table_lines.size();
      for (int i = 1; i < this->table_props.size(); i++)
        new_line[table_props.at(i)] = values.at(i - 1);
      this->table_lines.push_back(new_line);
    }
    usleep(100000);
  }

  void delete_where(string prop, variant_t expected_value, int thread_id) {
    for (auto it = this->table_lines.begin(); it != this->table_lines.end();) {
      if (it->at(prop) == expected_value)
        it = this->table_lines.erase(it);
      else
        ++it;
    }
    {
      lock_guard<mutex> console_lock(console_mutex);
      cout << "Thread " << thread_id << " realizou deleção." << endl;
    }
  }

  void delete_all(int thread_id) {
    this->table_lines.clear();
    {
      lock_guard<mutex> console_lock(console_mutex);
      cout << "Thread " << thread_id << " realizou deleção geral." << endl;
    }
  }

  void update_to(string prop, variant_t expected_value, table_line new_values,
                 int thread_id) {
    vector<table_line> updated_lines;

    // Atualiza as propriedades e guarda linhas que foram atualizadas
    for (auto &line : this->table_lines) {
      if (line.at(prop) == expected_value) {
        for (const auto &[prop_changed, new_value] : new_values) {
          line[prop_changed] = new_value;
        }
        updated_lines.push_back(line);
      }
    }

    usleep(100000);

    {
      lock_guard<mutex> console_lock(console_mutex);
      cout << "Thread " << thread_id << " realizou atualização.\n";

      // Leitura dos dados atualizados
      for (const auto &updated_line : updated_lines) {
        cout << "(";
        visit([](auto &&value) { cout << value; }, updated_line.at("id"));
        cout << ") Changed Props: | ";
        for (const auto &[prop_changed, new_value] : new_values) {
          cout << "\033[32m" << prop_changed << ": ";
          visit([](auto &&v) { cout << "\033[33m" << v << "\033[0m | "; },
                new_value);
        }
        cout << endl;
      }
    }
  }

  void find_all(int thread_id) {
    lock_guard<mutex> console_lock(console_mutex);
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
  void find_where(string prop, Fn function, variant_t expected_value,
                  int thread_id) {
    lock_guard<mutex> console_lock(console_mutex);
    for (const auto &line : this->table_lines) {
      if (function(line.at(prop), expected_value)) {
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
          visit([](auto &&value) { cout << value; },
                line.at(table_props.at(j)));
          if (j < this->table_props.size() - 1)
            cout << " | ";
        }
        cout << endl;
      }
    }
  }
};

template <typename T> class tables_map {
public:
  unordered_map<string, shared_ptr<T>> tables;

  void create_table(string table_name, vector<string> table_props) {
    shared_ptr<T> new_table = make_shared<T>();
    new_table->init_table(table_props, table_name);
    this->tables.insert({table_name, new_table});
    cout << "\033[32mTabela " << table_name << " criada com sucesso!\033[0m"
         << endl
         << endl;
  }
};

tables_map<table> database;

// Função para aquisição de leitura - implementa o padrão readers-writers com
// prioridade para escritores
void acquire_read_lock(int thread_id) {
  // Primeiro adquire o semáforo que limita o número de leitores
  reader_semaphore.acquire();

  unique_lock<mutex> lock(control_mutex);
  waiting_readers++;

  // Espera até que:
  // 1. Não haja escritor ativo
  // 2. Não haja escritores esperando (prioridade para escritores)
  reader_cv.wait(
      lock, []() { return !writer_active.load() && !waiting_writers.load(); });

  waiting_readers--;
  active_readers++;
}

// Função para liberação de leitura
void release_read_lock(int thread_id) {
  unique_lock<mutex> lock(control_mutex);
  active_readers--;

  // Se foi o último leitor, acorda escritores esperando
  if (active_readers == 0) {
    writer_cv.notify_one();
  }

  lock.unlock();
  reader_semaphore.release();
}

// Função para aquisição de escrita
void acquire_write_lock(int thread_id) {
  unique_lock<mutex> lock(control_mutex);
  waiting_writers++;

  // Espera até que:
  // 1. Não haja leitores ativos
  // 2. Não haja escritor ativo
  writer_cv.wait(lock, []() {
    return active_readers.load() == 0 && !writer_active.load();
  });

  waiting_writers--;
  writer_active = true;
}

// Função para liberação de escrita
void release_write_lock(int thread_id) {
  unique_lock<mutex> lock(control_mutex);
  writer_active = false;

  // Prioridade para escritores: se há escritores esperando, acorda um escritor
  // Caso contrário, acorda todos os leitores
  if (waiting_writers > 0) {
    writer_cv.notify_one();
  } else {
    reader_cv.notify_all();
  }
}

// Rotina corrigida para leitura e escrita
template <typename Fn>
void routine(int id, Fn function, bool is_writer = false) {
  if (is_writer) {
    // Operação de escrita
    acquire_write_lock(id);

    // Executa a função de escrita com acesso exclusivo
    function();

    release_write_lock(id);

    {
      lock_guard<mutex> lock(console_mutex);
      cout << "\033[31mThread " << id
           << " realizou operação de escrita e liberou o recurso.\033[0m\n";
    }

  } else {
    // Operação de leitura
    acquire_read_lock(id);

    // Executa a função de leitura (pode haver múltiplos leitores)
    function();

    release_read_lock(id);

    {
      lock_guard<mutex> lock(console_mutex);
      cout << "\033[32mThread " << id
           << " realizou operação de leitura e liberou o recurso.\033[0m\n";
    }
  }
}

int main() {
  vector<string> stock_props = {
      "ticker", "company_name", "price", "market_cap", "volume",
  };

  database.create_table("stocks", stock_props);
  usleep(500000);

  // Inserção dos dados iniciais
  for (const auto &company : companies) {
    database.tables.at("stocks")->insert(company, 0);
  }

  cout << "\033[33m" << THREADS_AVAILABLE
       << "\033[32m threads criadas com sucesso!\033[0m" << endl
       << endl;

  vector<thread> threads;

  int i = 1;
  // Threads de leitura (primeira parte)
  for (; i <= THREADS_AVAILABLE / 4; i++) {
    threads.emplace_back([i]() {
      routine(i, [i]() { database.tables.at("stocks")->find_all(i); }, false);
    });
  }

  // Threads de escrita
  for (int j = 0; i <= THREADS_AVAILABLE - 2; i++, j++) {
    threads.emplace_back([i, j]() {
      routine(
          i,
          [i, j]() {
            for (const auto &new_company : companies_extra.at(j)) {
              database.tables.at("stocks")->insert(new_company, i);
            }
          },
          true);
    });
  }

  // Threads de leitura (segunda parte)
  for (; i <= THREADS_AVAILABLE; i++) {
    threads.emplace_back([i]() {
      routine(
          i,
          [i]() {
            database.tables.at("stocks")->find_where("price", is_bigger_than,
                                                     30, i);
          },
          false);
    });
  }

  for (auto &t : threads) {
    t.join();
  }

  return EXIT_SUCCESS;
}
