#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <zconf.h>

int main()
{
    int *datas = new int[10];
    datas[10] = 1;

    std::vector<int> data(1024 * 1024, 0);

    std::cout << getpid() << std::endl;

    std::string line;
    std::cin >> line;
}
