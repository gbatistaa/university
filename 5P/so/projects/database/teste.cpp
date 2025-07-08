#include <chrono> // necessário para std::this_thread::sleep_for
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

void thread_function(int id, const std::vector<std::string> &names) {
  std::string filename = "output-" + std::to_string(id) + ".txt";
  std::ofstream file(filename);

  if (file.is_open()) {
    file << "Thread ID: " << id << "\n";
    file << "Nomes:\n";
    for (const auto &name : names) {
      file << "- " << name << "\n";
      file.flush(); // garante que o conteúdo seja escrito imediatamente
      std::this_thread::sleep_for(
          std::chrono::milliseconds(50)); // delay de 50ms
    }
    file.close();
  } else {
    std::cerr << "Erro ao abrir o arquivo: " << filename << "\n";
  }
}

int main() {
  unsigned int num_threads = 3;
  if (num_threads == 0)
    num_threads = 4; // fallback

  std::vector<std::thread> threads;

  std::vector<std::string> names = {
      "Ana",     "Bruno",   "Carlos",  "Daniela",  "Eduardo", "Fernanda",
      "Gabriel", "Helena",  "Igor",    "Juliana",  "Karen",   "Lucas",
      "Mariana", "Nicolas", "Olívia",  "Paulo",    "Quésia",  "Rafael",
      "Sabrina", "Tiago",   "Ursula",  "Vinicius", "Wesley",  "Xavier",
      "Yasmin",  "Zuleica", "Arthur",  "Bianca",   "Caio",    "Debora",
      "Elias",   "Fátima",  "Giovana", "Hugo",     "Isabela", "Jorge",
      "Katia",   "Leandro", "Manuela", "Nelson",   "Otávio",  "Priscila",
      "Renato",  "Simone",  "Talita",  "Ulisses",  "Valéria", "William",
      "Yuri",    "Zara"};

  for (unsigned int i = 0; i < num_threads; ++i) {
    threads.emplace_back(thread_function, i, names);
  }

  for (auto &t : threads) {
    t.join();
  }

  std::cout << "Arquivos com 50 nomes criados por " << num_threads
            << " threads.\n";
  return 0;
}
