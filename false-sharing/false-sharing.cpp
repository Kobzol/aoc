#include <iostream>
#include <chrono>
#include <thread>
#include <cassert>
#include <vector>
#include <atomic>
#include <cstring>

#define REPEAT 1024 * 1024 * 50UL

using Type = size_t;

void thread_fn(int tid, int increment, size_t repeat, Type* memory, int size)
{
    int target = tid * increment;

    for (size_t i = 0; i < repeat; i++)
    {
        std::memset(memory + target, i, size); // force memory access
    }
}

int main(int argc, char** argv)
{
    int threadCount = std::stoi(argv[1]);
    int increment = std::stoi(argv[2]);
    size_t* memory;
    if (posix_memalign((void**)&memory, 64, 100000 * sizeof(size_t)))
    {
        std::cerr << "Couldn't allocate memory" << std::endl;
        exit(1);
    }
    assert(((uintptr_t)memory & 0x3F) == 0);

    using Clock = std::chrono::system_clock;
    auto start = Clock::now();

    for (int t = 0; t < 2; t++)
    {
        std::vector<std::thread> threads;
        for (int i = 0; i < threadCount; i++)
        {
            threads.emplace_back(thread_fn, i + 1, increment, REPEAT, memory, sizeof(Type));
        }

        for (auto& thread : threads)
        {
            thread.join();
        }
        std::cout << t << std::endl;
    }

    std::cout << memory[0] << std::endl;
    std::cerr << std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - start).count() << std::endl;

    free(memory);

    return 0;
}
