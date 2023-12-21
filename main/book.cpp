#include "book.hpp"
#include "tokenscanner.hpp"
#include <algorithm>
#include <cstring>
#include <iomanip>
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

Book::Book(double price, int quantity) {
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
    nameindex.add_index(this->name, strlen(new_book.isbn) ? new_book.isbn : this->isbn);
  }
  if (strlen(new_book.author)) {
    authorindex.delete_index(this->author, this->isbn);
    strcpy(this->author, new_book.author);
    authorindex.add_index(this->author, strlen(new_book.isbn) ? new_book.isbn : this->isbn);
  }
  if (strlen(new_book.keyword)) {
    keywordindex.delete_index(this->keyword, this->isbn);
    strcpy(this->keyword, new_book.keyword);
    keywordindex.add_index(this->keyword, strlen(new_book.isbn) ? new_book.isbn : this->isbn);
  }
  if (new_book.price != -1) {
    this->price = new_book.price;
  }
  if (new_book.quantity != -1) {
    this->quantity = new_book.quantity;
  }
  if (strlen(new_book.isbn)) {
    strcpy(this->isbn, new_book.isbn);
  }
  return;
};

void Book::show_book() {
  std::cout << isbn << "\t" << name << "\t" << author << "\t" << keyword << "\t" << setiosflags(std::ios::fixed) << std::setprecision(2) << price << "\t" << quantity << std::endl;
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

void BookDatabase::list_book() {
  Book book;
  int book_number;
  memoryriver.get_info(book_number, 1);
  for (auto it  = Index.begin(); it != Index.end(); it++) {
    memoryriver.read(book, it->second);
    book.show_book();
  }
  return;
}

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
  Tokenscanner division;
  division.set_line(key);
  division.set_devide_by_slash(true);
  while (division.has_more_tokens()) {
    Node tmp;
    strcpy(tmp.keyword, division.next_token().c_str());
    strcpy(tmp.isbn, isbn);
    Index.Insert(tmp);
  }
  division.set_devide_by_slash(false);
  return;
}

void KeywordIndex::delete_index(char *key, char *isbn) {
  Tokenscanner division;
  division.set_line(key);
  division.set_devide_by_slash(true);
  while (division.has_more_tokens()) {
    Node tmp;
    strcpy(tmp.keyword, division.next_token().c_str());
    strcpy(tmp.isbn, isbn);
    Index.Delete(tmp);
  }
  division.set_devide_by_slash(false);
  return;
}

std::vector<std::string> KeywordIndex::search_book(char *key) {
  Tokenscanner division;
  division.set_line(key);
  std::vector<std::string> result;
  result.clear();
  Node tmp;
  strcpy(tmp.keyword, division.next_token().c_str());
  std::vector<std::string> tmp_result = Index.Find(tmp);
  result.insert(result.end(), tmp_result.begin(), tmp_result.end());
  return result;
}

void BookOperator::show_book() {
  if (!tokenscanner.has_more_tokens()) {
    bookdatabase.list_book();
    return;
  }
  tokenscanner.set_whether_cut_up_equal_sign(true);
  std::string token = tokenscanner.next_token();
  tokenscanner.set_whether_cut_up_equal_sign(false);
  if (token == "-ISBN=") {
    std::vector <int> index_list;
    char ISBN[21];
    if (!tokenscanner.has_more_tokens()) {
      throw 1;
    }
    tokenscanner.set_char(ISBN);
    index_list = bookdatabase.search_book(ISBN);
    if (index_list.empty()) {
      std::cout << std::endl;
      return;
    }
    if (tokenscanner.has_more_tokens()) {
      throw 1;
    }
    bookdatabase.list_book(index_list);
  } else {
    std::vector<std::string> index_list;
    tokenscanner.set_get_quotation_content(true);
    if (!tokenscanner.has_more_tokens()) {
      throw 1;
    }
    if (token == "-name=") {
      char name[61];
      tokenscanner.set_char(name);
      index_list = nameindex.search_book(name);
      if (index_list.empty()) {
        std::cout << std::endl;
        tokenscanner.set_get_quotation_content(false);
        return;
      }
    } else if (token == "-author=") {
      char author[61];
      tokenscanner.set_char(author);
      index_list = authorindex.search_book(author);
      if (index_list.empty()) {
        std::cout << std::endl;
        tokenscanner.set_get_quotation_content(false);
        return;
      }
    } else if (token == "-keyword=") {
      char keyword[61];
      tokenscanner.set_char(keyword);
      index_list = keywordindex.search_book(keyword);
      if (index_list.empty()) {
        std::cout << std::endl;
        tokenscanner.set_get_quotation_content(false);
        return;
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
  if (!tokenscanner.has_more_tokens()) {
    throw 1;
  }
  tokenscanner.set_char(ISBN);
  if (tokenscanner.has_more_tokens()) {
    throw 1;
  }
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
      if (!bookdatabase.search_book(isbn).empty()) {
        throw 1;
      }
    } else if (token == "-price=") {
      char *pend;
      price = strtof(tokenscanner.next_token().c_str(), &pend);
      if (strlen(pend)) {
        throw 1;
      }
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
  if (tokenscanner.has_more_tokens()) {
    throw 1;
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
  if (!tokenscanner.has_more_tokens()) {
    throw 1;
  }
  tokenscanner.set_char(isbn);
  try {
    quantity = tokenscanner.next_number();
  } catch (int) {
    throw 1;
  }
  if (quantity <= 0) {
    throw 1;
  }
  if (tokenscanner.has_more_tokens()) {
    throw 1;
  }
  if (bookdatabase.search_book(isbn).empty()) {
    throw 1;
  } else {
    Book book;
    book = bookdatabase.query_book(bookdatabase.search_book(isbn)[0]);
    if (!book.check_enough(quantity)) {
      throw 1;
    } else {
      book.change_quantity(-quantity);
      Book new_book(-1, book.show_quantity());
      bookdatabase.update_book(bookdatabase.search_book(isbn)[0], new_book);
      Log.update_finance(book.show_book_price() * quantity, false);
      std::cout << setiosflags(std::ios::fixed) << std::setprecision(2) << book.show_book_price() * quantity << std::endl;
      return;
    }
  }
}

void BookOperator::import_book() {
  char *pend;
  int quantity;
  double cost;
  if (selected_book == -1) {
    throw 1;
  }
  if (!tokenscanner.has_more_tokens()) {
    throw 1;
  }
  if (!tokenscanner.has_more_tokens()) {
    throw 1;
  }
  quantity = tokenscanner.next_number();
  if (!tokenscanner.has_more_tokens()) {
    throw 1;
  }
  cost = std::strtof(tokenscanner.next_token().c_str(), &pend);
  if (strlen(pend)) {
    throw 1;
  }
  if (tokenscanner.has_more_tokens()) {
    throw 1;
  }
  Book book(-1, quantity + bookdatabase.query_book(selected_book).show_quantity());
  bookdatabase.update_book(selected_book, book);
  Log.update_finance(cost, true);
  return;
}