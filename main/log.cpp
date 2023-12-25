#include "log.hpp"
#include "memoryriver.hpp"
#include <cstring>
#include <iomanip>

void LogManager::add_log(LogMessage &log) {
  int log_number;
  log_database.get_info(log_number, 1);
  log_database.write(log);
  log_database.write_info(log_number + 1, 1);
  return;
}

void LogManager::show_log() {
  int log_number;
  log_database.get_info(log_number, 1);
  if (log_number == 0) {
    std::cout << std::endl;
    return;
  }
  for (int i = 0; i < log_number; ++i) {
    LogMessage tmp;
    log_database.read(tmp, i * sizeof(LogMessage) + sizeof(int));
    std::cout << tmp;
  }
  return;
}

void LogManager::add_log(LogMessage &log, std::string admin_name) {
  int index = admin_map[admin_name];
  Admin tmp;
  admin_linklist.add(log, index);
  return;
}

void LogManager::add_admin(std::string admin) {
  int admin_number;
  admin_index.get_info(admin_number, 1);
  int index = admin_linklist.create_head();
  Admin tmp;
  tmp.index = index;strcpy(tmp.username, admin.c_str());
  admin_index.write(tmp);
  admin_map.insert(std::pair<std::string, int>(admin, index));
  admin_index.write_info(admin_number + 1, 1);
  return;
}

void LogManager::show_log(std::string admin_name) {
  int index = admin_map[admin_name];
  admin_linklist.show_node(index);
  return;
}

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

void LogManager::report_finance() {
  int finance_number;
  finance.get_info(finance_number, 1);
  if (finance_number == 0) {
    std::cout << std::endl;
    return;
  }
  std::pair<double, double> tmp1, tmp2;
  finance.read(tmp2, sizeof(int));
  std::cout << "+ " << setiosflags(std::ios::fixed) << std::setprecision(2) << tmp2.first
            << " - " << setiosflags(std::ios::fixed) << std::setprecision(2) << tmp2.second << std::endl;
  for (int i = 1; i <= finance_number; ++i) {
    finance.read(tmp2, i * sizeof(std::pair<double, double>) + sizeof(int));
    tmp1 = tmp2;
    std::cout << "+ " << setiosflags(std::ios::fixed) << std::setprecision(2) << tmp2.first - tmp1.first
              << " - " << setiosflags(std::ios::fixed) << std::setprecision(2) << tmp2.second - tmp1.second << std::endl;
  }
  return;
}