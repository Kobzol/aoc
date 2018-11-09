#include <iostream>
#include <sys/mman.h>
#include <vector>
#include <chrono>

int* MEMORY = nullptr;

using Clock = std::chrono::steady_clock;

#define REPETITIONS 100000000

void test_memory(const std::vector<int*>& memory)
{
    size_t sum = 0;
    size_t size = memory.size();

    auto start = Clock::now();

    for (int i = 0; i < REPETITIONS; i++)
    {
        for (int j = 0; j < size; j++)
        {
            *memory[j] += j;
        }
    }

    std::cerr << std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - start).count() << std::endl;
    std::cout << "Sum: " << sum << std::endl;
}

int main(int argc, char** argv)
{
    MEMORY = static_cast<int*>(::mmap(nullptr, 10 * 1024 * 1024 * 1024UL,
                PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0));
    if (MEMORY == (void*) -1)
    {
        perror("mmap");
        exit(1);
    }

    auto startAddress = reinterpret_cast<size_t>(MEMORY);

    int assoc = 8;
    int size = 32768;
    int slots = size / 64;
    int buckets = slots / assoc;
    size_t count = std::stoi(argv[1]);

    if (std::string(argv[2]) == "0")
    {
        std::vector<int*> memory;
        size_t increment = std::stoi(argv[3]);

        for (int i = 0; i < count; i++)
        {
            memory.push_back((int*)(startAddress + i * increment));
        }
//        for (int i = 0; i < count; i++)
//        {
//            std::cerr << std::hex << memory[i] << std::endl;
//        }

        test_memory(memory);
    }
    else
    {
        std::vector<int*> memory2;
        for (int i = 0; i < count; i++)
        {
            memory2.push_back(new int);
        }
        test_memory(memory2);
    }

    return 0;
}
