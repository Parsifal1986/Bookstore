#include "account.hpp"
#include "book.hpp"
#include "log.hpp"
#include "tokenscanner.hpp"
#include <iostream>
#include <cstdio>

Tokenscanner tokenscanner;
LogManager Log;
AccountOperator account_operator;

int main() {
  BookOperator book_operator;
  std::string command;
  LogMessage piece_of_log;
  bool flag = false;

  command.clear();
  while (!std::cin.eof()) {
    try {
      std::string line;
      std::getline(std::cin, line);
      tokenscanner.set_line(line);
      strcpy(piece_of_log.log, line.c_str());
      if (!tokenscanner.has_more_tokens()) {
        continue;
      }
      command = tokenscanner.next_token();
      if (command == "su") {
        account_operator.login();
        book_operator.change_select();
      } else if (command == "logout") {
        account_operator.logout();
        book_operator.change_select();
      } else if (command == "register") {
        account_operator.register_user();
      } else if (command == "passwd") {
        account_operator.modify_user();
      } else if (command == "useradd") {
        account_operator.add_user();
        flag = true;
      } else if (command == "delete") {
        account_operator.delete_user();
        flag = true;
      } else if (command == "show") {
        if (tokenscanner.next_token() == "finance") {
          if (!account_operator.check_right('7')) {
            throw 1;
            continue;
          }
          int count = -1;
          if (tokenscanner.has_more_tokens()) {
            count = tokenscanner.next_number();
          }
          if (tokenscanner.has_more_tokens()) {
            throw 1;
          }
          Log.show_finance(count);
        } else {
          tokenscanner.scroll_back();
          if (!account_operator.check_right('1')) {
            throw 1;
          }
          book_operator.show_book();
        }
      } else if (command == "select") {
        if (!account_operator.check_right('3')) {
          throw 1;
        }
        book_operator.select_book();
        flag = 1;
      } else if (command == "modify") {
        if (!account_operator.check_right('3')) {
          throw 1;
        }
        book_operator.update_book();
        flag = 1;
      } else if (command == "import") {
        if (!account_operator.check_right('3')) {
          throw 1;
        }
        book_operator.import_book();
        flag = 1;
      } else if (command == "buy") {
        if (!account_operator.check_right('1')) {
          throw 1;
        }
        book_operator.buy_book();
      } else if (command == "quit" || command == "exit") {
        if (tokenscanner.has_more_tokens()) {
          throw 1;
        }
        break;
      } else {
        throw 1;
      }
      tokenscanner.set_word_limit(-1);
      piece_of_log.type = true;
    } catch (int){
      std::cout << "Invalid" << std::endl;
      tokenscanner.set_devide_by_slash(false);
      tokenscanner.set_whether_cut_up_equal_sign(false);
      tokenscanner.set_word_limit(-1);
      piece_of_log.type = false;
    }
    Log.add_log(piece_of_log);
    if (flag) {
      Log.add_log(piece_of_log, account_operator.check_id());
      flag = false;
    }
  }
  return 0;
}