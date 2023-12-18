#include "log.hpp"

// void LogManager::add_log(char *log) {
//   this->log.push_back(log);
//   return;
// }

// void LogManager::show_log() {
//   for (auto i : this->log) {
//     std::cout << i << std::endl;
//   }
//   return;
// }

void LogManager::update_finance(float deal, bool type) {
  int finance_number;
  finance.get_info(finance_number, 1);
  std::pair<int, int> tmp;
  finance.read(tmp, finance_number * sizeof(std::pair<int, int>) + sizeof(int));
  if (type) {
    tmp.first += deal;
  } else {
    tmp.second += deal;
  }
  finance.write(tmp);
  finance.write_info(finance_number + 1, 1);
  return;
}

void LogManager::show_finance(int count) {
  int finance_number;
  finance.get_info(finance_number, 1);
  if (count > finance_number) {
    std::cout << "Invalid" << std::endl;
    return;
  }
  if (count == 0) {
    std::cout << std::endl;
    return;
  }
  std::pair<int, int> tmp1, tmp2;
  finance.read(tmp1, finance_number * sizeof(std::pair<int, int>) + sizeof(int));
  finance.read(tmp2, (finance_number - count) * sizeof(std::pair<int, int>) + sizeof(int));
  std::cout << "+" << tmp2.first - tmp1.first << " -" << tmp2.second - tmp1.second << std::endl;
  return;
}