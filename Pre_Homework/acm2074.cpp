#include "acm2074.hpp"

struct keys{
  char index[64] = {0};
  int value = 0;

  bool operator<(const keys &other) const {
    for (int i = 0; i < 64; ++i) {
      if (this->index[i] != other.index[i]) {
        return this->index[i] < other.index[i];
      }
    }
    return this->value < other.value;
  }

  bool operator==(const keys &other) const {
    for (int i = 0; i < 64; ++i) {
      if (this->index[i] != other.index[i]) {
        return false;
      }
    }
    return this->value == other.value;
  }

  bool Equal(const keys b) {
    for (int i = 0; i < 64; ++i) {
      if (this->index[i] != b.index[i]) {
        return false;
      }
    }
    return true;
  }

  // std::ostream &operator<<(std::ostream &out, const keys &other) {
  //   out << other.value;
  //   return out;
  // }

};

int main() {
  Bp_Tree<keys, sizeof(int) + 64> tree("BP_Tree");
  keys value;
  std::string command;
  int n;
  std::cin >> n;
  for (int i = 0; i < n; ++i) {
    std::cin >> command;
    if (command == "insert") {
      std::cin >> value.index >> value.value;
      tree.Insert(value, tree.root_position);
    } else if (command == "find") {
      std::cin >> value.index;value.value = 0;
      tree.Find(value);
    } else if (command == "delete") {
      std::cin >> value.index >> value.value;
      tree.Delete(value, tree.root_position);
    }
  }
  return 0;
}