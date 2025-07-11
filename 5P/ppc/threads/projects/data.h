#include <string>
#include <vector>

using namespace std;

typedef struct sale {
  string product_name;
  float price;
} Sale;

extern vector<Sale> sales;
