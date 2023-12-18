#include "book.hpp"
#include "tokenscanner.hpp"
#include <cstring>
#include <vector>

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
    std::cout << "Didn't change ISBN" << std::endl;
    return;
  }
  if (strlen(new_book.isbn)) {
    strcpy(this->isbn, new_book.isbn);
  }
  if (strlen(new_book.name)) {
    strcpy(this->name, new_book.name);
  }
  if (strlen(new_book.author)) {
    strcpy(this->author, new_book.author);
  }
  if (strlen(new_book.author)) {
    strcpy(this->keyword, new_book.keyword);
  }
  if (new_book.price != -1) {
    this->price = new_book.price;
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

char *Book::show_book_name() {
  return name;
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
  old_book.modify_book(book);
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

void NameIndex::add_index(char *key, int index) {
  Node tmp;
  tmp.index = index;
  strcpy(tmp.name, key);
  Index.Insert(tmp);
  return;
}

void NameIndex::delete_index(char *key, int index) {
  Node tmp;
  tmp.index = index;
  strcpy(tmp.name, key);
  Index.Delete(tmp);
  return;
}

std::vector<int> NameIndex::search_book(char *key) {
  Node tmp;
  strcpy(tmp.name, key);
  return Index.Find(tmp);
}

void AuthorIndex::add_index(char *key, int index) {
  Node tmp;
  tmp.index = index;
  strcpy(tmp.author, key);
  Index.Insert(tmp);
  return;
}

void AuthorIndex::delete_index(char *key, int index) {
  Node tmp;
  tmp.index = index;
  strcpy(tmp.author, key);
  Index.Delete(tmp);
  return;
}

std::vector<int> AuthorIndex::search_book(char *key) {
  Node tmp;
  strcpy(tmp.author, key);
  return Index.Find(tmp);
}

void KeywordIndex::add_index(char *key, int index) {
  Node tmp;
  tmp.index = index;
  strcpy(tmp.keyword, key);
  Index.Insert(tmp);
  return;
}

void KeywordIndex::delete_index(char *key, int index) {
  Node tmp;
  tmp.index = index;
  strcpy(tmp.keyword, key);
  Index.Delete(tmp);
  return;
}

std::vector<int> KeywordIndex::search_book(char *key) {
  Node tmp;
  strcpy(tmp.keyword, key);
  return Index.Find(tmp);
}

void BookOperator::show_book() {
  tokenscanner.set_whether_cut_up_equal_sign(true);
  std::string token = tokenscanner.next_token();
  tokenscanner.set_whether_cut_up_equal_sign(false);
  std::vector <int> index_list;
  if (token == "-ISBN=") {
    char ISBN[21];
    tokenscanner.set_char(ISBN);
    index_list = bookdatabase.search_book(ISBN);
    if (index_list.empty()) {
      std::cout << "No such book" << std::endl;
      return;
    }
  } else {
    tokenscanner.set_get_quotation_content(true);
    if (token == "-name=") {
      char name[61];
      tokenscanner.set_char(name);
      index_list = nameindex.search_book(name);
      if (index_list.empty()) {
        std::cout << "No such book" << std::endl;
        return;
      }
    } else if (token == "-author=") {
      char author[61];
      tokenscanner.set_char(author);
      index_list = authorindex.search_book(author);
      if (index_list.empty()) {
        std::cout << "No such book" << std::endl;
        return;
      }
    } else if (token == "-keyword=") {
      char keyword[61];
      tokenscanner.set_char(keyword);
      index_list = keywordindex.search_book(keyword);
      if (index_list.empty()) {
        std::cout << "No such book" << std::endl;
        return;
      }
    } else {
      std::cout << "Invalid command" << std::endl;
      return;
    }
    tokenscanner.set_get_quotation_content(false);
  }
  if (tokenscanner.has_more_tokens()) {
    std::cout << "Invalid command" << std::endl;
  }
  bookdatabase.list_book(index_list);
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
    std::cout << "No book is selected" << std::endl;
    return;
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
  bookdatabase.update_book(selected_book, new_book);
  return;
}

void BookOperator::buy_book() {
  char isbn[21];
  int quantity;
  std::cin >> isbn >> quantity;
  if (bookdatabase.search_book(isbn).empty()) {
    std::cout << "No such book" << std::endl;
    return;
  } else {
    Book book;
    book = bookdatabase.query_book(bookdatabase.search_book(isbn)[0]);
    if (book.check_enough(quantity)) {
      std::cout << "Book is not enough" << std::endl;
      return;
    } else {
      book.change_quantity(-quantity);
      bookdatabase.update_book(bookdatabase.search_book(isbn)[0], book);
      std::cout << "Success" << std::endl;
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
        std::cout << "No such book" << std::endl;
        return;
    } else {
        Book book;
        book = bookdatabase.query_book(bookdatabase.search_book(isbn)[0]);
        book.change_quantity(quantity);
        bookdatabase.update_book(bookdatabase.search_book(isbn)[0], book);
        std::cout << "Success" << std::endl;
        return;
    }
}