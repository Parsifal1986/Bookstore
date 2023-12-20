#ifndef BOOK_HPP
#define BOOK_HPP

#include "blocklinkedlist.hpp"
#include "memoryriver.hpp"
#include "tokenscanner.hpp"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>

extern Tokenscanner tokenscanner;

class Book {
public:
  Book() = default;

  Book(char *isbn, char *name, char *author, char *keyword, double price,
       int quantity);

  ~Book() = default;

  void create_book(char *isbn);

  void modify_book(Book &book);

  void show_book();

  char *show_book_name() { return name; };

  char *show_book_isbn() { return isbn; };

  bool check_enough(int quantity);

  void change_quantity(int quantity);

private:
  char isbn[21] = {'\0'};
  char name[61] = {'\0'};
  char author[61] = {'\0'};
  char keyword[61] = {'\0'};
  int quantity = 0;
  float price = 0;
};

class BookDatabase {

public:
  BookDatabase() {
    memoryriver.initialise("book_data");
    Index.clear();
    int book_number;
    memoryriver.get_info(book_number, 1);
    for (int i = 0; i < book_number; ++i) {
      Book tmp;
      memoryriver.read(tmp, i * sizeof(Book) + sizeof(int));
      Index.insert(
          std::make_pair(tmp.show_book_name(), i * sizeof(Book) + sizeof(int)));
    }
  };

  ~BookDatabase() = default;

  int add_book(Book &book);

  void delete_book(int index);

  void update_book(int index, Book &book);

  Book query_book(int index);

  void list_book(std::vector<int> &index_list);

  void list_book(std::vector<std::string> &index_list);

  std::vector<int> search_book(char *key);

  void add_index(char *key, int index);

  void delete_index(char *key, int index);

private:
  MemoryRiver<Book, 1> memoryriver;
  std::map<std::string, int> Index;
};

class NameIndex {
public:
  NameIndex() { Index.initialise("name_index"); };

  ~NameIndex() = default;

  std::vector<std::string> search_book(char *key);

  void add_index(char *key, char *isbn);

  void delete_index(char *key, char *isbn);

private:
  struct Node {
    char name[61], isbn[21];

    bool operator<(const Node &rhs) const {
      return strcmp(name, rhs.name) == 0 ? strcmp(isbn, rhs.isbn) < 0
                                         : strcmp(name, rhs.name) < 0;
    }

    bool operator==(const Node &rhs) const {
      return strcmp(name, rhs.name) == 0 && strcmp(isbn, rhs.isbn) == 0;
    }

    bool Equal(const Node &rhs) const { return strcmp(name, rhs.name) == 0; }
  };
  Linklist<Node> Index;
};

class AuthorIndex {
public:
  AuthorIndex() { Index.initialise("author_index"); };

  ~AuthorIndex() = default;

  std::vector<std::string> search_book(char *key);

  void add_index(char *key, char *isbn);

  void delete_index(char *key, char *isbn);

private:
  struct Node {
    char author[61], isbn[21];

    bool operator<(const Node &rhs) const {
      return strcmp(author, rhs.author) == 0 ? strcmp(isbn, rhs.isbn) < 0
                                             : strcmp(author, rhs.author) < 0;
    }

    bool operator==(const Node &rhs) const {
      return strcmp(author, rhs.author) == 0 && strcmp(isbn, rhs.isbn) == 0;
    }

    bool Equal(const Node &rhs) const {
      return strcmp(author, rhs.author) == 0;
    }
  };
  Linklist<Node> Index;
};

class KeywordIndex {
public:
  KeywordIndex() { Index.initialise("keyword_index"); };

  ~KeywordIndex() = default;

  std::vector<std::string> search_book(char *key);

  void add_index(char *key, char *isbn);

  void delete_index(char *key, char *isbn);

private:
  struct Node {
    char keyword[61], isbn[21];

    bool operator<(const Node &rhs) const {
      return strcmp(keyword, rhs.keyword) == 0
                 ? strcmp(isbn, rhs.isbn) < 0
                 : strcmp(keyword, rhs.keyword) < 0;
    }

    bool operator==(const Node &rhs) const {
      return strcmp(keyword, rhs.keyword) == 0 && strcmp(isbn, rhs.isbn) == 0;
    }

    bool Equal(const Node &rhs) const {
      return strcmp(keyword, rhs.keyword) == 0;
    }
  };
  Linklist<Node> Index;
};

class BookOperator {
  friend class Book;

public:
  BookOperator() = default;

  ~BookOperator() = default;

  void show_book();

  void select_book();

  void update_book();

  void import_book();

  void buy_book();

private:
  BookDatabase bookdatabase;
  int selected_book = -1;
};

#endif // !BOOK_HPP