#include "Cargo.h"

#include <iostream>
#include <thread>
#include <iomanip>
#include <climits>

// ---------------------------------------------------------------------------
// Duomenys
// ---------------------------------------------------------------------------

const int cost[WAREHOUSES][DESTINATIONS] = {
    {4, 8, 1},   // Sandelis 1
    {7, 2, 3},   // Sandelis 2
    {3, 6, 9}    // Sandelis 3
};

const int supply[WAREHOUSES] = {120, 80, 100};
int demand[DESTINATIONS]     = {150, 100, 50};

const std::string warehouseNames[WAREHOUSES] = {"Sandelis 1", "Sandelis 2", "Sandelis 3"};
const std::string destNames[DESTINATIONS]    = {"Taskas A",   "Taskas B",   "Taskas C"};

// ---------------------------------------------------------------------------
// Globalus kintamieji
// ---------------------------------------------------------------------------

std::vector<Allocation> results;
int grandTotal = 0;
std::mutex resultMutex;
std::mutex demandMutex;
std::mutex coutMutex;
int demandLeft[DESTINATIONS] = {0};
double threadTime[WAREHOUSES] = {0};

// ---------------------------------------------------------------------------
// Pagalbine funkcija: gija saugi spausdinimo
// ---------------------------------------------------------------------------

template<typename T>
void safePrintImpl(T&& val) {
    std::cout << std::forward<T>(val);
}

template<typename T, typename... Rest>
void safePrintImpl(T&& val, Rest&&... rest) {
    std::cout << std::forward<T>(val);
    safePrintImpl(std::forward<Rest>(rest)...);
}

template<typename... Args>
void safePrint(Args&&... args) {
    std::lock_guard<std::mutex> lk(coutMutex);
    safePrintImpl(std::forward<Args>(args)...);
}

// ---------------------------------------------------------------------------
// Busenos atstatymas (naujai simuliacijai)
// ---------------------------------------------------------------------------

void resetState() {
    results.clear();
    grandTotal = 0;
    for (int j = 0; j < DESTINATIONS; j++)
        demandLeft[j] = demand[j];
    for (int i = 0; i < WAREHOUSES; i++)
        threadTime[i] = 0;
}

// ---------------------------------------------------------------------------
// Gijos darbuotojas
// ---------------------------------------------------------------------------

void warehouseThread(int i) {
    auto t0  = Clock::now();
    auto tid = std::this_thread::get_id();

    safePrint("\n[Gija ", i+1, " | ID ", tid, "] ",
              warehouseNames[i], " pradeda skaiciuoti...\n");

    int remaining = supply[i];
    std::vector<Allocation> myAllocs;

    while (remaining > 0) {
        int bestDest = -1, bestCost = INT_MAX;

        for (int j = 0; j < DESTINATIONS; j++) {
            std::lock_guard<std::mutex> lk(demandMutex);
            if (demandLeft[j] > 0 && cost[i][j] < bestCost) {
                bestCost = cost[i][j];
                bestDest = j;
            }
        }

        if (bestDest == -1) break;

        int qty;
        {
            std::lock_guard<std::mutex> lk(demandMutex);
            qty = std::min(remaining, demandLeft[bestDest]);
            demandLeft[bestDest] -= qty;
        }

        remaining -= qty;

        Allocation a{i, bestDest, qty, bestCost, qty * bestCost};
        myAllocs.push_back(a);

        auto elapsed = Ms(Clock::now() - t0).count();
        safePrint("  [Gija ", i+1, " | +",
                  std::fixed, std::setprecision(1), elapsed, " ms] ",
                  warehouseNames[i], " -> ", destNames[bestDest],
                  " : ", qty, " t x ", bestCost,
                  " EUR/t = ", qty * bestCost, " EUR\n");

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    auto t1 = Clock::now();
    threadTime[i] = Ms(t1 - t0).count();

    {
        std::lock_guard<std::mutex> lk(resultMutex);
        for (auto& a : myAllocs) {
            results.push_back(a);
            grandTotal += a.totalCost;
        }
    }

    safePrint("[Gija ", i+1, "] ", warehouseNames[i],
              " baige. Truko: ",
              std::fixed, std::setprecision(2), threadTime[i],
              " ms | Likutis: ", remaining, " t\n");
}

// ---------------------------------------------------------------------------
// Spausdinimo funkcijos
// ---------------------------------------------------------------------------

void printMatrix() {
    std::cout << "\n=== SANAUDU MATRICA (EUR/t) ===\n";
    std::cout << std::setw(14) << " ";
    for (int j = 0; j < DESTINATIONS; j++)
        std::cout << std::setw(12) << destNames[j];
    std::cout << std::setw(12) << "Tiekimas\n";
    std::cout << std::string(62, '-') << "\n";

    for (int i = 0; i < WAREHOUSES; i++) {
        std::cout << std::setw(14) << warehouseNames[i];
        for (int j = 0; j < DESTINATIONS; j++)
            std::cout << std::setw(12) << cost[i][j];
        std::cout << std::setw(12) << supply[i] << " t\n";
    }

    std::cout << std::setw(14) << "Poreikis";
    for (int j = 0; j < DESTINATIONS; j++)
        std::cout << std::setw(11) << demand[j] << " t";
    std::cout << "\n" << std::string(62, '-') << "\n";
}

void printTimings(double totalMs) {
    std::cout << "\n=== LAIKO STATISTIKA ===\n";
    std::cout << std::string(42, '-') << "\n";
    for (int i = 0; i < WAREHOUSES; i++) {
        std::cout << std::left  << std::setw(18) << warehouseNames[i]
                  << std::right << std::setw(10) << std::fixed
                  << std::setprecision(2) << threadTime[i] << " ms\n";
    }
    std::cout << std::string(42, '-') << "\n";

    double sequential = 0;
    for (int i = 0; i < WAREHOUSES; i++) sequential += threadTime[i];

    std::cout << std::left  << std::setw(18) << "Viso (lygiagretu)"
              << std::right << std::setw(10) << std::fixed
              << std::setprecision(2) << totalMs << " ms\n";
    std::cout << std::left  << std::setw(18) << "Viso (nuosekliai)"
              << std::right << std::setw(10) << std::fixed
              << std::setprecision(2) << sequential << " ms\n";
    std::cout << std::left  << std::setw(18) << "Pagreitis"
              << std::right << std::setw(10) << std::fixed
              << std::setprecision(2) << sequential / totalMs << "x\n";
    std::cout << std::string(42, '-') << "\n";
}

void printResults() {
    std::cout << "\n=== GALUTINIS PASKIRSTYMAS ===\n";
    std::cout << std::left
              << std::setw(14) << "Is"
              << std::setw(12) << "I"
              << std::setw(10) << "Kiekis"
              << std::setw(12) << "Kaina"
              << std::setw(12) << "Suma\n";
    std::cout << std::string(60, '-') << "\n";

    for (auto& a : results) {
        std::cout << std::left
                  << std::setw(14) << warehouseNames[a.warehouse]
                  << std::setw(12) << destNames[a.destination]
                  << std::setw(10) << (std::to_string(a.quantity) + " t")
                  << std::setw(12) << (std::to_string(a.unitCost) + " EUR/t")
                  << std::setw(12) << (std::to_string(a.totalCost) + " EUR")
                  << "\n";
    }

    std::cout << std::string(60, '-') << "\n";
    std::cout << std::right << std::setw(48) << "BENDRA KAINA: "
              << grandTotal << " EUR\n";
}

// ---------------------------------------------------------------------------
// Pagrindinė simuliacijos funkcija
// ---------------------------------------------------------------------------

void runSimulation() {
    resetState();

    std::cout << "\n--- Paleidziamos " << WAREHOUSES << " gijos ---\n";

    auto start = Clock::now();

    std::vector<std::thread> threads;
    for (int i = 0; i < WAREHOUSES; i++)
        threads.emplace_back(warehouseThread, i);

    for (auto& t : threads)
        t.join();

    double totalMs = Ms(Clock::now() - start).count();

    std::cout << "\n--- Visos gijos baige darka ---\n";

    printResults();
    printTimings(totalMs);
}