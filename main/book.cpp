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

void Book::create_book(char *isbn, char *name, char *author, char *keyword, double price, int quantity) {
  strcpy(this->isbn, isbn);
  strcpy(this->name, name);
  strcpy(this->author, author);
  strcpy(this->keyword, keyword);
  this->price = price;
  this->quantity = quantity;
  return;
};

void Book::modify_book(Book &new_book) {
  if (strcmp(isbn, new_book.isbn)) {
    std::cout << "Didn't change ISBN" << std::endl;
    return;
  }
  if (strlen(new_book.isbn)) {
    strcpy(this->isbn, new_book.isbn);
  }
  if (strlen(new_book.name)) {
    strcpy(this->name, new_book.name);
  }
  if (strlen(new_book.name)) {
    strcpy(this->author, new_book.author);
  }
  if (strlen(new_book.name)) {
    strcpy(this->keyword, new_book.keyword);
  }
  if (new_book.price != 0) {
    this->price = new_book.price;
  }
  return;
};

void Book::show_book() {
  printf("%s %s %s %s %.2f %d\n", isbn, name, author, keyword, price, quantity);
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
  
  memoryriver.update(book, index);
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

std::vector<int> ISBNIndex::search_book(char *key) {
  std::vector<int> result;
  result.clear();
  if (Index.find(key) == Index.end()) {
    return result;
  } else {
    result.push_back(Index[key]);
    return result;
  }
}

void ISBNIndex::add_index(char *key, int index) {
  if (!this->search_book(key).empty()) {
    return;
  }
  Index[key] = index;
  return;
}

void ISBNIndex::delete_index(char *key, int index) {
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

void BookOperator::select_book() {
  char ISBN[21];
  std::cin >> ISBN;
  if (isbnindex.search_book(ISBN).empty()) {
    Book new_book;
    new_book.create_book(ISBN, nullptr, nullptr, nullptr, 0, 0);
    isbnindex.add_index(ISBN, bookdatabase.add_book(new_book));
    selected_book = isbnindex.search_book(ISBN)[0];
  } else {
    selected_book = isbnindex.search_book(ISBN)[0];
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
  int quantity = 0;
  float price = 0;
  std::string input;
  getline(std::cin, input);
  Tokenscanner scanner(input);
  while (scanner.has_more_tokens()) {
    std::string token = scanner.next_token();
    if (token == "-ISBN") {
      strcpy(isbn, scanner.next_token().c_str());
    } else if (token == "-name") {
      strcpy(name, scanner.next_token().c_str());
    } else if (token == "-author") {
      strcpy(author, scanner.next_token().c_str());
    } else if (token == "-keyword") {
      strcpy(keyword, scanner.next_token().c_str());
    } else if (token == "-price") {
      price = atof(scanner.next_token().c_str());
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
  if (isbnindex.search_book(isbn).empty()) {
    std::cout << "No such book" << std::endl;
    return;
  } else {
    Book book;
    book = bookdatabase.query_book(isbnindex.search_book(isbn)[0]);
    if (book.check_enough(quantity)) {
      std::cout << "Book is not enough" << std::endl;
      return;
    } else {
      book.change_quantity(-quantity);
      bookdatabase.update_book(isbnindex.search_book(isbn)[0], book);
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
    if (isbnindex.search_book(isbn).empty()) {
        std::cout << "No such book" << std::endl;
        return;
    } else {
        Book book;
        book = bookdatabase.query_book(isbnindex.search_book(isbn)[0]);
        book.change_quantity(quantity);
        bookdatabase.update_book(isbnindex.search_book(isbn)[0], book);
        std::cout << "Success" << std::endl;
        return;
    }
}