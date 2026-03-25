#include "Menu.h"
#include "Cargo.h"
#include "InputData.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <algorithm>

// ---------------------------------------------------------------------------
// Spalvos
// ---------------------------------------------------------------------------

#ifdef _WIN32
#include <windows.h>
static void enableColor() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(h, &mode);
    SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}
#else
static void enableColor() {}
#endif

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define CYAN    "\033[36m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"
#define MAGENTA "\033[35m"

// ---------------------------------------------------------------------------
// Istorija
// ---------------------------------------------------------------------------

struct HistoryEntry {
    int    threadCount;
    int    totalCost;
    double timeMs;
    double speedup;
};

static std::vector<HistoryEntry> history;

// ---------------------------------------------------------------------------
// Pagalbinės funkcijos
// ---------------------------------------------------------------------------

static void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

static void printSep(char c = '-', int n = 62) {
    std::cout << std::string(n, c) << "\n";
}

static void printHeader() {
    printSep('=');
    std::cout << BOLD << CYAN
              << "   KORVINIU PERVEZIMO OPTIMIZAVIMAS\n"
              << "   Daugiagileja sistema  |  C++ threads\n"
              << RESET;
    printSep('=');
}

static int readInt(const std::string& prompt, int lo, int hi) {
    int v;
    while (true) {
        std::cout << YELLOW << prompt << RESET;
        if (std::cin >> v && v >= lo && v <= hi) {
            std::cin.ignore(1000, '\n');
            return v;
        }
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << RED << "  Klaida: iveskite skaiciu nuo "
                  << lo << " iki " << hi << "\n" << RESET;
    }
}

// ---------------------------------------------------------------------------
// Simuliacija su pasirinktu gijų skaičiumi
// ---------------------------------------------------------------------------

static void runWithThreadCount(int threadCount) {
    results.clear();
    grandTotal = 0;
    for (int j = 0; j < DESTINATIONS; j++)
        demandLeft[j] = demand[j];
    for (int i = 0; i < WAREHOUSES; i++)
        threadTime[i] = 0;

    std::cout << "\n" << BOLD << "Paleidziamos " << threadCount
              << " gijos...\n" << RESET << "\n";

    auto start = Clock::now();

    std::vector<std::thread> threads;
    int count = std::min(threadCount, WAREHOUSES);
    for (int i = 0; i < count; i++)
        threads.emplace_back(warehouseThread, i);
    for (auto& t : threads) t.join();

    double totalMs = Ms(Clock::now() - start).count();

    std::cout << "\n";
    printSep();
    printResults();
    printTimings(totalMs);

    double seq = 0;
    for (int i = 0; i < WAREHOUSES; i++) seq += threadTime[i];
    double speedup = (totalMs > 0) ? seq / totalMs : 1.0;
    history.push_back({threadCount, grandTotal, totalMs, speedup});
}

// ---------------------------------------------------------------------------
// Meniu: gijų palyginimas
// ---------------------------------------------------------------------------

static void menuCompareThreads() {
    clearScreen();
    printHeader();
    std::cout << BOLD << "\n  GIJU SKAICIAUS PALYGINIMAS\n" << RESET;
    printSep();
    std::cout << "\nBus paleista su 1, 2 ir " << WAREHOUSES
              << " gijomis ir palyginti laikai.\n";
    std::cout << YELLOW << "Spauskite Enter..." << RESET;
    std::cin.get();

    int maxT = std::min(3, WAREHOUSES);
    double times[3] = {0};
    int    costs[3] = {0};

    for (int t = 1; t <= maxT; t++) {
        std::cout << "\n" << BOLD << CYAN
                  << "--- Paleidimas su " << t << " gija(-omis) ---\n" << RESET;

        results.clear();
        grandTotal = 0;
        for (int j = 0; j < DESTINATIONS; j++)
            demandLeft[j] = demand[j];
        for (int i = 0; i < WAREHOUSES; i++)
            threadTime[i] = 0;

        auto start = Clock::now();
        std::vector<std::thread> threads;
        int count = std::min(t, WAREHOUSES);
        for (int i = 0; i < count; i++)
            threads.emplace_back(warehouseThread, i);
        for (auto& th : threads) th.join();

        times[t-1] = Ms(Clock::now() - start).count();
        costs[t-1] = grandTotal;
    }

    std::cout << "\n\n";
    printSep('=');
    std::cout << BOLD << "  PALYGINIMO REZULTATAI\n" << RESET;
    printSep('=');
    std::cout << std::left
              << std::setw(10) << "Gijos"
              << std::setw(16) << "Laikas (ms)"
              << std::setw(14) << "Pagreitis"
              << std::setw(14) << "Kaina (EUR)" << "\n";
    printSep();

    for (int t = 0; t < maxT; t++) {
        double speedup = (times[t] > 0) ? times[0] / times[t] : 1.0;
        std::string bar(static_cast<int>(speedup * 8), '|');
        std::cout << std::left
                  << std::setw(10) << (t + 1)
                  << std::setw(16) << std::fixed << std::setprecision(1) << times[t]
                  << std::setw(14) << std::fixed << std::setprecision(2) << speedup
                  << std::setw(14) << costs[t]
                  << GREEN << bar << RESET << "\n";
    }
    printSep();

    std::cout << "\n" << YELLOW << "Spauskite Enter..." << RESET;
    std::cin.get();
}

// ---------------------------------------------------------------------------
// Meniu: istorija
// ---------------------------------------------------------------------------

static void menuHistory() {
    clearScreen();
    printHeader();
    std::cout << BOLD << "\n  VYKDYMU ISTORIJA\n" << RESET;
    printSep();

    if (history.empty()) {
        std::cout << YELLOW << "\n  Istorija tuscia. Paleiskite simuliacija pirma.\n" << RESET;
    } else {
        std::cout << std::left
                  << std::setw(6)  << "Nr."
                  << std::setw(10) << "Gijos"
                  << std::setw(16) << "Laikas (ms)"
                  << std::setw(14) << "Pagreitis"
                  << std::setw(14) << "Kaina (EUR)" << "\n";
        printSep();

        for (int i = 0; i < (int)history.size(); i++) {
            auto& h = history[i];
            std::cout << std::left
                      << std::setw(6)  << (i + 1)
                      << std::setw(10) << h.threadCount
                      << std::setw(16) << std::fixed << std::setprecision(1) << h.timeMs
                      << std::setw(14) << std::fixed << std::setprecision(2) << h.speedup
                      << std::setw(14) << h.totalCost << "\n";
        }
        printSep();
        std::cout << GREEN << "  Viso vykdymu: " << history.size() << "\n" << RESET;
    }

    std::cout << "\n" << YELLOW << "Spauskite Enter..." << RESET;
    std::cin.get();
}

// ---------------------------------------------------------------------------
// Pagrindinis meniu
// ---------------------------------------------------------------------------

void runMenu() {
    enableColor();

    while (true) {
        clearScreen();
        printHeader();

        std::cout << "\n"
                  << BOLD << "  PAGRINDINIS MENIU\n\n" << RESET
                  << "  " << CYAN  << "[1]" << RESET << " Ivesti naujus duomenis\n"
                  << "  " << CYAN  << "[2]" << RESET << " Rodyti dabartinius duomenis\n"
                  << "  " << CYAN  << "[3]" << RESET << " Paleisti simuliacija (" << WAREHOUSES << " gijos)\n"
                  << "  " << CYAN  << "[4]" << RESET << " Pasirinkti giju skaiciu\n"
                  << "  " << CYAN  << "[5]" << RESET << " Palyginti 1 / 2 / " << WAREHOUSES << " gijas\n"
                  << "  " << CYAN  << "[6]" << RESET << " Vykdymu istorija\n"
                  << "  " << RED   << "[0]" << RESET << " Iseiti\n"
                  << "\n";

        if (!history.empty()) {
            auto& last = history.back();
            std::cout << MAGENTA << "  Paskutinis: "
                      << last.threadCount << " gijos | "
                      << std::fixed << std::setprecision(1) << last.timeMs << " ms | "
                      << last.totalCost << " EUR\n" << RESET;
        }

        printSep();
        int choice = readInt("  Pasirinkimas [0-6]: ", 0, 6);

        switch (choice) {
            case 1:
                clearScreen();
                printHeader();
                inputData();
                break;

            case 2:
                clearScreen();
                printHeader();
                showCurrentData();
                std::cout << "\n" << YELLOW << "Spauskite Enter..." << RESET;
                std::cin.get();
                break;

            case 3:
                clearScreen();
                printHeader();
                runWithThreadCount(WAREHOUSES);
                std::cout << "\n" << YELLOW << "Spauskite Enter..." << RESET;
                std::cin.get();
                break;

            case 4: {
                int tc = readInt("  Giju skaicius (1-" + std::to_string(WAREHOUSES) + "): ",
                                 1, WAREHOUSES);
                clearScreen();
                printHeader();
                runWithThreadCount(tc);
                std::cout << "\n" << YELLOW << "Spauskite Enter..." << RESET;
                std::cin.get();
                break;
            }

            case 5:
                menuCompareThreads();
                break;

            case 6:
                menuHistory();
                break;

            case 0:
                clearScreen();
                std::cout << GREEN << "\n  Programa baigta. Viso gero!\n\n" << RESET;
                return;
        }
    }
}