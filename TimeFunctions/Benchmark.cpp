#pragma once
#include <vector>
#include "Benchmark.h"
#include "../Calculations/Cargo.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <thread>
#include <algorithm>

void runBenchmark(const std::vector<Cargo>& data)
{
    // Warm-up: paleidžiame kartą prieš matavimus, kad cache ir branch predictor sušiltų
    std::cout << "\n[~] Apsilimas (warm-up)...\n";
    volatile long long warmup = calculateSingle(data);
    calculateMulti(data, std::thread::hardware_concurrency());
    (void)warmup;

    // Viengijis - laiko matavimas
    auto t1 = std::chrono::high_resolution_clock::now();
    long long singleResult = calculateSingle(data);
    auto t2 = std::chrono::high_resolution_clock::now();
    double time_single = std::chrono::duration<double, std::milli>(t2 - t1).count();

    // --- Automatinis daugiagijis testavimas: 2, 4, 8, 16 gijų namų kompiuteris---
    int threadCounts[] = {2, 4, 8, 56};
    // --- Automatinis daugiagijis testavimas: 2, 4, 8, 12 nešiojimas---
    //int threadCounts[] = {2, 4, 6, 12};

    // Dinamiškai parenkame gijų skaičius pagal procesoriaus branduolius
    //int maxThreads = std::thread::hardware_concurrency();
    //int threadCounts[] = {
    //    std::max(1, maxThreads / 6),
    //    std::max(1, maxThreads / 3),
    //    std::max(1, maxThreads / 2),
    //    maxThreads
    //};

    double times[4];
    long long results[4];

    for (int i = 0; i < 4; i++)
    { //daugiagijinis laiko skaiviamas
        auto ts = std::chrono::high_resolution_clock::now();
        results[i] = calculateMulti(data, threadCounts[i]);
        auto te = std::chrono::high_resolution_clock::now();
        times[i] = std::chrono::duration<double, std::milli>(te - ts).count();
    }

    // Rezultatų lentelė
    std::cout << "\n========== REZULTATAI ==========\n";
    std::cout << "\n[Viengijis]\n";
    std::cout << "  Bendra kaina : " << singleResult << "\n";
    std::cout << "  Laikas       : " << time_single << " ms\n";
    std::cout << "  Vienas branduolys, nuoseklus skaiciavimas\n";

    std::cout << "\n[Daugiagijis palyginimas]\n";
    std::cout << "  Gijos  | Laikas (ms)  | Pagreitis | Bendra kaina\n";
    std::cout << "  -------|--------------|-----------|-------------\n";

    for (int i = 0; i < 4; i++)
    {
        double speedup = (times[i] > 0.0) ? (time_single / times[i]) : 0.0;
        std::cout << "  " << std::setw(5)  << threadCounts[i]
                  << "  | " << std::setw(12) << std::fixed << std::setprecision(2) << times[i]
                  << "  | " << std::setw(9)  << std::fixed << std::setprecision(3) << speedup << "x"
                  << "  | " << std::setw(13) << results[i] << "\n";
    }

    // Geriausio varianto nustatymas
    int bestIdx = 0;
    for (int i = 1; i < 4; i++)
        if (times[i] < times[bestIdx]) bestIdx = i;

    double bestSpeedup = (times[bestIdx] > 0.0) ? (time_single / times[bestIdx]) : 0.0;

    std::cout << "\n[Isvada]\n";
    std::cout << "  >> Geriausias variantas: " << threadCounts[bestIdx] << " gijos ("
              << bestSpeedup << "x pagreitis).\n";

    // Rezultatų teisingumas
    bool allMatch = true;
    for (int i = 0; i < 4; i++)
        if (results[i] != singleResult) allMatch = false;

    if (allMatch)
        std::cout << "  [OK] Visi metodai grazina vienoda suma - skaiciavimai teisingi.\n";
    else
        std::cout << "  [!] ISPEJIMAS: Rezultatai nesutampa.\n";
}