#include "Cargo.h"
#include <vector>
#include <thread>
#include <cmath>


static inline long long computePrice(int weight, int distance)
{

    double factor  = 1.0 + 0.01  * weight + 0.005  * distance;              // pagrindinis svorio ir atstumo koeficientas
    double factor2 = 1.0 + 0.003 * weight   * std::sin(weight   * 0.01);    // svorio svyravimo priemoka
    double factor3 = 1.0 + 0.002 * distance * std::cos(distance * 0.007);   // atstumo zonos efektyvumas
    double factor4 = 1.0 + std::log(weight + distance + 1) * 0.004;         // bendras logaritminis masto priedas
    double factor5 = 1.0 + std::sqrt((double)weight * distance) * 0.0001;   // geometrinio vidurkio priemoka
    double factor6 = 1.0 + std::pow((weight % 7 + 1) * (distance % 11 + 1) * 0.0001, 1.5); // modulinis kainų lygis

    double combinedFactor = factor * factor2 * factor3 * factor4 * factor5 * factor6;


    double overhead = std::pow(weight % 10 + 1, 2) + std::pow(distance % 5 + 1, 3);
    double penalty  = std::sin(weight * 0.001) * std::cos(distance * 0.001);
    double logBonus = std::log(weight + 1) * std::sqrt(distance + 1);

    return static_cast<long long>(
        (weight * distance * combinedFactor) + overhead + penalty * 100.0 + logBonus
    );
}


long long calculateSingle(const std::vector<Cargo>& data)
{
    long long total = 0;
    for (const auto& c : data)
        total += computePrice(c.weight, c.distance);
    return total;
}


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
                sum += computePrice(data[j].weight, data[j].distance);
            partial_sums[i] = sum;
        });
    }

    for (auto& th : threads) th.join();

    long long total = 0;
    for (auto s : partial_sums) total += s;
    return total;
}