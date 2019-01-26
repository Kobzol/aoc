#include <random>
#include <algorithm>
#include <iostream>
#include <Routing/Algorithms/Common/State.h>
#include <Routing/Algorithms/TravelCostCalculators/CostCalculator.h>
#include <Routing/Algorithms/OneToOne/Dijkstra/Dijkstra.h>
#include <Routing/Tests/IOUtils.h>

#define REPETITIONS 10

int main()
{
    auto g = Routing::Tests::LoadGraph("/home/kobzol/projects/it4i/antarex-routing/data/CZE-1528295206-proc-20180724134457.hdf");
    int start_node = 202398867;
    int end_node = 202131068;

    using Clock = std::chrono::system_clock;
    auto start = Clock::now();

    Routing::Algorithms::Dijkstra algorithm(g);
    for (int i = 0; i < REPETITIONS; i++)
    {
        Routing::Algorithms::State s(start_node, 0);
        auto result = algorithm.GetResult(s, end_node, Routing::CostCalcType::Len_cost);
    }

    std::cerr << std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - start).count() / REPETITIONS << std::endl;

    return 0;
}
