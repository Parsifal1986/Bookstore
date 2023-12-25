#ifndef TOKENSCANNER_HPP
#define TOKENSCANNER_HPP

#include <cctype>
#include <iostream>
#include <cstring>
#include <stdexcept>
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
    pre_pos = pos;
    if (pos >= line.length())
      return "";
    while (line[pos] == ' ')
      ++pos;
    int start = pos;
    if (devide_by_slash && line[pos] == '|') {
      throw 1;
    }
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
    if (word_limit != -1 && pos - start > word_limit) {
      throw 1;
    }
    if (devide_by_slash) {
      if (pos == line.length()){
        return line.substr(start, pos - start);
      } else {
        return line.substr(start, pos - start - 1);
      }
    }
    return line.substr(start, pos - start);
  }

  int next_number() {
    if (pos >= line.length())
      return 0;
    while (line[pos] == ' ')
      ++pos;
    int start = pos;
    if (!std::isdigit(line[pos])) {
      throw 1;
    }
    while (pos < line.length() && line[pos] != ' ' && isdigit(line[pos])) {
      ++pos;
      if (cut_up_equal_sign && line[pos] == '=') {
        ++pos;
        break;
      }
    }
    if (pos - start > 10) {
      throw 1;
    }
    int result;
    try {
      result = std::stoi(line.substr(start, pos - start));
    } catch (std::out_of_range) {
      throw 1;
    }
    return result;
  }

  std::string next_quoted_token() {
    std::string tmp = next_token();
    if (tmp[0] != '"' || tmp[tmp.length() - 1] != '"') {
      throw 1;
    }
    if (tmp.length() <= 2 ) {
      throw 1;
    }
    return tmp.substr(1, tmp.length() - 2);
  }

  bool has_more_tokens() {
    if (pos >= line.length())
      return false;
    while (line[pos] == ' ')
      ++pos;
    return pos < line.length();
  }

  void scroll_back() {
    pos = pre_pos;
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

  void set_devide_by_slash(bool devide_by_slash) {
    this->devide_by_slash = devide_by_slash;
  }

  void set_word_limit(int word_limit) {
    this->word_limit = word_limit;
  }

  bool is_legal(std::string str) {
    for (int i = 0; i < str.length(); ++i) {
      if (!std::isalpha(str[i]) && !std::isdigit(str[i]) && str[i] != '_') {
        return false;
      }
    }
    return true;
  }

  bool is_printable(std::string str) {
    for (int i = 0; i < str.length(); ++i) {
      if (!std::isprint(str[i])) {
        return false;
      }
    }
    return true;
  }

  bool has_quotatioin_mark(std::string str) {
    for (int i = 0; i < str.length(); ++i) {
      if (str[i] == '"') {
        return true;
      }
    }
    return false;
  }

  bool check_specific_char(std::string str, char ch) {
    for (int i = 0; i < str.length(); ++i) {
      if (str[i] == ch) {
        return true;
      }
    }
    return false;
  }

  bool detect_white_space() {
    if (line[pos] == ' ') {
      return true;
    }
    return false;
  }

  std::string ignore_white_space() {
    std::string tmp;
    bool flag = 0;
    for (int i = 0; i < line.length(); ++i) {
      if (line[i] == ' ') {
        if (!flag) {
          tmp += line[i];
          flag = 1;
        }
      } else {
        if (line[i] != ' ') {
          tmp += line[i];
          flag = 0;
        }
      }
    }
    return tmp;
  };

 private:
  std::string line;
  int pos = 0, pre_pos = 0;
  std::vector<std::string> tokens;
  bool cut_up_equal_sign = false;
  bool devide_by_slash = false;
  int word_limit = -1;
};

#endif //TOKENSCANNER_HPP