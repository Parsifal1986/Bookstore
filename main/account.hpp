#ifndef ACCOUNT_HPP
#define ACCOUNT_HPP

#include "book.hpp"
#include "memoryriver.hpp"
#include "tokenscanner.hpp"
#include "log.hpp"
#include <iostream>
#include <map>
#include <cstdio>
#include <stack>
#include <cstring>
#include <set>
#include <string>

extern Tokenscanner tokenscanner;

class User {
  public:
  User() = default;

  User(char *userid, char *password, char right, char *username);

  ~User() = default;
  
  void register_user(char *userid, char *password, char *username);

  void create_user(char *userid, char *password, char right, char *username);

  bool check_password(char *password);

  bool check_right(int needed_right);

  void change_password(char *password) {
    std::strcpy(this->password, password);
  }

  char read_right() {
    return right;
  }

  char* check_id() {
    return userid;
  };

  private:
  char username[31] = {'\0'};
  char userid[31] = {'\0'};
  char password[31] = {'\0'};
  char right = '0';
};

class AccountManager {
 public: 

  AccountManager();

  ~AccountManager() = default;

  void add_account(User &user);

  void delete_account(std::string userid);

  void update_account(User &user);

  int query_account(std::string userid);

  User draw_account(int index);

 private:
  std::map<std::string, int> account_base;
  MemoryRiver<User, 1> memoryriver;
};

class AccountOperator {
 public:
  AccountOperator();

  ~AccountOperator() = default;

  void register_user();

  void add_user();

  void delete_user();

  void modify_user();

  void login();

  void logout();

  bool check_right(int needed_right) {
    if (userlist.empty()) {
      return false;
    }
    return userlist.top().check_right(needed_right);
  }

 private:
  AccountManager account;
  std::stack<User> userlist;
  std::set<std::string> check_log_or_not;
};

#endif // ACCOUNT_HPP