#include <random>
#include <algorithm>
#include <iostream>

using Type = float;

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

int main(int argc, char** argv)
{
    if (argc < 2) return 1;

    auto data = create_data(2 * 1024 * 1024);

    if (std::stoi(argv[1]) == 1)
    {
        std::sort(data.begin(), data.end());
    }

    Type sum = 0;
    for (int i = 0; i < 200; i++)
    {
        for (auto x : data)
        {
            if (x < 6)
            {
                sum += x;
            }
        }
    }

    std::cout << sum << std::endl;

    return 0;
}
