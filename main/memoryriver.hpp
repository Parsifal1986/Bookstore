#ifndef BPT_MEMORYRIVER_HPP
#define BPT_MEMORYRIVER_HPP

#include <fstream>
#include <ios>
#include <vector>

using std::fstream;
using std::ifstream;
using std::ofstream;
using std::string;

template <class T, int info_len = 2> class MemoryRiver {
private:
  /* your code here */
  fstream file, info_file;
  string file_name, info_file_name;
  int sizeofT = sizeof(T);
  std::vector<int> deleted_index;

public:
  MemoryRiver() = default;

  ~MemoryRiver() {
    if (!deleted_index.empty()) {
      info_file.open(info_file_name, std::ios::in | std::ios::out);
      info_file.seekp(0, std::ios::beg);
      int tmp = deleted_index.size();
      info_file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
      for (int i = 0; i < deleted_index.size(); ++i) {
        info_file.write(reinterpret_cast<char *>(&deleted_index[i]), sizeof(int));
      }
      deleted_index.clear();
    }
  };

  MemoryRiver(const string &file_name) : file_name(file_name) {}

  void initialise(string FN = "") {
    if (FN != "")
      file_name = FN;
    file.open(file_name, std::ios::out | std::ios::in | std::ios::binary);
    if (!file) {
      file.open(file_name, std::ios::out);
      int tmp = 0;
      for (int i = 0; i < info_len; ++i)
        file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
    }
    file.close();
    info_file_name = file_name + "temp_storage";
    info_file.open(info_file_name, std::ios::in | std::ios::out);
    if (info_file) {
      int tmp;
      info_file.seekg(0, std::ios::beg);
      info_file.read(reinterpret_cast<char *>(&tmp), sizeof(int));
      for (int i = 0; i < tmp; ++i) {
        int tmp2;
        info_file.read(reinterpret_cast<char *>(&tmp2), sizeof(int));
        deleted_index.push_back(tmp2);
      }
      info_file.seekp(0, std::ios::beg);
      tmp = 0;
      info_file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
      info_file.close();
    } else {
      info_file.open(info_file_name, std::ios::out);
      int tmp = 0;
      info_file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
      info_file.close();
    }
  }

  // 读出第n个int的值赋给tmp，1_base
  void get_info(int &tmp, int n) {
    if (n > info_len)
      return;
    file.open(file_name, std::ios::in);
    file.seekg((n - 1) * sizeof(int), std::ios::beg);
    file.read(reinterpret_cast<char *>(&tmp), sizeof(int));
    file.close();
  }

  // 将tmp写入第n个int的位置，1_base
  void write_info(int tmp, int n) {
    if (n > info_len)
      return;
    file.open(file_name, std::ios::in | std::ios::out);
    file.seekp((n - 1) * sizeof(int), std::ios::beg);
    file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
    file.close();
  }

  // 在文件合适位置写入类对象t，并返回写入的位置索引index
  // 位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
  // 位置索引index可以取为对象写入的起始位置
  int write(T &t) { 
    file.open(file_name, std::ios::in | std::ios::out);
    int index = file.tellp();
    if (!deleted_index.empty()) {
      index = deleted_index.back();
      deleted_index.pop_back();
      file.seekp(index, std::ios::beg);
      file.write(reinterpret_cast<char *>(&t), sizeofT);
      file.close();
      return index;
    } else {
      file.seekp(0, std::ios::end);
      index = file.tellp();
      file.write(reinterpret_cast<char *>(&t), sizeofT);
      file.close();
    }
    return index;
   }

  // 用t的值更新位置索引index对应的对象，保证调用的index都是由write函数产生
  void update(T &t, const int index) { 
    file.open(file_name, std::ios::in | std::ios::out);
    file.seekp(index, std::ios::beg);
    file.write(reinterpret_cast<char *>(&t), sizeofT);
    file.close();
    return;
  }

  // 读出位置索引index对应的T对象的值并赋值给t，保证调用的index都是由write函数产生
  void read(T &t, const int index) {
    file.open(file_name, std::ios::in);
    file.seekg(index, std::ios::beg);
    file.read(reinterpret_cast<char *>(&t), sizeofT);
    file.close();
    return;
  }

  // 删除位置索引index对应的对象(不涉及空间回收时，可忽略此函数)，保证调用的index都是由write函数产生
  void Delete(int index) { 
    deleted_index.push_back(index);
    file.open(file_name, std::ios::in | std::ios::out);
    file.seekp(index, std::ios::beg);
    T tmp;
    file.write(reinterpret_cast<char *>(&tmp), sizeofT);
    file.close();
    return;
  }
};

#endif // BPT_MEMORYRIVER_HPP