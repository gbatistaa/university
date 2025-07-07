#include <string>
#include <variant>
#include <vector>

using namespace std;

using variant_t = variant<string, bool, int, double>;
using numeric = variant<int, float, double>;

extern vector<vector<variant_t>> companies;

extern bool bigger_than(variant_t a, variant_t b);

extern bool smaller_than(variant_t a, variant_t b);

extern bool equal_to_(variant_t a, variant_t b);

template <typename Fn> extern void worker(Fn function);
