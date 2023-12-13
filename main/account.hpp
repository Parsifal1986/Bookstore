#ifndef ACCOUNT_HPP
#define ACCOUNT_HPP

#include "memoryriver.hpp"
#include <iostream>
#include <map>
#include <cstdio>
#include <stack>
#include <cstring>
#include <set>
#include <string>

class User {
  public:
  User() = default;

  User(char *userid, char *password, char right, char *username);

  ~User() = default;
  
  void register_user(char *userid, char *password, char *username);

  void create_user(char *userid, char *password, char right, char *username);

  bool login(char *password);

  bool check_right(int needed_right);

  char* check_name() {
    return username;
  };

  private:
  char username[31] = {'\0'};
  char userid[31] = {'\0'};
  char password[31] = {'\0'};
  char right = '0';
};

class AccountManager {
 public: 

  AccountManager() = default;

  ~AccountManager() = default;

  bool add_account(User &user);

  bool delete_account(std::string userid);

  bool update_account(User &user);

  bool query_account(std::string userid);

 private:
  std::map<std::string, int> account;
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
    return userlist.top().check_right(needed_right);
  }

 private:
  AccountManager account;
  std::stack<User> userlist;
  std::set<std::string> check_log_or_not;
};

#endif // ACCOUNT_HPP