#include "acm1336.hpp"
#include <iostream>
#include <string>
#include <fstream>

int main() {
    MemoryRiver<int> mr("test");
    mr.initialise();
    mr.write_info(1, 1);
    mr.write_info(2, 2);
    int a = 100;
    int place_1 = mr.write(a);
    int num;
    mr.read(num, place_1);
    std::cout << num << std::endl;
    a = 1000;
    mr.update(a, place_1);
    mr.read(num, place_1);
    std::cout << num << std::endl;
    mr.get_info(num, 2);
    std::cout << num;
    return 0;
}