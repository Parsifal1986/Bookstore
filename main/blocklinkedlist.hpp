#ifndef ACM2074
#define ACM2074
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
#include <queue>
#include <regex>
#include <string>

template<class T> class Linklist {
private:  

  constexpr int SIZE = (4096 / (sizeof(T) + sizeof(int)));

  struct Node {
    T value[SIZE];
    int next_pos = -1, prev_pos = -1;
    int size = 0;
  };

  int head = 0, sizeofNode;

  std::string file_name;

  std::fstream file;

public:
  Linklist() = default;

  ~Linklist() {
    file.close();
  }

  void initialise(std::string filename) {
    file_name = filename;
    file.open(file_name, std::ios::out | std::ios::in | std::ios::binary);
    sizeofNode = sizeof(Node);
    if (!file) {
      file.open(file_name, std::ios::out);
      head = 0;
      Node *head = new Node;
      head->size = 0;
      file.write(reinterpret_cast<char *>(head), sizeofNode);
      file.close();
      file.open(file_name, std::ios::out | std::ios::in | std::ios::binary);
      delete head;
    }
  }

  void set_filename(std::string filename) {
    file_name = filename;
  }

  void Insert(T value) {
    Node *p = new Node;
    file.seekg(head);
    file.read(reinterpret_cast<char *>(p), sizeofNode);
    while (p->value[p->size - 1] < value && p->next_pos != -1) {
      file.seekg(p->next_pos);
      file.read(reinterpret_cast<char *>(p), sizeofNode);
    }
    T *position = std::lower_bound(p->value, p->value + p->size, value);
    int index = position - p->value;
    for (int i = p->size; i > index; --i) {
      p->value[i] = p->value[i - 1];
    }
    p->value[index] = value;
    ++p->size;
    if (p->size == SIZE) {
      Split(p);
    } else {
      file.seekp(-sizeofNode, std::ios::cur);
      file.write(reinterpret_cast<char *>(p), sizeofNode);
    }
    delete p;
    return;
  }

  void Delete(T value) {
    Node *p = new Node;
    file.seekg(head);
    file.read(reinterpret_cast<char *>(p), sizeofNode);
    while (p->value[p->size - 1] < value && p->next_pos != -1) {
      file.seekg(p->next_pos);
      file.read(reinterpret_cast<char *>(p), sizeofNode);
    }
    T *position = std::lower_bound(p->value, p->value + p->size, value);
    if (*position == value) {
      int index = position - p->value;
      for (int i = index; i < p->size - 1; ++i) {
        p->value[i] = p->value[i + 1];
      }
      --p->size;
      int cur_pos = int(file.tellg()) - sizeofNode;
      if (p->size < SIZE / 2) {
        Node *brother = new Node;
        if (p->next_pos != -1) {
          file.seekg(p->next_pos, std::ios::beg);
          file.read(reinterpret_cast<char *>(brother), sizeofNode);
          if (brother->size > SIZE / 2) {
            p->value[p->size] = brother->value[0];
            ++p->size;
            for (int i = 0; i < brother->size - 1; ++i) {
              brother->value[i] = brother->value[i + 1];
            }
            --brother->size;
            file.seekp(-sizeofNode, std::ios::cur);
            file.write(reinterpret_cast<char *>(brother), sizeofNode);
            file.seekp(cur_pos, std::ios::beg);
            file.write(reinterpret_cast<char *>(p), sizeofNode);
          } else {
            Merge(p, brother);
            file.seekp(cur_pos, std::ios::beg);
            file.write(reinterpret_cast<char *>(p), sizeofNode);
          }
        } else if (p->prev_pos != -1) {
          Node *brother = new Node;
          file.seekg(p->prev_pos, std::ios::beg);
          file.read(reinterpret_cast<char *>(brother), sizeofNode);
          if (brother->size > SIZE / 2) {
            for (int i = p->size; i > 0; --i) {
              p->value[i] = p->value[i - 1];
            }
            p->value[0] = brother->value[brother->size - 1];
            ++p->size;
            --brother->size;
            file.seekp(-sizeofNode, std::ios::cur);
            file.write(reinterpret_cast<char *>(brother), sizeofNode);
            file.seekp(cur_pos, std::ios::beg);
            file.write(reinterpret_cast<char *>(p), sizeofNode);
          } else {
            Merge(brother, p);
            file.seekp(-sizeofNode, std::ios::cur);
            file.write(reinterpret_cast<char *>(brother), sizeofNode);
          }
        } else {
          file.seekp(-sizeofNode, std::ios::cur);
          file.write(reinterpret_cast<char *>(p), sizeofNode);
        }
        delete brother;
      } else {
        file.seekp(-sizeofNode, std::ios::cur);
        file.write(reinterpret_cast<char *>(p), sizeofNode);
      }
    }
    delete p;
    return;
  }

  std::vector<std::string> Find(T value) {
    std::vector<std::string> result;
    Node *p = new Node;
    file.seekg(head);
    file.read(reinterpret_cast<char *>(p), sizeofNode);
    while (p->value[p->size - 1] < value && p->next_pos != -1) {
      file.seekg(p->next_pos);
      file.read(reinterpret_cast<char *>(p), sizeofNode);
    }
    for (int i = 0; i < p->size; ++i) {
      if (p->value[i].Equal(value)) {
        while(p->value[i].Equal(value)) {
          result.push_back(p->value[i].isbn);
          ++i;
          if (i == p->size) {
            if (p->next_pos != -1) {
              file.seekg(p->next_pos, std::ios::beg);
              file.read(reinterpret_cast<char *>(p), sizeofNode);
              i = 0;
            } else {
              break;
            }
          }
        }
        break;
      }
    }
    delete p;
    return result;
  }

  void Split(Node *p) {
    Node *q = new Node;
    q->next_pos = p->next_pos;
    q->prev_pos = int(file.tellg()) - sizeofNode;
    for (int i = 0; i < SIZE / 2; ++i) {
      q->value[i] = p->value[i + SIZE / 2];
    }
    q->size = SIZE / 2;
    p->size = SIZE / 2;
    file.seekp(0, std::ios::end);
    file.write(reinterpret_cast<char *>(q), sizeofNode);
    p->next_pos = int(file.tellp()) - sizeofNode;
    file.seekp(q->prev_pos, std::ios::beg);
    file.write(reinterpret_cast<char *>(p), sizeofNode);
    delete q;
  }

  void Merge(Node *p, Node *q) {
    for (int i = 0; i < q->size; ++i) {
      p->value[p->size + i] = q->value[i];
    }
    p->size += q->size;
    p->next_pos = q->next_pos;
    Node *r = new Node;
    if (q->next_pos != -1) {
      file.seekg(q->next_pos, std::ios::beg);
      file.read(reinterpret_cast<char *>(r), sizeofNode);
      r->prev_pos = q->prev_pos;
      file.seekp(-sizeofNode, std::ios::cur);
      file.write(reinterpret_cast<char *>(r), sizeofNode);
    }
    delete r;
    return;
  }
};

#endif // !ACM2074