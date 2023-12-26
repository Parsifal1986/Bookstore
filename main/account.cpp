#include "account.hpp"
#include "book.hpp"
#include "memoryriver.hpp"
#include "tokenscanner.hpp"
#include <cctype>
#include <cstring>
#include <variant>

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
  int user_number, left_to_be_deleted;
  memoryriver.get_info(user_number, 1);
  memoryriver.get_info(left_to_be_deleted, 2);
  for (int i = 0; i < user_number + left_to_be_deleted; ++i) {
    User tmp;
    memoryriver.read(tmp, i * sizeof(User) + 2 * sizeof(int));
    if (!strlen(tmp.check_id())) {
      continue;
    }
    account_base.insert(std::pair<std::string, int>(tmp.check_id(), i * sizeof(User) + 2 * sizeof(int)));
  }
}

void AccountManager::add_account(User &user){
  int user_number, left_to_be_deleted;
  memoryriver.get_info(user_number,1);
  memoryriver.get_info(left_to_be_deleted, 2);
  account_base.insert(std::pair<std::string, int>(user.check_id(), memoryriver.write(user)));
  memoryriver.write_info(user_number + 1, 1);
  if (left_to_be_deleted) {
    memoryriver.write_info(left_to_be_deleted - 1, 2);
  }
}

void AccountManager::delete_account(std::string userid){
  int user_number, left_to_be_deleted;
  memoryriver.get_info(user_number, 1);
  memoryriver.get_info(left_to_be_deleted, 2);
  memoryriver.Delete(account_base[userid]);
  memoryriver.write_info(user_number - 1, 1);
  memoryriver.write_info(left_to_be_deleted + 1, 2);
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
    Log.add_admin("root");
  }
}

void AccountOperator::register_user() {
  char userid[31];
  char password[31];
  char username[31];
  if (!tokenscanner.has_more_tokens()) {
    throw 1;
  }
  tokenscanner.set_word_limit(30);
  tokenscanner.set_char(userid);
  if (!tokenscanner.is_legal(userid)) {
    throw 1;
  }
  if (account.query_account(userid) != -1) {
    throw 1;
  }
  if (!tokenscanner.has_more_tokens()) {
    throw 1;
  }
  tokenscanner.set_char(password);
  if (!tokenscanner.is_legal(password)) {
    throw 1;
  }
  if (!tokenscanner.has_more_tokens()) {
    throw 1;
  }
  tokenscanner.set_char(username);
  if (tokenscanner.has_more_tokens()) {
    throw 1;
  }
  if (!tokenscanner.is_printable(username)) {
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
  tokenscanner.set_word_limit(30);
  tokenscanner.set_char(userid);
  if (!tokenscanner.is_legal(userid)) {
    throw 1;
  }
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
  if (!tokenscanner.is_legal(password)) {
    throw 1;
  }
  std::string tmp(tokenscanner.next_token());
  if (tmp.size() > 1) {
    throw 1;
  } else {
    right = tmp[0];
  }
  if (!std::isdigit(right)) {
    throw 1;
  }
  if (!tokenscanner.has_more_tokens()) {
    throw 1;
  }
  tokenscanner.set_char(username);
  if (tokenscanner.has_more_tokens()) {
    throw 1;
  }
  if (!check_right(right + 1) || (right != '1' && right != '3')) {
    throw 1;
  }
  if (tokenscanner.has_more_tokens()) {
    throw 1;
  }
  User user(userid, password, right, username);
  account.add_account(user);
  if (right == '3') {
    Log.add_admin(userid);
  }
  return;
}

void AccountOperator::delete_user() {
  char userid[31];
  if (!tokenscanner.has_more_tokens()) {
    throw 1;
  }
  tokenscanner.set_word_limit(30);
  tokenscanner.set_char(userid);
  if (tokenscanner.has_more_tokens()) {
    throw 1;
  }
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
  tokenscanner.set_word_limit(30);
  tokenscanner.set_char(userid);
  if (account.query_account(userid) == -1) {
    throw 1;
  }
  User modified_user = account.draw_account(account.query_account(userid));
  if (!tokenscanner.has_more_tokens()) {
    throw 1;
  }
  tokenscanner.set_char(current_password);
  if (!check_right('7')) {
    if (!modified_user.check_password(current_password)) {
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
  } else {
    if (tokenscanner.has_more_tokens()) {
      tokenscanner.set_char(new_password);
      if (tokenscanner.has_more_tokens()) {
        throw 1;
      }
      if (!modified_user.check_password(current_password)) {
        throw 1;
      }
      modified_user.change_password(new_password);
      account.update_account(modified_user);
    } else {
      strcpy(new_password, current_password);
      modified_user.change_password(new_password);
      account.update_account(modified_user);
    }
  }
  return;
}

void AccountOperator::login() {
  char userid[31];
  char password[31];
  if (!tokenscanner.has_more_tokens()) {
    throw 1;
  }
  tokenscanner.set_word_limit(30);
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
  if (tokenscanner.has_more_tokens()) {
    throw 1;
  }
  // if (check_log_or_not.find(userid) != check_log_or_not.end()) {
  //   throw 1;
  // }
  userlist.push(new_user);
  select_book.push(-1);
  check_log_or_not.insert(userid);
  return;
}

void AccountOperator::logout() {
  if (tokenscanner.has_more_tokens()) {
    throw 1;
  }
  if (userlist.empty()) {
    throw 1;
  } else {
    check_log_or_not.erase(userlist.top().check_id());
    userlist.pop();
    select_book.pop();
  }
  return;
}