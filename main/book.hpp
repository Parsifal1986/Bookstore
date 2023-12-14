#ifndef BOOK_HPP
#define BOOK_HPP

#include "memoryriver.hpp"
#include "blocklinkedlist.hpp"
#include <fstream>
#include <iostream>
#include <cstdio>
#include <map>
#include <stdexcept>

class Book {
 public:
  Book() = default;

  Book(char *isbn, char *name, char *author, char *keyword, double price, int quantity);

  ~Book() = default;

  void create_book(char *isbn, char *name, char *author, char *keyword, double price, int quantity);

  void modify_book(Book &book);

  void show_book();

  char* show_book_name();

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
  };

  ~BookDatabase() = default;

  int add_book(Book &book);

  void delete_book(int index);

  void update_book(int index, Book &book);

  Book query_book(int index);

  void list_book(std::vector<int> &index_list);

 private:
  MemoryRiver<Book, 1> memoryriver;
};

class IndexSystem {
 public:
  IndexSystem() = default;

  virtual ~IndexSystem() = default;

  virtual std::vector<int> search_book(char *key) = 0;

  virtual void add_index(char *key, int index);

  virtual void delete_index(char *key, int index);
};

class ISBNIndex : public IndexSystem {
 public:
  ISBNIndex() {
    Index.clear();
  };

  ~ISBNIndex() = default;

  std::vector<int> search_book(char *key) override;

  void add_index(char *key, int index) override;

  void delete_index(char *key, int index) override;

 private:
  std::map<std::string, int> Index;
};

class NameIndex : public IndexSystem {
 public:
  NameIndex() {
    Index.set_filename("NameIndex");
  };

  ~NameIndex(){
    
  };

  std::vector<int> search_book(char *key) override;

  void add_index(char *key, int index) override;

  void delete_index(char *key, int index) override;

 private:
  struct Node {
    char name[61];
    int index;

    bool operator<(const Node &rhs) const {
      return strcmp(name, rhs.name) == 0 ? index < rhs.index
                                         : strcmp(name, rhs.name) < 0;
    }

    bool operator==(const Node &rhs) const {
      return strcmp(name, rhs.name) == 0 && index == rhs.index;
    }

    bool Equal(const Node &rhs) const {
      return strcmp(name, rhs.name) == 0;
    }
  };
  Linklist<Node> Index;
};

class AuthorIndex : public IndexSystem {
 public:
  AuthorIndex() {
    Index.set_filename("AuthorIndex");
  };

  ~AuthorIndex() = default;

  std::vector<int> search_book(char *key) override;

  void add_index(char *key, int index) override;

  void delete_index(char *key, int index) override;
 private:
   struct Node {
    char author[61];
    int index;

    bool operator<(const Node &rhs) const {
      return strcmp(author, rhs.author) == 0 ? index < rhs.index
                                         : strcmp(author, rhs.author) < 0;
    }

    bool operator==(const Node &rhs) const {
      return strcmp(author, rhs.author) == 0 && index == rhs.index;
    }

    bool Equal(const Node &rhs) const { return strcmp(author, rhs.author) == 0; }
   };
   Linklist<Node> Index;
};

class KeywordIndex : public IndexSystem {
 public:
  KeywordIndex() {
    Index.set_filename("KeywordIndex");
  };

  ~KeywordIndex() = default;

  std::vector<int> search_book(char *key) override;

  void add_index(char *key, int index) override;

  void delete_index(char *key, int index) override;

 private:
   struct Node {
    char keyword[61];
    int index;

    bool operator<(const Node &rhs) const {
      return strcmp(keyword, rhs.keyword) == 0 ? index < rhs.index
                                         : strcmp(keyword, rhs.keyword) < 0;
    }

    bool operator==(const Node &rhs) const {
      return strcmp(keyword, rhs.keyword) == 0 && index == rhs.index;
    }

    bool Equal(const Node &rhs) const { return strcmp(keyword, rhs.keyword) == 0; }
   };
   Linklist<Node> Index;
};

class BookOperator {
 public:
  BookOperator() = default;

  ~BookOperator() = default;

  void search_book();

  void select_book();

  void update_book();

  void import_book();

  void buy_book();

 private:
  BookDatabase bookdatabase;
  ISBNIndex isbnindex;
  NameIndex nameindex;
  AuthorIndex authorindex;
  KeywordIndex keywordindex;
  int selected_book = -1;
};

#endif // !BOOK_HPP