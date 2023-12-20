#include "account.hpp"
#include "book.hpp"
#include "memoryriver.hpp"
#include "tokenscanner.hpp"
#include <cctype>
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

bool User::check_password(char *password) {
  if (!(std::strcmp(this->password, password))) {
    return true;
  }
  return false;
}

bool User::check_right(int needed_right) {
  if (this->right < needed_right) {
    return false;
  }
  return true;
}

AccountManager::AccountManager() {
  memoryriver.initialise("AccountManager");
  int user_number;
  memoryriver.get_info(user_number, 1);
  for (int i = 0; i < user_number; ++i) {
    User tmp;
    memoryriver.read(tmp, i * sizeof(User) + sizeof(int));
    account_base.insert(std::pair<std::string, int>(tmp.check_id(), i * sizeof(User) + sizeof(int)));
  }
}

void AccountManager::add_account(User &user){
  int user_number;
  memoryriver.get_info(user_number,1);
  account_base.insert(std::pair<std::string, int>(user.check_id(), memoryriver.write(user)));
  memoryriver.write_info(user_number + 1, 1);
}

void AccountManager::delete_account(std::string userid){
  int user_number;
  memoryriver.get_info(user_number, 1);
  memoryriver.Delete(account_base[userid]);
  memoryriver.write_info(user_number - 1, 1);
  account_base.erase(userid);
}

int AccountManager::query_account(std::string userid){
  if (account_base.find(userid) == account_base.end()) {
    return -1;
  } else {
    return account_base[userid];
  }
}

void AccountManager::update_account(User &user){
  memoryriver.update(user, account_base[user.check_id()]);
}

User AccountManager::draw_account(int index){
  User tmp;
  memoryriver.read(tmp, index);
  return tmp;
}

AccountOperator::AccountOperator() {
  if (account.query_account("root") == -1) {
    char userid[] = "root";
    char password[] = "sjtu";
    char username[] = "root";
    User user(userid, password, '7', username);
    account.add_account(user);
  }
}

void AccountOperator::register_user() {
  char userid[31];
  char password[31];
  char username[31];
  if (!tokenscanner.has_more_tokens()) {
    throw 1;
  }
  tokenscanner.set_char(userid);
  if (account.query_account(userid) != -1) {
    throw 1;
  }
  if (!tokenscanner.has_more_tokens()) {
    throw 1;
  }
  tokenscanner.set_char(password);
  if (!tokenscanner.has_more_tokens()) {
    throw 1;
  }
  tokenscanner.set_char(username);
  if (tokenscanner.has_more_tokens()) {
    throw 1;
  }
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
  if (!tokenscanner.has_more_tokens()) {
    throw 1;
  }
  tokenscanner.set_char(userid);
  if (userlist.empty()) {
    throw 1;
  }
  if (!check_right('3')) {
    throw 1;
  }
  if (account.query_account(userid) != -1) {
    throw 1;
  }
  if (!tokenscanner.has_more_tokens()) {
    throw 1;
  }
  tokenscanner.set_char(password);
  right = tokenscanner.next_token().c_str()[0];
  if (!isdigit(right)) {
    throw 1;
  }
  if (!tokenscanner.has_more_tokens()) {
    throw 1;
  }
  tokenscanner.set_char(username);
  if (!check_right(right + 1) || (right != '1' && right != '3')) {
    throw 1;
  }
  if (tokenscanner.has_more_tokens()) {
    throw 1;
  }
  User user(userid, password, right, username);
  account.add_account(user);
  return;
}

void AccountOperator::delete_user() {
  char userid[31];
  if (!tokenscanner.has_more_tokens()) {
    throw 1;
  }
  tokenscanner.set_char(userid);
  if (!check_right('7')) {
    throw 1;
  }
  if (account.query_account(userid) == -1) {
    throw 1;
  }
  if (check_log_or_not.find(userid) != check_log_or_not.end()) {
    throw 1;
  }
  account.delete_account(userid);
  return;
}

void AccountOperator::modify_user() {
  char userid[31];
  char current_password[31];
  char new_password[31];
  if (userlist.empty()) {
    throw 1;
  }
  if (!tokenscanner.has_more_tokens()) {
    throw 1;
  }
  tokenscanner.set_char(userid);
  if (account.query_account(userid) == -1) {
    throw 1;
  }
  User modified_user = account.draw_account(account.query_account(userid));
  if (!check_right('7')) {
    if (!tokenscanner.has_more_tokens()) {
      throw 1;
    }
    tokenscanner.set_char(current_password);
  }
  if (!modified_user.check_password(current_password) && strlen(current_password)) {
    throw 1;
  }
  if (!tokenscanner.has_more_tokens()) {
    throw 1;
  }
  tokenscanner.set_char(new_password);
  if (tokenscanner.has_more_tokens()) {
    throw 1;
  }
  modified_user.change_password(new_password);
  account.update_account(modified_user);
  return;
}

void AccountOperator::login() {
  char userid[31];
  char password[31];
  tokenscanner.set_char(userid);
  if (account.query_account(userid) == -1) {
    throw 1;
  }
  User new_user = account.draw_account(account.query_account(userid));
  if (!check_right(new_user.read_right())) {
    tokenscanner.set_char(password);
    if (!new_user.check_password(password)) {
      throw 1;
    }
  } else {
    if (tokenscanner.has_more_tokens()) {
      tokenscanner.set_char(password);
      if (!new_user.check_password(password)) {
        throw 1;
      }
    }
  }
  if (check_log_or_not.find(userid) != check_log_or_not.end()) {
    throw 1;
  }
  userlist.push(new_user);
  check_log_or_not.insert(userid);
  return;
}

void AccountOperator::logout() {
  if (userlist.empty()) {
    throw 1;
  } else {
    check_log_or_not.erase(userlist.top().check_id());
    userlist.pop();
  }
  return;
}