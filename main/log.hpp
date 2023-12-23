#ifndef LOG_HPP
#define LOG_HPP

#include "memoryriver.hpp"
#include <iostream>
#include <cstdio>

class LogManager {
 public:
  LogManager() {
    finance.initialise("finance");
  };

  ~LogManager() = default;

  void add_log(char *log);

  void show_log();

  void update_finance(double deal, bool type);

  void show_finance(int count);

 private:
  MemoryRiver<std::pair<double, double>, 1> finance;
};



#endif //LOG_HPP