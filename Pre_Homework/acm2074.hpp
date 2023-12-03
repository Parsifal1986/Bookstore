#ifndef KEY_VALUE_DATABASE
#define KEY_VALUE_DATABASE
#include <algorithm>
#include <cstring>
#include <ios>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <regex>
#include <string>

template <class T, int sizeofT = sizeof(T)> class Bp_Tree {
private:
  std::string file_name;
  std::fstream file;

  struct Node {
    T key[4];
    int next[5] = {-1, -1, -1, -1, -1};
    int parent = -1;
    bool is_leaf = 0;
    int size = 0;
  };

  int sizeofNode;

public:

  int root_position;

  Bp_Tree(std::string name = "Bp_Tree") {
    file_name = name;
    file.open(file_name, std::ios::out);
    file.close();
    file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    Node *root = new Node();
    root_position = 0;
    root->is_leaf = true;
    root->size = 0;
    root->key[0].value = 0;
    sizeofNode = sizeof(Node) - sizeof(T) + sizeofT;
    file.write(reinterpret_cast<char *>(root), sizeofNode);
    delete root;
  };

  ~Bp_Tree() {
    file.clear();
    file.close();
  };

  void Insert(T &value, int position) {
    file.seekg(position, std::ios::beg);
    Node *root = new Node();
    file.read(reinterpret_cast<char *>(root), sizeofNode);
    if (root->size == 0) {
      root->key[0] = value;
      root->size++;
      file.seekp(root_position, std::ios::beg);
      file.write(reinterpret_cast<char *>(root), sizeofNode);
      delete root;
      return;
    }
    Node *cur = root;
    if (root->is_leaf) {
      int i = 0;
      for (; i < cur->size; ++i) {
        if (value < cur->key[i]) {
          for (int j = cur->size; j > i; --j) {
            cur->key[j] = cur->key[j - 1];
          }
          cur->key[i] = value;
          cur->size++;
          break;
        }
      }
      if (i == cur->size){
        cur->key[i] = value;
        cur->size++;
      }
      cur->next[cur->size] = cur->next[cur->size - 1];
      file.seekp(-sizeofNode, std::ios::cur);
      file.write(reinterpret_cast<char *>(cur), sizeofNode);
      if (cur->size == 4) {
        split(cur);
      }
    } else {
      int i = 0;
      for (; i < cur->size; ++i) {
        if (value < cur->key[i]) {
          Insert(value, cur->next[i]);
          break;
        }
        if (i == cur->size - 1) {
          Insert(value, cur->next[i + 1]);
          ++i;
          break;
        }
      }
      if (i >= 1) {
        file.seekg(cur->next[i], std::ios::beg);
        Node *son = new Node;
        file.read(reinterpret_cast<char *>(son), sizeofNode);
        cur->key[i - 1] = son->key[0];
        delete son;
      }
    }
    delete cur;
    return;
  };

  void Delete(T value, int position) {
    file.seekg(position, std::ios::beg);
    Node *root = new Node;
    file.read(reinterpret_cast<char *>(root), sizeofNode);
    if (root->size == 0) {
      delete root;
      return;
    }
    Node *cur = root;
    if (root->is_leaf) {
      bool flag = 0;
      for (int i = 0; i < cur->size; ++i) {
        if (value == cur->key[i]) {
          for (int j = i; j < cur->size - 1; ++j) {
            cur->key[j] = cur->key[j + 1];
          }
          cur->next[cur->size - 1] = cur->next[cur->size];
          cur->size--;
          file.seekp(-sizeofNode, std::ios::cur);
          file.write(reinterpret_cast<char *>(cur), sizeofNode);
          flag = 1;
          break;
        }
      }
      if (cur->size < 2) {
        merge(cur);
      }
    } else {
      int i = 0;
      for (; i < cur->size; ++i) {
        if (value < cur->key[i]) {
          Delete(value, cur->next[i]);
          break;
        }
        if (i == cur->size - 1) {
          Delete(value, cur->next[i + 1]);
          break;
        }
      }
      file.seekg(position, std::ios::beg);
      file.read(reinterpret_cast<char *>(cur), sizeofNode);
      if (i >= 1) {
        file.seekg(cur->next[i], std::ios::beg);
        Node *son = new Node;
        file.read(reinterpret_cast<char *>(son), sizeofNode);
        cur->key[i - 1] = son->key[0];
        file.seekp(position, std::ios::beg);
        file.write(reinterpret_cast<char *>(cur), sizeofNode);
        delete son;
      }
      if (cur->size < 2 && cur->parent != -1) {
        merge(cur);
      }
    }
    delete cur;
    return;
  };

  void Find(T &value) {
    Node *root = new Node;//后续可能要考虑delete root的问题
    file.seekg(root_position, std::ios::beg);
    file.read(reinterpret_cast<char *>(root), sizeofNode);
    if (root->size == 0) {
      delete root;
      return;
    }
    Node *cur = root;
    bool flag = 0;
    while (!cur->is_leaf) {
      for (int i = 0; i < cur->size; ++i) {
        if (value < cur->key[i]) {
          file.seekg(cur->next[i], std::ios::beg);
          file.read(reinterpret_cast<char *>(cur), sizeofNode);
          break;
        }
        if (i == cur->size - 1) {
          file.seekg(cur->next[i + 1], std::ios::beg);
          file.read(reinterpret_cast<char *>(cur), sizeofNode);
          break;
        }
      }
    }
    int i = 0;
    while (cur->key[i] < value && i < cur->size) {
      i++;
      if (i == cur->size && cur->next[i] != -1) {
        file.seekg(cur->next[i], std::ios::beg);
        file.read(reinterpret_cast<char *>(cur), sizeofNode);
        i = 0;
      }
    }
    if (value.Equal(cur->key[i])) {
      flag = 1;
      while (value.Equal(cur->key[i]) && i < cur->size) {
        std::cout << cur->key[i].value << " ";
        i++;
        if (i == cur->size && cur->next[i] != -1) {
          file.seekg(cur->next[i], std::ios::beg);
          file.read(reinterpret_cast<char *>(cur), sizeofNode);
          i = 0;
        }
      }
      std::cout << "\n";
    }
    if (!flag) {
      std::cout << "null" << "\n";
    }
    return;
  };
  
  void merge(Node *cur) {
    if (cur->parent == -1) {
      return;
    }
    Node *parent = new Node;
    file.seekg(cur->parent, std::ios::beg);
    file.read(reinterpret_cast<char *>(parent), sizeofNode);
    int i = 0;
    for (; i < parent->size; ++i) {
      if (cur->key[0] < parent->key[i]) {
        break;
      }
    }
    if (i == 0) {
      Node *brother = new Node;
      file.seekg(parent->next[1], std::ios::beg);
      file.read(reinterpret_cast<char *>(brother), sizeofNode);
      if (brother->size < 4) {
        for (int j = cur->size; j > 0; --j) {
          cur->key[j] = cur->key[j - 1];
          cur->next[j] = cur->next[j - 1];
        }
        cur->key[0] = brother->key[0];
        cur->size++;
        for (int j = 0; j < brother->size - 1; ++j) {
          brother->key[j] = brother->key[j + 1];
          brother->next[j] = brother->next[j + 1];
        }
        brother->size--;
        parent->key[0] = brother->key[0];
        file.write(reinterpret_cast<char *>(cur), sizeofNode);
        file.seekp(parent->next[1], std::ios::beg);
        file.write(reinterpret_cast<char *>(brother), sizeofNode);
        file.seekp(cur->parent, std::ios::beg);
        file.write(reinterpret_cast<char *>(parent), sizeofNode);
      } else {
        split(brother);
        merge(cur);
      }
      delete brother;
    } else {
      int brother_pos = parent->next[i - 1];
      Node *brother = new Node;
      file.seekg(brother_pos, std::ios::beg);
      file.read(reinterpret_cast<char *>(brother), sizeofNode);
      brother->key[brother->size] = cur->key[i - 1];
      brother->size++;
      if (parent->size > 1) {
        for (int j = i; j < parent->size; ++j) {
          parent->key[j - 1] = parent->key[j];
          parent->next[j] = parent->next[j + 1];
        }
        parent->size--;
      } else {
        brother->parent = -1;
        root_position = parent->next[0];
      }
      brother->next[brother->size] = cur->next[1];
      file.seekg(brother_pos, std::ios::beg);
      file.write(reinterpret_cast<char *>(brother), sizeofNode);
      if (brother->parent != -1) {
        file.seekp(brother->parent, std::ios::beg);
        file.write(reinterpret_cast<char *>(parent), sizeofNode);
      }
      if (brother->size == 4) {
        file.seekp(brother_pos + sizeofNode, std::ios::beg);
        split(brother);
      }
      delete brother;
    }
    delete parent;
  };

  void split(Node *cur) {
    Node *new_node = new Node;
    for (int i = 0; i < 2; ++i) {
      new_node->key[i] = cur->key[i + 2];
    }
    int cur_pos = int(file.tellp()) - sizeofNode;
    cur->size = 2;
    new_node->is_leaf = cur->is_leaf;
    new_node->size = 2;
    new_node->next[2] = cur->next[4];
    new_node->parent = cur->parent;
    file.seekp(0, std::ios::end);
    int new_pos = (int)file.tellp();
    cur->next[2] = new_pos;
    file.write(reinterpret_cast<char *>(new_node), sizeofNode);
    file.seekp(cur_pos, std::ios::beg);
    file.write(reinterpret_cast<char *>(cur), sizeofNode);
    if (cur->parent == -1) {
      Node *new_root = new Node;
      new_root->is_leaf = false;
      new_root->size = 1;
      new_root->key[0] = new_node->key[0];
      new_root->next[0] = cur_pos;
      new_root->next[1] = new_pos;
      file.seekp(0, std::ios::end);
      cur->parent = (file.tellp());
      new_node->parent = (file.tellp());
      root_position = (file.tellp());
      file.write(reinterpret_cast<char *>(new_root), sizeofNode);
      file.seekp(new_pos, std::ios::beg);
      file.write(reinterpret_cast<char *>(new_node), sizeofNode);
      file.seekp(cur_pos, std::ios::beg);
      file.write(reinterpret_cast<char *>(cur), sizeofNode);
      delete new_root;
    } else {
      file.seekg(cur->parent, std::ios::beg);
      Node *parent = new Node;
      file.read(reinterpret_cast<char *>(parent), sizeofNode);
      file.seekp(cur->parent, std::ios::beg);
      int i = 0;
      for (; i < parent->size; ++i) {
        if (new_node->key[0] < parent->key[i]) {
          for (int j = parent->size; j > i; --j) {
            parent->key[j] = parent->key[j - 1];
            parent->next[j + 1] = parent->next[j];
          }
          parent->key[i] = new_node->key[0];
          parent->size++;
          break;
        }
        if (i == parent->size) {
          parent->key[i] = new_node->key[0];
          parent->size++;
        }
      }
      parent->next[i + 1] = new_pos;
      if (parent->size == 4) {
        split(parent);
      }
      file.seekg(cur->parent, std::ios::beg);
      file.write(reinterpret_cast<char *>(parent), sizeofNode);
      delete parent;
    };
    delete new_node;
    return;
  };
};

#endif //KEY_VALUE_DATABASE