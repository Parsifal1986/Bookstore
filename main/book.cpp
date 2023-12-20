#include "book.hpp"
#include "tokenscanner.hpp"
#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

NameIndex nameindex;
AuthorIndex authorindex;
KeywordIndex keywordindex;

Book::Book(char *isbn, char *name, char *author, char *keyword, double price, int quantity) {
  strcpy(this->isbn, isbn);
  strcpy(this->name, name);
  strcpy(this->author, author);
  strcpy(this->keyword, keyword);
  this->price = price;
  this->quantity = quantity;
};

void Book::create_book(char *isbn) {
  strcpy(this->isbn, isbn);
  return;
};

void Book::modify_book(Book &new_book) {
  if (!strcmp(isbn, new_book.isbn)) {
    throw 1;
  }
  if (strlen(new_book.name)) {
    nameindex.delete_index(this->name, this->isbn);
    strcpy(this->name, new_book.name);
    nameindex.add_index(this->name, this->isbn);
  }
  if (strlen(new_book.author)) {
    authorindex.delete_index(this->author, this->isbn);
    strcpy(this->author, new_book.author);
    authorindex.add_index(this->author, this->isbn);
  }
  if (strlen(new_book.author)) {
    keywordindex.delete_index(this->keyword, this->isbn);
    strcpy(this->keyword, new_book.keyword);
    keywordindex.add_index(this->keyword, this->isbn);
  }
  if (new_book.price != -1) {
    this->price = new_book.price;
  }
  if (strlen(new_book.isbn)) {
    strcpy(this->isbn, new_book.isbn);
  }
  return;
};

void Book::show_book() {
  std::cout << isbn << "\t" << name << "\t" << author << "\t" << keyword << "\t" << price << "\t" << quantity << std::endl;
  return;
};

bool Book::check_enough(int quantity) {
  if (this->quantity < quantity) {
    return false;
  }
  return true;
};

void Book::change_quantity(int quantity) {
  this->quantity += quantity;
  return;
};

int BookDatabase::add_book(Book &book) {
  int book_number;
  memoryriver.get_info(book_number, 1);
  int index = memoryriver.write(book);
  memoryriver.write_info(book_number + 1, 1);
  return index;
};

void BookDatabase::delete_book(int index) {
  int book_number;
  memoryriver.get_info(book_number, 1);
  memoryriver.Delete(index);
  memoryriver.write_info(book_number - 1, 1);
  return;
};

void BookDatabase::update_book(int index, Book &book) {
  Book old_book;
  memoryriver.read(old_book, index);
  if (strcmp(old_book.show_book_isbn(), book.show_book_isbn()) && strlen(book.show_book_isbn())) {
    Index.erase(old_book.show_book_isbn());
    Index.insert(std::make_pair(book.show_book_isbn(), index));
  }
    try {
      old_book.modify_book(book);
    } catch (int) {
      throw 1;
    }
  memoryriver.update(old_book, index);
  return;
};

Book BookDatabase::query_book(int index) {
  Book book;
  memoryriver.read(book, index);
  return book;
};

void BookDatabase::list_book(std::vector<int> &index_list) {
  Book book;
  for (auto it = index_list.begin(); it != index_list.end(); it++) {
    memoryriver.read(book, *it);
    book.show_book();
  }
  return;
};

void BookDatabase::list_book(std::vector<std::string> &index_list) {
  Book book;
  for (auto it = index_list.begin(); it != index_list.end(); it++) {
    memoryriver.read(book, Index[*it]);
    book.show_book();
  }
  return;
};

std::vector<int> BookDatabase::search_book(char *key) {
  std::vector<int> result;
  result.clear();
  if (Index.find(key) != Index.end()) {
    result.push_back(Index[key]);
  }
  return result;
}

void BookDatabase::add_index(char *key, int index) {
  if (!this->search_book(key).empty()) {
    return;
  }
  Index[key] = index;
  return;
}

void BookDatabase::delete_index(char *key, int index) {
  if (this->search_book(key).empty()) {
    return;
  }
  Index.erase(key);
  return;
}

void NameIndex::add_index(char *key, char* isbn){
  Node tmp;
  strcpy(tmp.name, key);
  strcpy(tmp.isbn, isbn);
  Index.Insert(tmp);
  return;
}

void NameIndex::delete_index(char *key, char *isbn) {
  Node tmp;
  strcpy(tmp.name, key);
  strcpy(tmp.isbn, isbn);
  Index.Delete(tmp);
  return;
}

std::vector<std::string> NameIndex::search_book(char *key) {
  Node tmp;
  strcpy(tmp.name, key);
  return Index.Find(tmp);
}

void AuthorIndex::add_index(char *key, char *isbn) {
  Node tmp;
  strcpy(tmp.author, key);
  strcpy(tmp.isbn, isbn);
  Index.Insert(tmp);
  return;
}

void AuthorIndex::delete_index(char *key, char *isbn) {
  Node tmp;
  strcpy(tmp.author, key);
  strcpy(tmp.isbn, isbn);
  Index.Delete(tmp);
  return;
}

std::vector<std::string> AuthorIndex::search_book(char *key) {
  Node tmp;
  strcpy(tmp.author, key);
  return Index.Find(tmp);
}

void KeywordIndex::add_index(char *key, char *isbn) {
  Node tmp;
  strcpy(tmp.keyword, key);
  strcpy(tmp.isbn, isbn);
  Index.Insert(tmp);
  return;
}

void KeywordIndex::delete_index(char *key, char *isbn) {
  Node tmp;
  strcpy(tmp.keyword, key);
  strcpy(tmp.isbn, isbn);
  Index.Delete(tmp);
  return;
}

std::vector<std::string> KeywordIndex::search_book(char *key) {
  Node tmp;
  strcpy(tmp.keyword, key);
  return Index.Find(tmp);
}

void BookOperator::show_book() {
  tokenscanner.set_whether_cut_up_equal_sign(true);
  std::string token = tokenscanner.next_token();
  tokenscanner.set_whether_cut_up_equal_sign(false);
  if (token == "-ISBN=") {
    std::vector <int> index_list;
    char ISBN[21];
    tokenscanner.set_char(ISBN);
    index_list = bookdatabase.search_book(ISBN);
    if (index_list.empty()) {
      throw 1;
    }
    if (tokenscanner.has_more_tokens()) {
      throw 1;
    }
    bookdatabase.list_book(index_list);
  } else {
    std::vector<std::string> index_list;
    tokenscanner.set_get_quotation_content(true);
    if (token == "-name=") {
      char name[61];
      tokenscanner.set_char(name);
      index_list = nameindex.search_book(name);
      if (index_list.empty()) {
        throw 1;
      }
    } else if (token == "-author=") {
      char author[61];
      tokenscanner.set_char(author);
      index_list = authorindex.search_book(author);
      if (index_list.empty()) {
        throw 1;
      }
    } else if (token == "-keyword=") {
      char keyword[61];
      tokenscanner.set_char(keyword);
      index_list = keywordindex.search_book(keyword);
      if (index_list.empty()) {
        throw 1;
      }
    } else {
      throw 1;
    }
    tokenscanner.set_get_quotation_content(false);
    if (tokenscanner.has_more_tokens()) {
      throw 1;
    }
    bookdatabase.list_book(index_list);
  }
  return;
}

void BookOperator::select_book() {
  char ISBN[21];
  tokenscanner.set_char(ISBN);
  if (bookdatabase.search_book(ISBN).empty()) {
    Book new_book;
    new_book.create_book(ISBN);
    bookdatabase.add_index(ISBN, bookdatabase.add_book(new_book));
    selected_book = bookdatabase.search_book(ISBN)[0];
  } else {
    selected_book = bookdatabase.search_book(ISBN)[0];
  }
  return;
}

void BookOperator::update_book() {
  if (selected_book == -1) {
    throw 1;
  }
  char isbn[21] = {'\0'};
  char name[61] = {'\0'};
  char author[61] = {'\0'};
  char keyword[61] = {'\0'};
  int quantity = -1;
  float price = -1;
  while (tokenscanner.has_more_tokens()) {
    tokenscanner.set_whether_cut_up_equal_sign(true);
    std::string token = tokenscanner.next_token();
    tokenscanner.set_whether_cut_up_equal_sign(false);
    if (token == "-ISBN=") {
      strcpy(isbn, tokenscanner.next_token().c_str());
    } else if (token == "-price=") {
      price = atof(tokenscanner.next_token().c_str());
    } else {
      tokenscanner.set_get_quotation_content(true);
      if (token == "-name=") {
        strcpy(name, tokenscanner.next_token().c_str());
      } else if (token == "-author=") {
        strcpy(author, tokenscanner.next_token().c_str());
      } else if (token == "-keyword=") {
        strcpy(keyword, tokenscanner.next_token().c_str());
      }
      tokenscanner.set_get_quotation_content(false);
    }
  }
  Book new_book(isbn, name, author, keyword, price, quantity);
  try {
    bookdatabase.update_book(selected_book, new_book);
  } catch (int){
    throw 1;
  }
  return;
}

void BookOperator::buy_book() {
  char isbn[21];
  int quantity;
  std::cin >> isbn >> quantity;
  if (bookdatabase.search_book(isbn).empty()) {
    throw 1;
  } else {
    Book book;
    book = bookdatabase.query_book(bookdatabase.search_book(isbn)[0]);
    if (book.check_enough(quantity)) {
      throw 1;
    } else {
      book.change_quantity(-quantity);
      bookdatabase.update_book(bookdatabase.search_book(isbn)[0], book);
      return;
    }
  }
}

void BookOperator::import_book() {
    char isbn[21];
    int quantity;
    double cost;
    std::cin >> isbn >> quantity >> cost;
    if (bookdatabase.search_book(isbn).empty()) {
      throw 1;
    } else {
        Book book;
        book = bookdatabase.query_book(bookdatabase.search_book(isbn)[0]);
        book.change_quantity(quantity);
        bookdatabase.update_book(bookdatabase.search_book(isbn)[0], book);
        return;
    }
}