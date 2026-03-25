#include "Cargo.h"
#include <thread>

long long calculateSingle(const std::vector<Cargo>& data) {
    long long sum = 0;
    for (auto& c : data)
        sum += c.weight * c.distance;
    return sum;
}

void partial(const std::vector<Cargo>& data, int start, int end, long long& result) {
    result = 0;
    for (int i = start; i < end; i++)
        result += data[i].weight * data[i].distance;
}

long long calculateMulti(const std::vector<Cargo>& data, int threads) {
    std::vector<std::thread> t;
    std::vector<long long> results(threads);

    int size = data.size();
    int chunk = size / threads;

    for (int i = 0; i < threads; i++) {
        int start = i * chunk;
        int end = (i == threads - 1) ? size : start + chunk;

        t.emplace_back(partial, std::cref(data), start, end, std::ref(results[i]));
    }

    for (auto& th : t) th.join();

    long long sum = 0;
    for (auto r : results) sum += r;
    return sum;
}