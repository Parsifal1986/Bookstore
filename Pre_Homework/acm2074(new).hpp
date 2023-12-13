#ifndef KEY_VALUE_DATABASE
#define KEY_VALUE_DATABASE
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
#include <queue>
#include <regex>
#include <string>

const int RANK_Node = 56;
const int RANK_Data = 59;

template <class T, int sizeofT = sizeof(T)> class Bp_Tree {
private:
  std::string file_name;
  std::fstream file;

  struct Node {
    T key[RANK_Node + 1];
    int next[RANK_Node + 2];
    int parent = -1;
    bool is_leaf = 0;
    int size = 0;
  };

  struct Data {
    int size = 0;
    T key[RANK_Data + 1];
    int next = -1, prev = -1;
  };

  int sizeofNode;

  int sizeofData;

  struct Carry {
    bool if_carry = false;
    T value;
  }carry;

public:
  int root_position;

  Bp_Tree(std::string name = "Bp_Tree") {
    file_name = name;
    file.open(file_name, std::ios::out);
    file.close();
    file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    Node *root = new Node();
    root->is_leaf = true;
    memset(root->next, -1, sizeof(root->next));
    sizeofNode = sizeof(Node);
    file.write(reinterpret_cast<char *>(root), sizeofNode);
    root_position = 0;
    delete root;
  };

  ~Bp_Tree() {
    file.clear();
    file.close();
  };

  void Insert(T &value, int position) {
    file.seekg(position, std::ios::beg);
    Node *cur = new Node;
    file.read(reinterpret_cast<char *>(cur), sizeofNode);
    if (cur->is_leaf) {
      int i = 0;
      Data *data = new Data;
      for (; i < cur->size; ++i) {
        if (value < cur->key[i]) {
          break;
        }
      }
      if (cur->next[i] == -1) {
        file.seekp(0, std::ios::end);
        cur->next[i] = file.tellp();
      } else {
        file.seekg(cur->next[i], std::ios::beg);
        file.read(reinterpret_cast<char *>(data), sizeofData);
      }
      auto it = std::upper_bound(data->key, data->key + data->size, value);
      if (it == data->key) {
        carry.if_carry = true;
        carry.value = value;
      }
      for (int j = data->size; j > it - data->key; --j) {
        data->key[j] = data->key[j - 1];
      }
      data->key[it - data->key] = value;
      data->size++;
      if (!(cur->key[i] == data->key[0])) {
        cur->key[i] = data->key[0];
        file.seekp(position, std::ios::beg);
        file.write(reinterpret_cast<char *>(cur), sizeofNode);
      }
      file.seekp(cur->next[i], std::ios::beg);
      if (data->size == RANK_Data + 1) {
        split_data(cur, data, i);
        file.write(reinterpret_cast<char *>(cur), sizeofNode);
      } else {
        file.write(reinterpret_cast<char *>(data), sizeofData);
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
      if (carry.if_carry) {
        carry.if_carry = false;
        cur->key[i] = carry.value;
        if (i == 0) {
          carry.if_carry = true;
        }
        file.seekp(position, std::ios::beg);
        file.write(reinterpret_cast<char *>(cur), sizeofNode);
      }
    }
    if (cur->size == RANK_Node + 1) {
      split(cur);
      file.seekp(position, std::ios::beg);
    }
    delete cur;
    return;
  };

  void Delete(T value, int position) {
    Node *cur = new Node;
    file.seekg(position, std::ios::beg);
    file.read(reinterpret_cast<char *>(cur), sizeofNode);
    if (cur->is_leaf) {
      Data *data = new Data;
      int i = 0;
      for (; i < cur->size; ++i) {
        if (value < cur->key[i]) {
          break;
        }
      }
      file.seekg(cur->next[i], std::ios::beg);
      file.read(reinterpret_cast<char *>(data), sizeofData);
      file.seekg(cur->next[i], std::ios::beg);
      auto it = std::lower_bound(data->key, data->key + data->size, value);
      if (it == data->key + data->size || value < *it) {
        return;
      }
      for (int j = it - data->key; j < data->size - 1; ++j) {
        data->key[j] = data->key[j + 1];
      }
      if (it == data->key) {
        carry.if_carry = true;
        carry.value = data->key[0];
      }
      data->size--;
      if (data->size < RANK_Data / 2 && position != root_position) {
        if (i == 0 && data->next != -1) {
          file.seekg(data->next, std::ios::beg);
          Data *brother = new Data;
          file.read(reinterpret_cast<char *>(brother), sizeofData);
          if (brother->size > RANK_Data / 2) {
            cur->key[cur->size] = brother->key[0];
            cur->size++;
            for (int j = 0; j < brother->size - 1; ++j) {
              brother->key[j] = brother->key[j + 1];
            }
            brother->size--;
            cur->key[i + 1] = brother->key[0];
          } else {
            merge_data(data, brother);
            data->next = brother->next;
          }
          file.seekp(-sizeofData, std::ios::cur);
          file.write(reinterpret_cast<char *>(brother), sizeofData);
          file.seekp(brother->prev, std::ios::beg);
          file.write(reinterpret_cast<char *>(data), sizeofData);
          delete brother;
        } else {
          file.seekg(cur->next[i - 1], std::ios::beg);
          Data *brother = new Data;
          file.read(reinterpret_cast<char *>(brother), sizeofData);
          if (brother->size > RANK_Data / 2) {
            for (int j = 0; j < brother->size - 1; ++j) {
              cur->key[j] = cur->key[j + 1];
            }
            cur->key[0] = brother->key[brother->size];
            cur->size++;
            brother->size--;
            cur->key[i] = data->key[0];
          } else {
            merge_data(data, brother);
            data->prev = brother->prev;
          }
          file.seekp(-sizeofData, std::ios::cur);
          file.write(reinterpret_cast<char *>(brother), sizeofData);
          file.seekp(brother->next, std::ios::beg);
          file.write(reinterpret_cast<char *>(data), sizeofData);
          delete brother;
        }
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
      if (carry.if_carry) {
        carry.if_carry = false;
        cur->key[i] = carry.value;
        if (i == 0) {
          carry.if_carry = true;
        }
      }
      if (cur->size < RANK_Node / 2 && cur->parent != -1) {
        merge(cur);
      } else {
        file.seekp(position, std::ios::beg);
        file.write(reinterpret_cast<char *>(cur), sizeofNode);
      }
    }
    delete cur;
    return;
  };

  void Find(T &value) {
    Node *cur = new Node;
    file.seekg(root_position, std::ios::beg);
    file.read(reinterpret_cast<char *>(cur), sizeofNode);
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
    Data *data = new Data;
    int i = 0;
    for (; i < cur->size; ++i) {
      if (value < cur->key[i]) {
        file.seekg(cur->next[i], std::ios::beg);
        file.read(reinterpret_cast<char *>(data), sizeofData);
        break;
      }
      if (i == cur->size - 1) {
        file.seekg(cur->next[i + 1], std::ios::beg);
        file.read(reinterpret_cast<char *>(data), sizeofData);
        break;
      }
    }
    for (int i = 0; i < data->size; ++i) {
      if (value.Equal(data->key[i])) {
        flag = 1;
        while (value.Equal(data->key[i]) && i < data->size) {
          std::cout << data->key[i].value << "\n";
          i++;
          if (i == data->size && data->next != -1) { 
            file.seekg(data->next, std::ios::beg);
            file.read(reinterpret_cast<char *>(data), sizeofData);
            i = 0;
          }
        }
      }
    }
    if (!flag) {
      std::cout << "null"
                << "\n";
    }
    return;
  };

  void merge(Node *cur) {
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
      brother->key[brother->size] = cur->key[0];
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

  void merge_data(Data *cur, Data *brother) {
    for (int i = 0; i < brother->size; ++i) {
      cur->key[cur->size] = brother->key[i];
      cur->size++;
    }
    return;
  }

  void split(Node *cur) {
    Node *new_node = new Node;
    for (int i = 0; i < RANK_Node / 2; ++i) {
      new_node->key[i] = cur->key[i + RANK_Node / 2 + 1];
      new_node->next[i] = cur->next[i + RANK_Node / 2 + 1];
    }
    int cur_pos = int(file.tellp());
    cur->size = RANK_Node / 2;
    new_node->size = RANK_Node / 2;
    new_node->parent = cur->parent;
    new_node->is_leaf = cur->is_leaf;
    file.seekp(0, std::ios::end);
    if (!new_node->is_leaf) {
      change_sons_parent(new_node);
    }
    file.seekp(0,std::ios::end);
    if (cur->parent == -1) {
      Node *new_root = new Node;
      new_root->is_leaf = false;
      new_root->size = 1;
      new_root->key[0] = new_node->key[0];
      new_root->next[0] = cur_pos;
      new_root->next[1] = int(file.tellp());
      new_node->parent = int(file.tellp()) + sizeofNode;
      file.write(reinterpret_cast<char *>(new_node), sizeofNode);
      cur->parent = int(file.tellp());
      file.write(reinterpret_cast<char *>(new_root), sizeofNode);
      root_position = (file.tellp());
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
          parent->key[i] = cur->key[RANK_Node / 2];
          parent->next[i + 1] = int(file.tellp());
          parent->size++;
          break;
        }
      }
      if (i == parent->size) {
        parent->key[i] = cur->key[RANK_Node / 2];
        parent->next[i + 1] = int(file.tellp());
        parent->size++;
      }
      file.write(reinterpret_cast<char *>(new_node), sizeofNode);
      file.seekp(cur_pos, std::ios::beg);
      file.write(reinterpret_cast<char *>(cur), sizeofNode);
      file.seekg(cur->parent, std::ios::beg);
      file.write(reinterpret_cast<char *>(parent), sizeofNode);
      if (parent->size == 4) {
        split(parent);
      }
      delete parent;
    };
    delete new_node;
    return;
  };

  void split_data(Node* cur, Data* data, int position) {
    Data *new_data = new Data;
    for (int i = 0; i <= RANK_Data / 2; ++i) {
      new_data->key[i] = data->key[i + RANK_Data / 2 + 1];
    }
    new_data->size = RANK_Data / 2 + 1;
    data->size = RANK_Data / 2 + 1;
    new_data->next = data->next;
    new_data->prev = file.tellp();
    for (int i = cur->size; i > position; --i) {
      cur->key[i] = cur->key[i - 1];
      cur->next[i + 1] = cur->next[i];
    }
    cur->key[position] = new_data->key[0];
    cur->next[position + 1] = file.tellp();
    cur->size++;
    file.seekp(0, std::ios::end);
    data->next = file.tellp();
    file.write(reinterpret_cast<char*>(new_data), sizeofData);
    file.seekp(new_data->prev, std::ios::beg);
    file.write(reinterpret_cast<char*>(data), sizeofData);
    return;
  }

  void change_sons_parent(Node *cur) {
    int cur_pos = int(file.tellp()) - sizeofNode;
    for (int i = 0; i < cur->size + 1; ++i) {
      file.seekg(cur->next[i], std::ios::beg);
      Node *son = new Node;
      file.read(reinterpret_cast<char *>(son), sizeofNode);
      son->parent = cur_pos;
      file.seekp(-sizeofNode, std::ios::cur);
      file.write(reinterpret_cast<char *>(son), sizeofNode);
      delete son;
    }
    return;
  };
};

#endif // KEY_VALUE_DATABASE