#include "data.h"
#include <vector>

using variant_t = variant<string, bool, int, double>;

bool bigger_than(variant_t a, variant_t b) { return a > b; }

bool smaller_than(variant_t a, variant_t b) { return a < b; }

bool equal_to_(variant_t a, variant_t b) { return a == b; }

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
