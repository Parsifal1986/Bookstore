#ifndef LOG_HPP
#define LOG_HPP

#include "memoryriver.hpp"
#include <fstream>
#include <iostream>
#include <cstdio>
#include <map>
#include <ostream>
#include <string>

struct LogMessage {
  char log[200] = {'\0'};
  char username[31] = {'\0'};

  friend std::ostream & operator << (std::ostream & out, LogMessage &log) {
    out << log.username << log.log << std::endl;
    return out;
  };
};

struct Admin {
  char username[31] = {'\0'};
  int index = 0;
};

template <class T> class linklist {
 public:
  linklist() {
    file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    if (!file) {
      file.open(file_name, std::ios::out);
    }
    file.close();
  };

  ~linklist() = default;

  int create_head() {
    Node tmp;
    tmp.next = -1;
    file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    file.seekp(0, std::ios::end);
    int index = file.tellp();
    file.write(reinterpret_cast<char *>(&tmp), sizeofNode);
    file.close();
    return index;
  }

  void add(T &data, int index) {
    Node tmp;
    tmp.data = data;
    tmp.next = -1;
    file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    file.seekg(index, std::ios::beg);
    file.read(reinterpret_cast<char *>(&tmp), sizeofNode);
    while(tmp.next != -1) {
      file.seekg(tmp.next, std::ios::beg);
      file.read(reinterpret_cast<char *>(&tmp), sizeofNode);
    }
    int cur = int(file.tellg()) - sizeofNode;
    file.seekp(0, std::ios::end);
    tmp.next = file.tellp();
    Node new_node;
    new_node.data = data;
    new_node.next = -1;
    file.write(reinterpret_cast<char *>(&new_node), sizeofNode);
    file.seekp(cur, std::ios::beg);
    file.write(reinterpret_cast<char *>(&tmp), sizeofNode);
    file.close();
    return;
  }

  void show_node(int index) {
    Node tmp;
    file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    file.seekg(index * sizeofNode, std::ios::beg);
    file.read(reinterpret_cast<char *>(&tmp), sizeofNode);
    while(tmp.next != -1) {
      file.seekg(tmp.next * sizeofNode, std::ios::beg);
      file.read(reinterpret_cast<char *>(&tmp), sizeofNode);
      std::cout << tmp.data << std::endl;
    }
    file.close();
    return;
  }

 private:
  struct Node {
    T data;
    int next;
  };
  std::fstream file;
  std::string file_name = "admin_linklist";
  int sizeofNode = sizeof(Node);
};

class LogManager {
 public:
  LogManager() {
    finance.initialise("finance");
    log_database.initialise("log_database");
    admin_index.initialise("admin_index");
    int admin_number;
    admin_index.get_info(admin_number, 1);
    for (int i = 0; i < admin_number; ++i) {
      Admin tmp;
      admin_index.read(tmp, i * sizeof(Admin) + sizeof(int));
      admin_map.insert(std::pair<std::string, int>(tmp.username, tmp.index));
    };
  };

  ~LogManager() = default;

  void add_log(LogMessage &log);

  void add_log(LogMessage &log, std::string admin_name);

  void add_admin(std::string admin);

  void show_log();

  void show_log(std::string admin_name);

  void update_finance(double deal, bool type);

  void show_finance(int count);

  void report_finance();

 private:
  MemoryRiver<std::pair<double, double>, 1> finance;
  MemoryRiver<LogMessage, 1> log_database;
  MemoryRiver<Admin, 1> admin_index;
  std::map<std::string, int> admin_map;
  linklist<LogMessage> admin_linklist;
};

#endif //LOG_HPP