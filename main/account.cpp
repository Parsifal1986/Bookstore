#include "account.hpp"
#include "memoryriver.hpp"
#include <cstring>

User::User(char *userid, char *password, char right, char *username) {
  std::strcpy(this->userid, userid);
  std::strcpy(this->password, password);
  std::strcpy(this->username, username);
  this->right = right;
}

void User::register_user(char *userid, char *password, char *username){
  std::strcpy(this->userid, userid);
  std::strcpy(this->password, password);
  std::strcpy(this->username, username);
  this->right = '1';
  return;
}

void User::create_user(char *userid, char *password, char right, char *username) {
  std::strcpy(this->userid, userid);
  std::strcpy(this->password, password);
  std::strcpy(this->username, username);
  this->right = right;
  return;
}

bool User::login(char *password) {
  if (!(std::strcmp(this->password, password))) {
    std::cout << "welcome" << this->username << std::endl;
    return true;
  }
  return false;
}

bool User::check_right(int needed_right) {
  if (this->right < needed_right) {
    std::cout << "Permission denied" << std::endl;
    return false;
  }
  return true;
}

bool AccountManager::add_account(User &user){
  if (query_account(user.check_name())) {
    std::cout << "user already exists" << std::endl;
    return false;
  } else {
    int user_number;
    memoryriver.get_info(user_number,1);
    account.insert(std::pair<std::string, int>(user.check_name(), memoryriver.write(user)));
    memoryriver.write_info(user_number + 1, 1);
    return true;
  }
}

bool AccountManager::delete_account(std::string userid){
  if (!query_account(userid)) {
    std::cout << "user doesn't exist" << std::endl;
    return false;
  } else {
    User blank_user;
    int user_number;
    memoryriver.get_info(user_number, 1);
    memoryriver.Delete(account[userid]);
    memoryriver.write_info(user_number - 1, 1);
    account.erase(userid);
    return true;
  }
}

bool AccountManager::query_account(std::string userid){
  if (account.find(userid) == account.end()) {
    return false;
  } else {
    return true;
  }
}

bool AccountManager::update_account(User &user){
  if (!query_account(user.check_name())) {
    std::cout << "user doesn't exist" << std::endl;
    return false;
  } else {
    memoryriver.update(user, account[user.check_name()]);
    return true;
  }
}

AccountOperator::AccountOperator() {
  char userid[] = "root";
  char password[] = "sjtu";
  char username[] = "root";
  User user(userid, password, '7', username);
  account.add_account(user);
}

void AccountOperator::register_user() {
  char userid[31];
  char password[31];
  char username[31];
  std::cin >> userid >> password >> username;
  User user;
  user.register_user(userid, password, username);
  account.add_account(user);
  return;
}

void AccountOperator::add_user() {
  char userid[31];
  char password[31];
  char username[31];
  char right;
  std::cin >> userid >> password >> right >> username;
  if (userlist.empty()) {
    std::cout << "No account has log in" << std::endl;
    return;
  }
  if (!userlist.top().check_right(right)) {
    std::cout << "Permission denied" << std::endl;
    return;
  }
  User user;
  user.create_user(userid, password, right, username);
  account.add_account(user);
  return;
}

void AccountOperator::delete_user() {
  char userid[31];
  std::cin >> userid;
  if (check_log_or_not.find(userid) != check_log_or_not.end()) {
    std::cout << "Account has log in and can't be deleted right now" << std::endl;
    return;
  }
  User user;
  account.delete_account(userid);
  return;
}

void AccountOperator::modify_user() {
  char userid[31];
  char current_password[31];
  char new_password[31];
  std::cin >> userid >> current_password >> new_password;
  User user;
  account.update_account(user);
  return;
}

void AccountOperator::login() {
  char userid[31];
  char password[31];
  std::cin >> userid >> password;
  if (check_log_or_not.find(userid) != check_log_or_not.end()) {
    std::cout << "Account has log in" << std::endl;
    return;
  }
  User user;
  if (account.query_account(userid)) {
    if (user.login(password)) {
      check_log_or_not.insert(userid);
      userlist.push(user);
      return;
    }
  } else {
    std::cout << "login failed" << std::endl;
  }
  return;
}

void AccountOperator::logout() {
  if (userlist.size() == 1) {
    std::cout << "No account has log in" << std::endl;
    return;
  } else {
    check_log_or_not.erase(userlist.top().check_name());
    userlist.pop();
  }
  return;
}