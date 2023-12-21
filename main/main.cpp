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
  while (command != "quit" && command != "exit" && !std::cin.eof()) {
    try {
      std::string line;
      std::getline(std::cin, line);
      tokenscanner.set_line(line);
      if (!tokenscanner.has_more_tokens()) {
        continue;
      }
      command = tokenscanner.next_token();
      if (command == "su") {
        account_operator.login();
      } else if (command == "logout") {
        account_operator.logout();
      } else if (command == "register") {
        account_operator.register_user();
      } else if (command == "passwd") {
        account_operator.modify_user();
      } else if (command == "useradd") {
        account_operator.add_user();
      } else if (command == "delete") {
        account_operator.delete_user();
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
          continue;
        }
        book_operator.select_book();
      } else if (command == "modify") {
        if (!account_operator.check_right('3')) {
          throw 1;
          continue;
        }
        book_operator.update_book();
      } else if (command == "import") {
        if (!account_operator.check_right('3')) {
          throw 1;
          continue;
        }
        book_operator.import_book();
      } else if (command == "buy") {
        if (!account_operator.check_right('1')) {
          throw 1;
          continue;
        }
        book_operator.buy_book();
      } else if (command == "quit" || command == "exit") {
        break;
      } else {
        throw 1;
      }
    } catch (int){
      std::cout << "Invalid" << std::endl;
      tokenscanner.set_devide_by_slash(false);
      tokenscanner.set_get_quotation_content(false);
      tokenscanner.set_whether_cut_up_equal_sign(false);
    }
  }
  return 0;
}