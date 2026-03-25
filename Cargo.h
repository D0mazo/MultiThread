#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <climits>
#include <iomanip>
#include <string>
#include <chrono>

// ---------------------------------------------------------------------------
// Problem data
// ---------------------------------------------------------------------------

const int WAREHOUSES = 3;
const int DESTINATIONS = 3;

// cost[i][j] = transport cost per ton from warehouse i to destination j
const int cost[WAREHOUSES][DESTINATIONS] = {
    {4, 8, 1},   // Warehouse 1
    {7, 2, 3},   // Warehouse 2
    {3, 6, 9}    // Warehouse 3
};

// Supply at each warehouse (tons)
const int supply[WAREHOUSES] = {120, 80, 100};

// Demand at each destination (tons)
int demand[DESTINATIONS] = {150, 100, 50};

const std::string warehouseNames[WAREHOUSES] = {"Sandelis 1", "Sandelis 2", "Sandelis 3"};
const std::string destNames[DESTINATIONS]    = {"Taskas A",   "Taskas B",   "Taskas C"};

// ---------------------------------------------------------------------------
// Shared result storage (protected by mutex)
// ---------------------------------------------------------------------------

struct Allocation {
    int warehouse;
    int destination;
    int quantity;
    int unitCost;
    int totalCost;
};

std::vector<Allocation> results;
int grandTotal = 0;
std::mutex resultMutex;
std::mutex demandMutex;

// Working demand copy (modified by threads)
int demandLeft[DESTINATIONS];

// ---------------------------------------------------------------------------
// Thread worker: greedy cheapest-destination strategy for warehouse i
// ---------------------------------------------------------------------------

void warehouseThread(int i) {
    auto tid = std::this_thread::get_id();

    std::cout << "\n[Gija " << i + 1 << " | ID " << tid << "] "
              << warehouseNames[i] << " pradeda skaiciuoti...\n";

    int remaining = supply[i];
    std::vector<Allocation> myAllocs;

    // Greedy: pick cheapest available destination repeatedly
    while (remaining > 0) {
        int bestDest = -1;
        int bestCost = INT_MAX;

        // Find cheapest destination that still has demand
        for (int j = 0; j < DESTINATIONS; j++) {
            demandMutex.lock();
            int avail = demandLeft[j];
            demandMutex.unlock();

            if (avail > 0 && cost[i][j] < bestCost) {
                bestCost = cost[i][j];
                bestDest = j;
            }
        }

        if (bestDest == -1) break; // No destinations left

        // Decide how much to send
        demandMutex.lock();
        int qty = std::min(remaining, demandLeft[bestDest]);
        demandLeft[bestDest] -= qty;
        demandMutex.unlock();

        remaining -= qty;

        Allocation a;
        a.warehouse  = i;
        a.destination = bestDest;
        a.quantity   = qty;
        a.totalCost  = qty * bestCost;
        a.unitCost   = bestCost;
        myAllocs.push_back(a);

        std::cout << "  [Gija " << i + 1 << "] " << warehouseNames[i]
                  << " -> " << destNames[bestDest]
                  << " : " << qty << " t x " << bestCost
                  << " EUR/t = " << (qty * bestCost) << " EUR\n";

        // Small delay to make threading visible
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    // Write results with lock
    {
        std::lock_guard<std::mutex> lock(resultMutex);
        for (auto& a : myAllocs) {
            results.push_back(a);
            grandTotal += a.totalCost;
        }
    }

    std::cout << "[Gija " << i + 1 << "] " << warehouseNames[i]
              << " baige. Likutis: " << remaining << " t\n";
}

// ---------------------------------------------------------------------------
// Print helpers
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

void printResults() {
    std::cout << "\n=== GALUTINIS PASKIRSTYMAS ===\n";
    std::cout << std::left
              << std::setw(14) << "Is"
              << std::setw(12) << "I"
              << std::setw(10) << "Kiekis"
              << std::setw(10) << "Kaina"
              << std::setw(12) << "Suma\n";
    std::cout << std::string(58, '-') << "\n";

    for (auto& a : results) {
        std::cout << std::left
                  << std::setw(14) << warehouseNames[a.warehouse]
                  << std::setw(12) << destNames[a.destination]
                  << std::setw(10) << (std::to_string(a.quantity) + " t")
                  << std::setw(10) << (std::to_string(a.unitCost) + " EUR/t")
                  << std::setw(12) << (std::to_string(a.totalCost) + " EUR")
                  << "\n";
    }

    std::cout << std::string(58, '-') << "\n";
    std::cout << std::right << std::setw(46) << "BENDRA KAINA: "
              << grandTotal << " EUR\n";
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main() {
    std::cout << "============================================\n";
    std::cout << "  Korviniu Pervezimo Optimizavimas\n";
    std::cout << "  Daugiagileja programa (C++ threads)\n";
    std::cout << "============================================\n";

    printMatrix();

    // Initialise working demand array
    for (int j = 0; j < DESTINATIONS; j++)
        demandLeft[j] = demand[j];

    std::cout << "\n--- Paleidziamos " << WAREHOUSES << " gijos ---\n";

    // Launch one thread per warehouse
    std::vector<std::thread> threads;
    for (int i = 0; i < WAREHOUSES; i++)
        threads.emplace_back(warehouseThread, i);

    // Wait for all threads
    for (auto& t : threads)
        t.join();

    std::cout << "\n--- Visos gijos baige darka ---\n";

    printResults();

    return 0;
}