#ifndef TOKENSCANNER_HPP
#define TOKENSCANNER_HPP

#include <iostream>
#include <cstring>
#include <vector>

class Tokenscanner {
 public:
  Tokenscanner() = default;

  Tokenscanner(const std::string &line) : line(line) {}

  ~Tokenscanner() = default;

  void set_line(const std::string &line) {
    this->line = line;
    pos = 0;
    tokens.clear();
  }

  std::string next_token() {
    if (pos >= line.length())
      return "";
    while (line[pos] == ' ')
      ++pos;
    int start = pos;
    while (pos < line.length() && line[pos] != ' ') {
      ++pos;
      if (cut_up_equal_sign && line[pos] == '=') {
        ++pos;
        break;
      }
      if (devide_by_slash && line[pos] == '|') {
        ++pos;
        break;
      }
    }
    if (devide_by_slash) {
      return line.substr(start, pos - start - 1);
    }
    if (get_quotation_content) {
      return line.substr(start + 1, pos - start - 2);
    } else {
      return line.substr(start, pos - start);
    }
  }

  int next_number() {
    if (pos >= line.length())
      return 0;
    while (line[pos] == ' ')
      ++pos;
    int start = pos;
    while (pos < line.length() && line[pos] != ' ' && isdigit(line[pos])) {
      ++pos;
      if (cut_up_equal_sign && line[pos] == '=') {
        ++pos;
        break;
      }
    }
    return std::stoi(line.substr(start, pos - start));
  }

  bool has_more_tokens() {
    if (pos >= line.length())
      return false;
    while (line[pos] == ' ')
      ++pos;
    return pos < line.length();
  }

  void set_ignore_whitespace(bool ignore_whitespace) {
    if (ignore_whitespace) {
      while (line[pos] == ' ') {
        ++pos;
      }
    }
  }

  void set_ignore_case(bool ignore_case) {
    if (ignore_case) {
      for (int i = 0; i < line.length(); ++i) {
        line[i] = tolower(line[i]);
      }
    }
  };

  void set_numbers_as_string(bool numbers_as_string) {
    if (numbers_as_string) {
      for (int i = 0; i < line.length(); ++i) {
        if (isdigit(line[i])) {
          line[i] = '0';
        }
      }
    }
  };

  void set_scan_numbers(bool scan_numbers) {
    if (scan_numbers) {
      for (int i = 0; i < line.length(); ++i) {
        if (isdigit(line[i])) {
          line[i] = '0';
        }
      }
    }
  };

  void set_scan_strings(bool scan_strings) {
    if (scan_strings) {
      for (int i = 0; i < line.length(); ++i) {
        if (line[i] == '"') {
          line[i] = '0';
        }
      }
    }
  };

  void set_char(char* str) {
    strcpy(str, next_token().c_str());
    return;
  }

  void set_string_quotes(const std::string &string_quotes) {
    for (int i = 0; i < line.length(); ++i) {
      if (line[i] == string_quotes[0])
        line[i] = '"';
      else if (line[i] == string_quotes[1])
        line[i] = '"';
    };
  };

  void set_whether_cut_up_equal_sign(bool cut_up_equal_sign) {
    this->cut_up_equal_sign = cut_up_equal_sign;
  }

  void set_get_quotation_content(bool get_quotation_content) {
    this->get_quotation_content = get_quotation_content;
  }

  void set_devide_by_slash(bool devide_by_slash) {
    this->devide_by_slash = devide_by_slash;
  }

 private:
  std::string line;
  int pos = 0;
  std::vector<std::string> tokens;
  bool cut_up_equal_sign = false;
  bool get_quotation_content = false;
  bool devide_by_slash = false;
};

#endif //TOKENSCANNER_HPP