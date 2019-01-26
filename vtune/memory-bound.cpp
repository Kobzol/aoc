#include <random>
#include <algorithm>
#include <iostream>

using Type = int;

std::vector<Type> create_data(size_t size)
{
    std::default_random_engine rng(0);
    std::uniform_int_distribution<int> dist(1, 10);

    std::vector<Type> data(size);
    std::generate(std::begin(data), std::end(data), [&dist, &rng]() {
        return dist(rng);
    });

    return data;
}

int main()
{
    size_t SIZE = 8 * 1024 * 1024;
    auto data = create_data(SIZE);

    for (int i = 0; i < 500; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            data[j] += j;
        }
    }

    std::cout << data[0] + data[1024] << std::endl;

    return 0;
}
