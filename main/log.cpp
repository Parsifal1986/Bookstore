#include "log.hpp"
#include <iomanip>

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

void LogManager::update_finance(double deal, bool type) {
  int finance_number;
  finance.get_info(finance_number, 1);
  std::pair<double, double> tmp;
  finance.read(tmp, (finance_number - 1) * sizeof(std::pair<double, double>) + sizeof(int));
  if (!type) {
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
  if (count == -1) {
    count = finance_number;
  }
  if (count > finance_number) {
    std::cout << "Invalid" << std::endl;
    return;
  }
  if (count == 0) {
    std::cout << std::endl;
    return;
  }
  std::pair<double, double> tmp1, tmp2;
  finance.read(tmp1, (finance_number - 1) * sizeof(std::pair<double, double>) + sizeof(int));
  finance.read(tmp2, (finance_number - count - 1) * sizeof(std::pair<double, double>) + sizeof(int));
  std::cout << "+ " << setiosflags(std::ios::fixed) << std::setprecision(2) << tmp1.first - tmp2.first
            << " - " << setiosflags(std::ios::fixed) << std::setprecision(2) << tmp1.second - tmp2.second << std::endl;
  return;
}