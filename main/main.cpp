#include "account.hpp"
#include "book.hpp"
#include "log.hpp"
#include "tokenscanner.hpp"
#include <iostream>
#include <cstdio>

Tokenscanner tokenscanner;
LogManager Log;

int main() {
  AccountOperator account_operator;
  BookOperator book_operator;
  std::string command;

  command.clear();
  while (command != "quit" && command != "exit") {
    try {
      std::string line;
      std::getline(std::cin, line);
      tokenscanner.set_line(line);
      command = tokenscanner.next_token();
      if (command == "su") {
        account_operator.login();
      }
      if (command == "logout") {
        account_operator.logout();
      }
      if (command == "register") {
        account_operator.register_user();
      }
      if (command == "passwd") {
        account_operator.modify_user();
      }
      if (command == "useradd") {
        account_operator.add_user();
      }
      if (command == "delete") {
        account_operator.delete_user();
      }
      if (command == "show") {
        if (!account_operator.check_right('1')) {
          throw 1;
        }
        book_operator.show_book();
      }
      if (command == "select") {
        if (!account_operator.check_right('3')) {
          throw 1;
          continue;
        }
        book_operator.select_book();
      }
      if (command == "modify") {
        if (!account_operator.check_right('3')) {
          throw 1;
          continue;
        }
        book_operator.update_book();
      }
      if (command == "import") {
        if (!account_operator.check_right('3')) {
          throw 1;
          continue;
        }
        book_operator.import_book();
      }
      if (command == "buy") {
        if (!account_operator.check_right('1')) {
          throw 1;
          continue;
        }
        book_operator.buy_book();
      }
      if (command == "show finance") {
        if (!account_operator.check_right('7')) {
          throw 1;
          continue;
        }
        int count;
        count = tokenscanner.next_number();
        if (tokenscanner.has_more_tokens()) {
          throw 1;
        }
        Log.show_finance(count);
      }
      if (command == "quit" || command == "exit") {
        break;
      }
    } catch (int){
      std::cout << "Invalid" << std::endl;
    }
  }
  return 0;
}