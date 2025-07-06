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
  vector<string> table_props; // Lista de propriedades da tabela

  vector<table_line> table_lines; // Lista de linhas das tabelas

  // Iniciar a tabela adicionando os campos de propriedades
  void init_table(vector<string> props)
  {
    this->table_props = props;
  }

  // Função de inserir dados na tabela
  void insert(vector<variant_t> values)
  {
    if (values.size() == this->table_props.size())
    {
      table_line new_line;
      for (int i = 0; i < this->table_props.size(); i++)
        new_line[table_props.at(i)] = values.at(i);
      table_lines.push_back(new_line);
    }
  }

  void find_all()
  {
    for (int i = 0; i < this->table_lines.size(); i++)
    {
      table_line line = this->table_lines.at(i);
      cout << "(" << i << ") ";
      for (int j = 0; j < this->table_props.size(); j++)
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

  void find_where(string prop, variant_t expected_value)
  {
    for (int i = 0; i < this->table_lines.size(); i++)
    {
      table_line line = this->table_lines.at(i);
      if (line.at(prop) == expected_value)
      {
        cout << "(" << i << ") ";
        for (int j = 0; j < this->table_props.size(); j++)
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
    new_table.init_table(table_props);
    this->tables.insert({table_name, new_table});

    return;
  }
};

int main()
{
  tables_map<table> database;

  vector<string> student_props = {"name", "age", "cpf", "iea", "code"};
  database.create_table("students", student_props);

  vector<variant_t> student1 = {"Gabriel Batista Barbosa", 21, "05354093546", 7.5396, "202300027249"};
  vector<variant_t> student2 = {"Larissa Mendes Rocha", 22, "12439876532", 8.1342, "202200018734"};
  vector<variant_t> student3 = {"Pedro Henrique Silva Costa", 21, "89765432109", 6.4287, "202300006513"};
  vector<variant_t> student4 = {"Ana Carolina Lima Souza", 19, "34561278900", 9.0021, "202400001234"};
  vector<variant_t> student5 = {"Lucas Fernando Oliveira", 23, "76321049875", 7.7854, "202100023981"};

  database.tables.at("students").insert(student1);
  database.tables.at("students").insert(student2);
  database.tables.at("students").insert(student3);
  database.tables.at("students").insert(student4);
  database.tables.at("students").insert(student5);

  database.tables.at("students").find_all();
  database.tables.at("students").find_where("age", 21);

  return EXIT_SUCCESS;
}
