#include <string>
#include <variant>
#include <vector>

using namespace std;

using variant_t = variant<string, bool, int, double>;
using numeric = variant<int, float, double>;

extern vector<vector<variant_t>> companies;

extern vector<vector<variant_t>> group1;
extern vector<vector<variant_t>> group2;
extern vector<vector<variant_t>> group3;
extern vector<vector<variant_t>> group4;

// Vetor com os 4 grupos
extern vector<vector<vector<variant_t>>> companies_extra;

extern bool is_bigger_than(variant_t a, variant_t b);

extern bool is_smaller_than(variant_t a, variant_t b);

extern bool is_equal_to(variant_t a, variant_t b);

extern void thread_msg(string msg, int id);

template <typename Fn> extern void worker(Fn function);
