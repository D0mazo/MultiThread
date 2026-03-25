#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <chrono>

using Clock = std::chrono::steady_clock;
using Ms    = std::chrono::duration<double, std::milli>;

// ---------------------------------------------------------------------------
// Konstantos
// ---------------------------------------------------------------------------

const int WAREHOUSES   = 3;
const int DESTINATIONS = 3;

extern const int cost[WAREHOUSES][DESTINATIONS];
extern const int supply[WAREHOUSES];
extern int demand[DESTINATIONS];

extern const std::string warehouseNames[WAREHOUSES];
extern const std::string destNames[DESTINATIONS];

// ---------------------------------------------------------------------------
// Duomenu strukturos
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
extern int grandTotal;
extern std::mutex resultMutex;
extern std::mutex demandMutex;
extern std::mutex coutMutex;
extern int demandLeft[DESTINATIONS];
extern double threadTime[WAREHOUSES];

// ---------------------------------------------------------------------------
// Funkciju deklaracijos
// ---------------------------------------------------------------------------

void resetState();
void warehouseThread(int i);
void printMatrix();
void printResults();
void printTimings(double totalMs);
void runSimulation();