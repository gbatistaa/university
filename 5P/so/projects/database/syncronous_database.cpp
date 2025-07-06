#include <cstdlib>
#include <iostream>
#include <semaphore>
#include <unordered_map>
#include <variant>
#include <iterator>
#include <vector>

using namespace std;

// Tipo customizado que permite uma variável receber vários tipos de dados

using variant_t = variant<string, bool, int, double>;

counting_semaphore<2> sem(2);

// Classe de tipagem de cada linha da tabela
class table_line : public unordered_map<string, variant_t>
{
};

class table
{

public:
  string table_name;
  vector<string> table_props;     // Lista de propriedades da tabela
  vector<table_line> table_lines; // Lista de linhas das tabelas

  // Iniciar a tabela adicionando os campos de propriedades
  void init_table(vector<string> props, string table_name)
  {
    this->table_name = table_name;
    vector<string>
        props_complete = props;
    // Adiciona por padrão a propriedade id a todos as tabelas criadas
    props_complete.insert(props_complete.begin(), "id");
    this->table_props = props_complete;
  }

  // Função de inserir dados na tabela (sem id)
  void insert(vector<variant_t> values)
  {
    if (values.size() > 0 && values.size() == this->table_props.size() - 1)
    {
      table_line new_line;
      new_line[this->table_props.at(0)] = (int)this->table_lines.size();
      for (int i = 1; i < this->table_props.size(); i++)
        new_line[table_props.at(i)] = values.at(i - 1);
      this->table_lines.push_back(new_line);
    }
  }

  void delete_where(string prop, variant_t expected_value)
  {
    for (auto it = this->table_lines.begin(); it != this->table_lines.end();)
    {
      if (it->at(prop) == expected_value)
        it = this->table_lines.erase(it); // erase retorna o próximo iterador válido
      else
        it++;
    }
  }

  void delete_all()
  {
    this->table_lines.clear();
    cout << "\033[32mAll elements were successfully removed!\033[0m" << endl;
  }

  void update_to(string prop, variant_t expected_value, unordered_map<string, variant_t> new_values)
  {
    for (table_line line : this->table_lines)
    {
      if (line.at(prop) == expected_value)
      {
        cout << "(";
        visit([](auto &&value)
              { cout << value; }, line.at("id"));
        cout << ") Changed Props: |";
        for (const auto &[prop_changed, new_value] : new_values)
        {
          line[prop_changed] = new_value;
          cout << "\033[32m" << prop_changed + ": ";
          visit([](auto &&value)
                { cout << "\033[33m" << value << "|\033[m"; }, new_value);
        }
        cout << endl;
      }
    }
  }

  void find_all()
  {
    if (!this->table_lines.empty())
    {
      for (int i = 0; i < this->table_lines.size(); i++)
      {
        table_line line = this->table_lines.at(i);
        cout << "(";
        visit([](auto &&value)
              { cout << value; }, line.at("id"));
        cout << ") ";
        for (int j = 1; j < this->table_props.size(); j++)
        {
          cout << table_props.at(j) << ": ";
          visit([](auto &&value)
                { cout << value; }, line[table_props.at(j)]); // Imprime o valor com base no seu tipo
          if (j < this->table_props.size() - 1)
            cout << " | ";
        }
        cout << endl;
      }
    }
    else
    {
      cout << "\033[31mThere is no elements on table " << this->table_name << ".\033[0m" << endl;
    }
  }

  void find_where(string prop, variant_t expected_value)
  {
    for (int i = 0; i < this->table_lines.size(); i++)
    {
      table_line line = this->table_lines.at(i);
      if (line.at(prop) == expected_value)
      {
        cout << "(";
        visit([](auto &&value)
              { cout << value; }, line.at("id"));
        cout << ") ";
        for (int j = 1; j < this->table_props.size(); j++)
        {
          cout << table_props.at(j) << ": ";
          visit([](auto &&value)
                { cout << value; }, line[table_props.at(j)]); // Imprime o valor com base no seu tipo
          if (j < this->table_props.size() - 1)
            cout << " | ";
        }
        cout << endl;
      }
    }
  }
};

// Classe de tipagem do hashmap de tabelas
template <typename T>
class tables_map
{
public:
  unordered_map<string, T> tables;

  void create_table(string table_name, vector<string> table_props)
  {
    table new_table;
    new_table.init_table(table_props, table_name);
    this->tables.insert({table_name, new_table});

    return;
  }
};

int main()
{
  tables_map<table> database;

  vector<string> student_props = {"name", "age", "cpf", "iea", "code", "approved"};
  database.create_table("students", student_props);

  vector<variant_t> student1 = {"Gabriel Batista Barbosa", 21, "05354093546", 7.5396, "202300027249", true};
  vector<variant_t> student2 = {"Larissa Mendes Rocha", 22, "12439876532", 8.1342, "202200018734", true};
  vector<variant_t> student3 = {"Pedro Henrique Silva Costa", 21, "89765432109", 6.4287, "202300026513", false};
  vector<variant_t> student4 = {"Ana Carolina Lima Souza", 19, "34561278900", 9.0021, "202400091234", true};
  vector<variant_t> student5 = {"Lucas Fernando Oliveira", 23, "76321049875", 7.7854, "202100023981", false};

  database.tables.at("students").insert(student1);
  database.tables.at("students").insert(student2);
  database.tables.at("students").insert(student3);
  database.tables.at("students").insert(student4);
  database.tables.at("students").insert(student5);

  database.tables.at("students").find_all();
  cout << endl;
  database.tables.at("students").find_where("age", 21);
  cout << endl;

  database.tables.at("students").delete_where("age", 23);
  cout << endl;

  database.tables.at("students").find_all();
  cout << endl;

  unordered_map<string, variant_t> new_values1 = {
      {"age", 22},
      {"iea", 9.75},
      {"approved", false},
  };
  database.tables.at("students").update_to("name", "Gabriel Batista Barbosa", new_values1);
  cout << endl;

  database.tables.at("students").find_all();
  cout << endl;

  database.tables.at("students").delete_all();
  cout << endl;

  database.tables.at("students").find_all();

  return EXIT_SUCCESS;
}
