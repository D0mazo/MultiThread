#include "Testai.h"
#include "Cargo.h"

#include <iostream>
#include <iomanip>
#include <thread>
#include <vector>
#include <climits>

// ---------------------------------------------------------------------------
// Vidinė pagalbinė struktūra testams
// ---------------------------------------------------------------------------

struct TestCase {
    std::string name;
    int         costMatrix[4][4];   // max 4x4
    int         supplyArr[4];
    int         demandArr[4];
    int         W, D;               // faktinis dydis
    int         expectedMax;
};

// ---------------------------------------------------------------------------
// Vieno testo paleidimas su laikmačiu
// ---------------------------------------------------------------------------

static int runTest(const TestCase& tc) {
    // --- vietiniai kintamieji (nekepame globaliu) ---
    int localDemand[4];
    for (int j = 0; j < tc.D; j++) localDemand[j] = tc.demandArr[j];

    int localGrandTotal = 0;
    std::mutex localResultMtx, localDemandMtx, localCoutMtx;
    int localDemandLeft[4];
    for (int j = 0; j < tc.D; j++) localDemandLeft[j] = localDemand[j];

    struct LocalAlloc { int w, d, qty, unitCost, total; };
    std::vector<LocalAlloc> localResults;

    auto worker = [&](int i) {
        int remaining = tc.supplyArr[i];
        std::vector<LocalAlloc> mine;

        while (remaining > 0) {
            int bestDest = -1, bestCost = INT_MAX;
            for (int j = 0; j < tc.D; j++) {
                std::lock_guard<std::mutex> lk(localDemandMtx);
                if (localDemandLeft[j] > 0 && tc.costMatrix[i][j] < bestCost) {
                    bestCost = tc.costMatrix[i][j];
                    bestDest = j;
                }
            }
            if (bestDest == -1) break;

            int qty;
            {
                std::lock_guard<std::mutex> lk(localDemandMtx);
                qty = std::min(remaining, localDemandLeft[bestDest]);
                localDemandLeft[bestDest] -= qty;
            }
            remaining -= qty;
            mine.push_back({i, bestDest, qty, bestCost, qty * bestCost});
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        std::lock_guard<std::mutex> lk(localResultMtx);
        for (auto& a : mine) {
            localResults.push_back(a);
            localGrandTotal += a.total;
        }
    };

    auto t0 = Clock::now();
    std::vector<std::thread> threads;
    for (int i = 0; i < tc.W; i++)
        threads.emplace_back(worker, i);
    for (auto& t : threads) t.join();
    double ms = Ms(Clock::now() - t0).count();

    bool ok = (localGrandTotal > 0 && localGrandTotal <= tc.expectedMax);

    std::cout << (ok ? "[PASS] " : "[FAIL] ")
              << std::left << std::setw(42) << tc.name
              << "Kaina: " << std::setw(8) << localGrandTotal
              << " riba <= " << std::setw(6) << tc.expectedMax
              << std::fixed << std::setprecision(1)
              << " (" << ms << " ms)\n";

    return ok ? 1 : 0;
}

// ---------------------------------------------------------------------------
// Visi testai
// ---------------------------------------------------------------------------

void runAllTests() {
    std::cout << "============================================\n";
    std::cout << "  Testai — korviniu programa\n";
    std::cout << "============================================\n\n";

    std::vector<TestCase> tests = {

        // T01: bazinis scenarijus (is uzduoties)
        {
            "T01: Bazinis 3x3 (is uzduoties)",
            {{4,8,1,0},{7,2,3,0},{3,6,9,0},{0,0,0,0}},
            {120, 80, 100, 0},
            {150, 100, 50, 0},
            3, 3, 1200
        },

        // T02: balansas - lygus kiekiai
        {
            "T02: Balansas 3x3",
            {{2,5,8,0},{6,1,4,0},{9,3,7,0},{0,0,0,0}},
            {100, 100, 100, 0},
            {100, 100, 100, 0},
            3, 3, 1500
        },

        // T03: trivialus 1x1
        {
            "T03: Trivialus 1x1",
            {{5,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
            {200, 0, 0, 0},
            {200, 0, 0, 0},
            1, 1, 1000
        },

        // T04: mazas 2x2
        {
            "T04: Mazas 2x2",
            {{3,7,0,0},{8,2,0,0},{0,0,0,0},{0,0,0,0}},
            {60, 90, 0, 0},
            {80, 70, 0, 0},
            2, 2, 700
        },

        // T05: vienodos kainos
        {
            "T05: Vienodos kainos 3x3",
            {{5,5,5,0},{5,5,5,0},{5,5,5,0},{0,0,0,0}},
            {50, 50, 50, 0},
            {50, 50, 50, 0},
            3, 3, 1100
        },

        // T06: minimalios kainos (visos = 1)
        {
            "T06: Minimalios kainos (visos = 1)",
            {{1,1,1,0},{1,1,1,0},{1,1,1,0},{0,0,0,0}},
            {100, 100, 100, 0},
            {100, 100, 100, 0},
            3, 3, 310
        },

        // T07: perteklius (tiekimas > poreikis)
        {
            "T07: Perteklius (tiekimas > poreikis)",
            {{2,9,4,0},{8,1,6,0},{5,3,7,0},{0,0,0,0}},
            {500, 500, 500, 0},
            {200, 300, 100, 0},
            3, 3, 2500
        },

        // T08: LT miestai
        {
            "T08: LT miestai (Vilnius/Kaunas/Klaipeda)",
            {{12,45,70,0},{40,10,55,0},{68,52,8,0},{0,0,0,0}},
            {300, 250, 200, 0},
            {280, 270, 200, 0},
            3, 3, 32000
        },

        // T09: 4 sandeliai, 2 taskai
        {
            "T09: 4 sandeliai, 2 taskai",
            {{3,9,0,0},{7,2,0,0},{5,6,0,0},{1,8,0,0}},
            {80, 60, 70, 90},
            {150, 150, 0, 0},
            4, 2, 2200
        },

        // T10: 4x4 didelis scenarijus
        {
            "T10: 4x4 didelis scenarijus",
            {{4,12,7,18},{11,2,15,6},{8,17,1,13},{14,6,10,2}},
            {200, 180, 220, 160},
            {250, 190, 210, 110},
            4, 4, 6000
        }
    };

    int passed = 0;
    auto t0 = Clock::now();

    for (auto& tc : tests)
        passed += runTest(tc);

    double totalMs = Ms(Clock::now() - t0).count();

    std::cout << "\n" << std::string(60, '-') << "\n";
    std::cout << "Rezultatas: " << passed << " / " << (int)tests.size()
              << " testai pavyko   |   Viso laiko: "
              << std::fixed << std::setprecision(1) << totalMs << " ms\n";
    std::cout << std::string(60, '-') << "\n";
}