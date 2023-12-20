#ifndef ERROR_HPP
#define ERROR_HPP

#include <exception>

class ErrorException : public std::exception {
 public:
  ErrorException(char *message) {
    this->message = message;
  }

  char *getMessage() const {
    return message;
  }
 
 private:
  char *message;
};

#endif // ERROR_HPP