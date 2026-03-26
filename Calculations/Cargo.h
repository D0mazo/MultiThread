#pragma once
#include <vector>

struct Cargo {
    int weight;
    int distance;
};

long long calculateSingle(const std::vector<Cargo>& data);
long long calculateMulti(const std::vector<Cargo>& data, int threads);