/*
 * testai.cpp  —  Korviniu pervezimo optimizavimo testiniai duomenys
 *
 * Paleisti:  g++ -std=c++17 -pthread -o testai testai.cpp && ./testai
 *
 * Kiekvienas testas naudoja atskiras gijas (po viena sandeliui).
 * Tikrinamas galutinis kainos rezultatas.
 */
 
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <climits>
#include <cassert>
#include <iomanip>
#include <string>
#include <chrono>
 
// ---------------------------------------------------------------------------
// Bendra algoritmo logika (identiška pagrindinei programai)
// ---------------------------------------------------------------------------
 
struct Allocation {
    int warehouse, destination, quantity, unitCost, totalCost;
};
 
struct TestResult {
    std::vector<Allocation> allocations;
    int grandTotal = 0;
    std::mutex mtx;
};
 
void runWorker(int wi, int supply_i,
               const std::vector<std::vector<int>>& cost,
               std::vector<int>& demandLeft,
               std::mutex& demandMtx,
               TestResult& out)
{
    int remaining = supply_i;
    std::vector<Allocation> mine;
 
    while (remaining > 0) {
        int bestDest = -1, bestCost = INT_MAX;
        for (int j = 0; j < (int)demandLeft.size(); j++) {
            demandMtx.lock();
            int avail = demandLeft[j];
            demandMtx.unlock();
            if (avail > 0 && cost[wi][j] < bestCost) {
                bestCost = cost[wi][j];
                bestDest = j;
            }
        }
        if (bestDest == -1) break;
 
        demandMtx.lock();
        int qty = std::min(remaining, demandLeft[bestDest]);
        demandLeft[bestDest] -= qty;
        demandMtx.unlock();
 
        remaining -= qty;
        mine.push_back({wi, bestDest, qty, bestCost, qty * bestCost});
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
 
    std::lock_guard<std::mutex> lk(out.mtx);
    for (auto& a : mine) {
        out.allocations.push_back(a);
        out.grandTotal += a.totalCost;
    }
}
 
int solve(const std::vector<std::vector<int>>& cost,
          const std::vector<int>& supply,
          const std::vector<int>& demand)
{
    int W = supply.size();
    std::vector<int> demandLeft = demand;
    std::mutex demandMtx;
    TestResult out;
 
    std::vector<std::thread> threads;
    for (int i = 0; i < W; i++)
        threads.emplace_back(runWorker, i, supply[i],
                             std::cref(cost), std::ref(demandLeft),
                             std::ref(demandMtx), std::ref(out));
    for (auto& t : threads) t.join();
    return out.grandTotal;
}
 
// ---------------------------------------------------------------------------
// Testų paleidimas
// ---------------------------------------------------------------------------
 
struct Test {
    std::string name;
    std::vector<std::vector<int>> cost;
    std::vector<int> supply;
    std::vector<int> demand;
    int expectedMax;   // Priimtina virsutine riba (gridy algoritmas nera optimalus)
};
 
void runAll(const std::vector<Test>& tests) {
    int passed = 0, failed = 0;
 
    for (auto& t : tests) {
        int result = solve(t.cost, t.supply, t.demand);
        bool ok = (result > 0 && result <= t.expectedMax);
 
        std::cout << (ok ? "[PASS] " : "[FAIL] ")
                  << std::left << std::setw(40) << t.name
                  << "Kaina: " << std::setw(8) << result
                  << " (riba <= " << t.expectedMax << ")\n";
 
        ok ? passed++ : failed++;
    }
 
    std::cout << "\n----------------------------------------\n";
    std::cout << "Rezultatas: " << passed << " pavyko / "
              << failed << " nepavyko\n";
}
 
int main() {
    std::cout << "============================================\n";
    std::cout << "  Testiniai duomenys — korviniu programa\n";
    std::cout << "============================================\n\n";
 
    std::vector<Test> tests = {
 
        // ------------------------------------------------------------------
        // T01: Bazinis scenarijus (is uzduoties)
        // ------------------------------------------------------------------
        {
            "T01: Bazinis (3x3, is uzduoties)",
            {{4, 8, 1},
             {7, 2, 3},
             {3, 6, 9}},
            {120, 80, 100},
            {150, 100, 50},
            1200   // Greedy riba
        },
 
        // ------------------------------------------------------------------
        // T02: Lygus tiekimas ir poreikis (balansas)
        // ------------------------------------------------------------------
        {
            "T02: Balansas (3x3, lygus kiekiai)",
            {{2, 5, 8},
             {6, 1, 4},
             {9, 3, 7}},
            {100, 100, 100},
            {100, 100, 100},
            1500
        },
 
        // ------------------------------------------------------------------
        // T03: Vienas sandelis — vienas taskas (trivialus)
        // ------------------------------------------------------------------
        {
            "T03: 1x1 trivialus",
            {{5}},
            {200},
            {200},
            1000
        },
 
        // ------------------------------------------------------------------
        // T04: Du sandeliai, du taskai (2x2)
        // ------------------------------------------------------------------
        {
            "T04: 2x2 mazas",
            {{3, 7},
             {8, 2}},
            {60, 90},
            {80, 70},
            700
        },
 
        // ------------------------------------------------------------------
        // T05: Didesnis (4x4), vienodos kainos
        // ------------------------------------------------------------------
        {
            "T05: 4x4 vienodos kainos",
            {{5, 5, 5, 5},
             {5, 5, 5, 5},
             {5, 5, 5, 5},
             {5, 5, 5, 5}},
            {50, 50, 50, 50},
            {50, 50, 50, 50},
            1100
        },
 
        // ------------------------------------------------------------------
        // T06: Labai mazos kainos (ribinis atvejis)
        // ------------------------------------------------------------------
        {
            "T06: Kainos = 1 (minimalios)",
            {{1, 1, 1},
             {1, 1, 1},
             {1, 1, 1}},
            {100, 100, 100},
            {100, 100, 100},
            310
        },
 
        // ------------------------------------------------------------------
        // T07: Didelis tiekimas, mazas poreikis
        // ------------------------------------------------------------------
        {
            "T07: Perteklius (tiekimas > poreikis)",
            {{2, 9, 4},
             {8, 1, 6},
             {5, 3, 7}},
            {500, 500, 500},
            {200, 300, 100},
            2500
        },
 
        // ------------------------------------------------------------------
        // T08: Realus scenarijus — Vilnius, Kaunas, Klaipeda
        // ------------------------------------------------------------------
        {
            "T08: LT miestai (Vilnius/Kaunas/Klaipeda)",
            {{12, 45, 70},   // Sandelis Vilniuje
             {40, 10, 55},   // Sandelis Kaune
             {68, 52,  8}},  // Sandelis Klaipedoje
            {300, 250, 200},
            {280, 270, 200},
            32000
        },
 
        // ------------------------------------------------------------------
        // T09: Asimetrinis (daugiau sandeliu nei tasku)
        // ------------------------------------------------------------------
        {
            "T09: 4 sandeliai, 2 taskai",
            {{3, 9},
             {7, 2},
             {5, 6},
             {1, 8}},
            {80, 60, 70, 90},
            {150, 150},
            2200
        },
 
        // ------------------------------------------------------------------
        // T10: Didelio masto (5x5)
        // ------------------------------------------------------------------
        {
            "T10: 5x5 didelis scenarijus",
            {{ 4, 12,  7, 18,  3},
             {11,  2, 15,  6,  9},
             { 8, 17,  1, 13,  5},
             {14,  6, 10,  2, 16},
             { 3, 19,  8, 11,  4}},
            {200, 180, 220, 160, 240},
            {250, 190, 210, 200, 150},
            8000
        }
    };
 
    runAll(tests);
 
    std::cout << "\nPastaba: Greedy algoritmas garantuoja sprendima,\n"
              << "bet ne visada globaliai optimalu. Tikrinami:\n"
              << "  - Kaina > 0 (krovinis paskirtas)\n"
              << "  - Kaina <= riba (algoritmas veikia pagrįstai)\n";
 
    return 0;
}