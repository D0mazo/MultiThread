#include "Cargo.h"
#include <thread>

#include "Cargo.h"
#include <vector>
#include <thread>
#include <cmath> // for pow

// Single-threaded: heavier realistic calculation
long long calculateSingle(const std::vector<Cargo>& data)
{
    long long total = 0;

    for (const auto& c : data)
    {
        double factor = 1.0 + 0.01 * c.weight + 0.005 * c.distance;
        double overhead = std::pow(c.weight % 10 + 1, 2) + std::pow(c.distance % 5 + 1, 3);
        long long price = static_cast<long long>((c.weight * c.distance * factor) + overhead);

        total += price;
    }

    return total;
}

// Multi-threaded: same formula, split across threads
long long calculateMulti(const std::vector<Cargo>& data, int num_threads)
{
    if (data.empty()) return 0;
    if (num_threads < 1) num_threads = 1;

    const int size = static_cast<int>(data.size());
    if (num_threads > size) num_threads = size;

    std::vector<std::thread> threads;
    std::vector<long long> partial_sums(num_threads, 0);
    const int chunk = (size + num_threads - 1) / num_threads;

    for (int i = 0; i < num_threads; ++i)
    {
        int start = i * chunk;
        int end   = std::min(start + chunk, size);
        if (start >= end) break;

        threads.emplace_back([start, end, i, &data, &partial_sums]() {
            long long sum = 0;
            for (int j = start; j < end; ++j)
            {
                double factor = 1.0 + 0.01 * data[j].weight + 0.005 * data[j].distance;
                double overhead = std::pow(data[j].weight % 10 + 1, 2) + std::pow(data[j].distance % 5 + 1, 3);
                sum += static_cast<long long>((data[j].weight * data[j].distance * factor) + overhead);
            }
            partial_sums[i] = sum;
        });
    }

    for (auto& th : threads) th.join();

    long long total = 0;
    for (auto s : partial_sums) total += s;
    return total;
}