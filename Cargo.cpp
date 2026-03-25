#include "Cargo.h"
#include <thread>

long long calculateSingle(const std::vector<Cargo>& data)
{
    long long sum = 0;
    for (const auto& c : data)
        sum += static_cast<long long>(c.weight) * c.distance;
    return sum;
}

long long calculateMulti(const std::vector<Cargo>& data, int num_threads)
{
    if (data.empty()) return 0;
    if (num_threads < 1) num_threads = 1;

    const int size = static_cast<int>(data.size());
    if (num_threads > size) num_threads = size;   // apsauga

    std::vector<std::thread> threads;
    std::vector<long long> partial_sums(num_threads, 0);

    const int chunk = (size + num_threads - 1) / num_threads;   // teisingas dalinimas

    for (int i = 0; i < num_threads; ++i)
    {
        int start = i * chunk;
        int end   = std::min(start + chunk, size);

        if (start >= end) break;

        threads.emplace_back([&, start, end, i]() {
            long long sum = 0;
            for (int j = start; j < end; ++j) {
                sum += static_cast<long long>(data[j].weight) * data[j].distance;
            }
            partial_sums[i] = sum;
        });
    }

    for (auto& th : threads) th.join();

    long long total = 0;
    for (auto s : partial_sums) total += s;
    return total;
}