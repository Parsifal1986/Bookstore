#include "acm2074(another_method).hpp"

struct keys{
  char index[64] = {'\0'};
  int value = 0;

  bool operator<(const keys &other) const {
    int i = 0;
    while(this->index[i] != '\0' && other.index[i] != '\0') {
      if (this->index[i] != other.index[i]) {
        return this->index[i] < other.index[i];
      }
      ++i;
    }
    if (this->index[i] == '\0' && other.index[i] != '\0') {
      return true;
    } else if (this->index[i] != '\0' && other.index[i] == '\0') {
      return false;
    }
    return this->value < other.value;
  }

  bool operator==(const keys &other) const {
    int i = 0;
    while (this->index[i] != '\0' && other.index[i] != '\0') {
      if (this->index[i] != other.index[i]) {
        return false;
      }
      ++i;
    }
    if (this->index[i] == other.index[i] && this->index[i] == '\0'){
      return this->value == other.value;
    } else {
      return false;
    }
  }

  bool Equal(const keys b) {
    int i = 0;
    while (this->index[i] != '\0' && b.index[i] != '\0') {
      if (this->index[i] != b.index[i]) {
        return false;
      }
      ++i;
    }
    if (this->index[i] == b.index[i] && this->index[i] == '\0') {
      return true;
    }
    return false;
  }

  // std::ostream& operator<<(std::ostream &out, const keys &other) {
  //   out << other.value;
  //   return out;
  // }

};

int main() {
  Linklist<keys> linklist;
  std::ios::sync_with_stdio(false);
  std::cin.tie(0);std::cout.tie(0);
  keys value;
  std::string command;
  int n;
  std::cin >> n;
  for (int i = 0; i < n; ++i) {
    std::cin >> command;
    if (command == "insert") {
      std::cin >> value.index >> value.value;
      linklist.Insert(value);
    } else if (command == "find") {
      std::cin >> value.index;value.value = 0;
      linklist.Find(value);
    } else if (command == "delete") {
      std::cin >> value.index >> value.value;
      linklist.Delete(value);
    }
  }
  return 0;
}