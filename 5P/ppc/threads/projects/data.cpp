#include <string>
#include <vector>

using namespace std;

typedef struct sale {
  string product_name;
  float price;
} Sale;

vector<Sale> sales = {
    {"Arroz 5kg", 24.99},
    {"Feijão Carioca 1kg", 7.49},
    {"Macarrão Espaguete 500g", 3.79},
    {"Açúcar Refinado 1kg", 4.59},
    {"Café Torrado 500g", 16.90},
    {"Óleo de Soja 900ml", 7.89},
    {"Farinha de Trigo 1kg", 5.25},
    {"Leite Integral 1L", 4.99},
    {"Manteiga 200g", 10.50},
    {"Pão de Forma 500g", 8.49},
    {"Queijo Mussarela 500g", 22.75},
    {"Presunto Cozido 500g", 19.99},
    {"Refrigerante 2L", 8.99},
    {"Água Mineral 1.5L", 2.49},
    {"Suco de Uva Integral 1L", 12.80},
    {"Biscoito Recheado 140g", 2.99},
    {"Chocolate Barra 90g", 5.89},
    {"Sabonete 90g", 1.99},
    {"Shampoo 350ml", 12.45},
    {"Condicionador 350ml", 13.20},
    {"Detergente 500ml", 2.39},
    {"Sabão em Pó 1kg", 9.90},
    {"Amaciante 2L", 11.75},
    {"Papel Higiênico (4 rolos)", 6.80},
    {"Escova de Dente", 3.75},
    {"Pasta de Dente 90g", 4.29},
    {"Desodorante Aerosol", 10.95},
    {"Arroz Integral 1kg", 6.79},
    {"Feijão Preto 1kg", 6.99},
    {"Molho de Tomate 340g", 2.59},
    {"Ketchup 400g", 5.49},
    {"Maionese 500g", 6.29},
    {"Mostarda 200g", 3.89},
    {"Sal Refinado 1kg", 2.99},
    {"Achocolatado em Pó 400g", 7.90},
    {"Iogurte Natural 170g", 2.59},
    {"Banana (1kg)", 4.49},
    {"Maçã (1kg)", 6.39},
    {"Carne Moída 1kg", 29.90},
    {"Frango Inteiro 1kg", 12.80},
};
