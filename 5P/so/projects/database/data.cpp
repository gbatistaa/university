#include "data.h"
#include <iostream>
#include <variant>
#include <vector>

using variant_t = variant<string, bool, int, double>;

bool is_bigger_than(variant_t a, variant_t b) {
  return visit(
      [](auto &&a_val, auto &&b_val) -> bool {
        using T = std::decay_t<decltype(a_val)>;
        using U = std::decay_t<decltype(b_val)>;

        // Comparação direta se os tipos forem iguais
        if constexpr (std::is_same_v<T, U>) {
          return a_val > b_val;
        }
        // Conversão implícita para double se ambos forem numéricos
        else if constexpr ((std::is_same_v<T, int> ||
                            std::is_same_v<T, double>) &&
                           (std::is_same_v<U, int> ||
                            std::is_same_v<U, double>)) {
          return static_cast<double>(a_val) > static_cast<double>(b_val);
        }
        // Comparação de strings
        else if constexpr (std::is_same_v<T, string> &&
                           std::is_same_v<U, string>) {
          return a_val > b_val;
        }
        // Comparação de bools
        else if constexpr (std::is_same_v<T, bool> && std::is_same_v<U, bool>) {
          return a_val > b_val;
        }
        // Outras combinações não são comparáveis
        else {
          return false; // Ou lançar uma exceção, dependendo do comportamento
                        // desejado
        }
      },
      a, b);
}
bool is_smaller_than(variant_t a, variant_t b) {
  return visit(
      [](auto &&a_val, auto &&b_val) -> bool {
        using T = std::decay_t<decltype(a_val)>;
        using U = std::decay_t<decltype(b_val)>;

        // Comparação direta se os tipos forem iguais
        if constexpr (std::is_same_v<T, U>) {
          return a_val < b_val;
        }
        // Conversão implícita para double se ambos forem numéricos
        else if constexpr ((std::is_same_v<T, int> ||
                            std::is_same_v<T, double>) &&
                           (std::is_same_v<U, int> ||
                            std::is_same_v<U, double>)) {
          return static_cast<double>(a_val) < static_cast<double>(b_val);
        }
        // Comparação de strings
        else if constexpr (std::is_same_v<T, string> &&
                           std::is_same_v<U, string>) {
          return a_val < b_val;
        }
        // Comparação de bools
        else if constexpr (std::is_same_v<T, bool> && std::is_same_v<U, bool>) {
          return a_val < b_val;
        }
        // Outras combinações não são comparáveis
        else {
          return false; // Ou lançar uma exceção, dependendo do comportamento
                        // desejado
        }
      },
      a, b);
}

bool is_equal_to(variant_t a, variant_t b) { return a == b; }

void thread_msg(string msg, int id) {
  if (id > 0) {
    cout << endl
         << "\033[32mThread " << "\033[33m" << id << " \033[32m fazendo " << msg
         << "!\033[0m" << endl;
  }
};

vector<vector<variant_t>> companies = {
    {"PETR4", "Petrobras", 29.50, 70.4, 32000000},
    {"VALE3", "Vale S.A.", 85.10, 110.7, 25000000},
    {"ITUB4", "Itaú Unibanco", 24.20, 40.1, 35000000},
    {"ABEV3", "Ambev", 15.60, 25.8, 28000000},
    {"MGLU3", "Magazine Luiza", 3.70, 8.9, 40000000},

    {"AAPL", "Apple Inc.", 170.00, 2700.0, 70000000},
    {"MSFT", "Microsoft Corp.", 310.50, 2300.0, 35000000},
    {"AMZN", "Amazon.com Inc.", 125.30, 1600.0, 45000000},
    {"GOOGL", "Alphabet Inc.", 2700.0, 1800.0, 15000000},
    {"TSLA", "Tesla Inc.", 700.0, 900.0, 30000000},

    {"BBAS3", "Banco do Brasil", 32.20, 15.6, 12000000},
    {"BBDC4", "Bradesco", 21.10, 22.3, 27000000},
    {"NTCO3", "Natura", 11.50, 6.2, 9000000},
    {"GGBR4", "Gerdau", 11.00, 10.4, 8500000},
    {"EMBR3", "Embraer", 19.20, 3.5, 13000000},

    {"META", "Meta Platforms", 190.0, 900.0, 20000000},
    {"KO", "Coca-Cola", 56.0, 240.0, 16000000},
    {"NFLX", "Netflix", 400.0, 180.0, 12000000},
    {"IBM", "IBM Corp.", 140.0, 125.0, 8000000},
    {"JNJ", "Johnson & Johnson", 170.0, 430.0, 11000000},

    {"BRDT3", "Petrobras Distribuidora", 24.5, 2.2, 5000000},
    {"SUZB3", "Suzano Papel", 45.0, 30.0, 7500000},
    {"CIEL3", "Cielo", 4.5, 5.0, 22000000},
    {"LREN3", "Lojas Renner", 30.0, 12.5, 9000000},
    {"RENT3", "Localiza", 50.0, 20.0, 10000000},

    {"BA", "Boeing", 210.0, 140.0, 9000000},
    {"INTC", "Intel Corp.", 54.0, 210.0, 23000000},
    {"MCD", "McDonald's", 280.0, 170.0, 7000000},
    {"NKE", "Nike Inc.", 150.0, 200.0, 8000000},
    {"PFE", "Pfizer Inc.", 45.0, 250.0, 28000000},
};

vector<vector<variant_t>> group1 = {
    {"ORCL", "Oracle Corporation", 130.5, 350.0, 10000000},
    {"SAP", "SAP SE", 155.0, 190.0, 5000000},
    {"CRM", "Salesforce Inc.", 215.3, 210.0, 8000000},
    {"UBER", "Uber Technologies", 45.2, 90.0, 25000000},
    {"LYFT", "Lyft Inc.", 12.5, 25.0, 6000000},
};

vector<vector<variant_t>> group2 = {
    {"AMD", "Advanced Micro Devices", 110.7, 180.0, 20000000},
    {"NVDA", "NVIDIA Corp.", 730.0, 3000.0, 27000000},
    {"QCOM", "Qualcomm Inc.", 145.0, 400.0, 9000000},
    {"TXN", "Texas Instruments", 165.0, 480.0, 7500000},
    {"ADBE", "Adobe Inc.", 500.5, 1200.0, 6000000},
};

vector<vector<variant_t>> group3 = {
    {"CSCO", "Cisco Systems", 52.3, 210.0, 14000000},
    {"SONY", "Sony Group", 98.5, 115.0, 4000000},
    {"PANW", "Palo Alto Networks", 240.0, 300.0, 4500000},
    {"SHOP", "Shopify Inc.", 65.0, 70.0, 10000000},
    {"SQ", "Block Inc.", 60.0, 35.0, 8000000},
};

vector<vector<variant_t>> group4 = {
    {"DIS", "Walt Disney", 90.0, 160.0, 12000000},
    {"PEP", "PepsiCo", 170.0, 230.0, 9000000},
    {"WMT", "Walmart Inc.", 140.0, 400.0, 15000000},
    {"PG", "Procter & Gamble", 160.0, 350.0, 11000000},
    {"CVX", "Chevron Corp.", 160.0, 320.0, 10000000},
};

vector<vector<vector<variant_t>>> companies_extra = {
    group1,
    group2,
    group3,
    group4,
};
