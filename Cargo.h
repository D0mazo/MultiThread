#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <chrono>

using Clock = std::chrono::steady_clock;
using Ms    = std::chrono::duration<double, std::milli>;

// ---------------------------------------------------------------------------
// Maksimalus dydis (dinaminiai dydžiai saugomi žemiau)
// ---------------------------------------------------------------------------

const int MAX_WH   = 4;
const int MAX_DEST = 4;

// Aktyvus dydis (keičiamas per InputData)
extern int WAREHOUSES;
extern int DESTINATIONS;

// Duomenys (nebe const — vartotojas gali keisti)
extern int         cost[MAX_WH][MAX_DEST];
extern int         supply[MAX_WH];
extern int         demand[MAX_DEST];
extern std::string warehouseNames[MAX_WH];
extern std::string destNames[MAX_DEST];

// ---------------------------------------------------------------------------
// Duomenų struktūra
// ---------------------------------------------------------------------------

struct Allocation {
    int warehouse;
    int destination;
    int quantity;
    int unitCost;
    int totalCost;
};

// ---------------------------------------------------------------------------
// Bendrai naudojami kintamieji (apsaugoti mutex)
// ---------------------------------------------------------------------------

extern std::vector<Allocation> results;
extern int    grandTotal;
extern std::mutex resultMutex;
extern std::mutex demandMutex;
extern std::mutex coutMutex;
extern int    demandLeft[MAX_DEST];
extern double threadTime[MAX_WH];

// ---------------------------------------------------------------------------
// Funkcijų deklaracijos
// ---------------------------------------------------------------------------

void resetState();
void warehouseThread(int i);
void printMatrix();
void printResults();
void printTimings(double totalMs);
void runSimulation();