#include "Menu.h"
#include "Cargo.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <algorithm>

using Clock = std::chrono::steady_clock;
using Ms    = std::chrono::duration<double, std::milli>;

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
// Spalvotas tekstas (Windows ANSI)
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
#define BLUE    "\033[34m"

// ---------------------------------------------------------------------------
// Pagalbines funkcijos
// ---------------------------------------------------------------------------

static void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

static void printSeparator(char c = '-', int len = 60) {
    std::cout << std::string(len, c) << "\n";
}

static void printHeader() {
    printSeparator('=');
    std::cout << BOLD << CYAN
              << "   KORVINIU PERVEZIMO OPTIMIZAVIMAS\n"
              << "   Daugiagileja sistema  |  C++ threads\n"
              << RESET;
    printSeparator('=');
}

static int readInt(const std::string& prompt, int minVal, int maxVal) {
    int val;
    while (true) {
        std::cout << YELLOW << prompt << RESET;
        if (std::cin >> val && val >= minVal && val <= maxVal) {
            std::cin.ignore();
            return val;
        }
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << RED << "  Klaida: iveskite skaiciu nuo "
                  << minVal << " iki " << maxVal << "\n" << RESET;
    }
}

// ---------------------------------------------------------------------------
// Simuliacija su pasirinktu giju skaiciumi
// ---------------------------------------------------------------------------

static void runWithThreadCount(int threadCount) {
    // Atstatome busena
    results.clear();
    grandTotal = 0;
    for (int j = 0; j < DESTINATIONS; j++)
        demandLeft[j] = demand[j];
    for (int i = 0; i < WAREHOUSES; i++)
        threadTime[i] = 0;

    std::cout << "\n" << BOLD << "Paleidziamos " << threadCount
              << " gijos..." << RESET << "\n\n";

    auto start = Clock::now();

    // Paleidziame threadCount giju (paskirstome sandelius tarp jų)
    std::vector<std::thread> threads;

    if (threadCount <= WAREHOUSES) {
        // Maziau arba lygiai sandeliu - kiekviena gija gauna bent viena
        // Paskirstome sandelius kiek galime
        for (int i = 0; i < threadCount && i < WAREHOUSES; i++)
            threads.emplace_back(warehouseThread, i);
    } else {
        // Daugiau giju nei sandeliu - papildomos gijos dirba kartu
        // Kiekvienas sandelis gauna savo gija, likusios laukia
        for (int i = 0; i < WAREHOUSES; i++)
            threads.emplace_back(warehouseThread, i);
    }

    for (auto& t : threads)
        t.join();

    double totalMs = Ms(Clock::now() - start).count();

    std::cout << "\n";
    printSeparator();
    printResults();
    printTimings(totalMs);

    // Issaugome i istorija
    double seq = 0;
    for (int i = 0; i < WAREHOUSES; i++) seq += threadTime[i];
    double speedup = (totalMs > 0) ? seq / totalMs : 1.0;
    history.push_back({threadCount, grandTotal, totalMs, speedup});
}

// ---------------------------------------------------------------------------
// Meniu: keisti matrica
// ---------------------------------------------------------------------------

static void menuEditMatrix() {
    clearScreen();
    printHeader();
    std::cout << BOLD << "\n  SANAUDU MATRICOS REDAGAVIMAS\n" << RESET;
    printSeparator();

    std::cout << "\nDabar esanti matrica:\n\n";
    printMatrix();

    std::cout << "\n" << YELLOW
              << "Iveskite naują kaina cost[i][j]\n"
              << "(i = sandelis 0-" << WAREHOUSES-1
              << ", j = taskas 0-" << DESTINATIONS-1 << ")\n"
              << RESET;

    int i = readInt("Sandelio numeris (i): ", 0, WAREHOUSES - 1);
    int j = readInt("Paskirties tasko numeris (j): ", 0, DESTINATIONS - 1);
    int newCost = readInt("Nauja kaina (EUR/t): ", 1, 9999);

    // cost[] yra const - demonstruojame tik kaip tai veiktu
    // (realiame projekte reiktu naudoti nekonstantine matrica)
    std::cout << GREEN << "\n  [Demonstruojama] cost[" << i << "][" << j
              << "] butu pakeista i " << newCost << " EUR/t\n"
              << "  Norint keisti - matrica turi buti nekonstantine.\n"
              << RESET;

    std::cout << "\nSpauskite Enter...";
    std::cin.get();
}

// ---------------------------------------------------------------------------
// Meniu: giju palyginimas
// ---------------------------------------------------------------------------

static void menuCompareThreads() {
    clearScreen();
    printHeader();
    std::cout << BOLD << "\n  GIJU SKAICIAUS PALYGINIMAS\n" << RESET;
    printSeparator();
    std::cout << "\nBus paleista su 1, 2 ir 3 gijomis ir palyginti laikai.\n";
    std::cout << YELLOW << "Spauskite Enter kad pradeti..." << RESET;
    std::cin.get();

    double times[3];
    int costs[3];

    for (int t = 1; t <= 3; t++) {
        std::cout << "\n" << BOLD << CYAN
                  << "--- Paleidimas su " << t << " gija(-omis) ---\n"
                  << RESET;

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

    // Suvestine lentele
    std::cout << "\n\n";
    printSeparator('=');
    std::cout << BOLD << "  PALYGINIMO REZULTATAI\n" << RESET;
    printSeparator('=');
    std::cout << std::left
              << std::setw(10) << "Gijos"
              << std::setw(16) << "Laikas (ms)"
              << std::setw(16) << "Pagreitis"
              << std::setw(14) << "Kaina (EUR)"
              << "\n";
    printSeparator();

    for (int t = 0; t < 3; t++) {
        double speedup = times[0] / times[t];
        std::string bar(static_cast<int>(speedup * 10), '|');
        std::cout << std::left
                  << std::setw(10) << (t + 1)
                  << std::setw(16) << std::fixed << std::setprecision(1) << times[t]
                  << std::setw(16) << std::fixed << std::setprecision(2) << speedup
                  << std::setw(14) << costs[t]
                  << GREEN << bar << RESET << "\n";
    }
    printSeparator();

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
    printSeparator();

    if (history.empty()) {
        std::cout << YELLOW << "\n  Istorija tuscia. Paleiskite simuliacija.\n" << RESET;
    } else {
        std::cout << std::left
                  << std::setw(6)  << "Nr."
                  << std::setw(10) << "Gijos"
                  << std::setw(16) << "Laikas (ms)"
                  << std::setw(14) << "Pagreitis"
                  << std::setw(14) << "Kaina (EUR)"
                  << "\n";
        printSeparator();

        for (int i = 0; i < (int)history.size(); i++) {
            auto& h = history[i];
            std::cout << std::left
                      << std::setw(6)  << (i + 1)
                      << std::setw(10) << h.threadCount
                      << std::setw(16) << std::fixed << std::setprecision(1) << h.timeMs
                      << std::setw(14) << std::fixed << std::setprecision(2) << h.speedup
                      << std::setw(14) << h.totalCost
                      << "\n";
        }
        printSeparator();
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
                  << BOLD << "  PAGRINDINIS MENIU\n" << RESET
                  << "\n"
                  << "  " << CYAN << "[1]" << RESET << " Paleisti su " << WAREHOUSES << " gijomis (standartinis)\n"
                  << "  " << CYAN << "[2]" << RESET << " Pasirinkti giju skaiciu\n"
                  << "  " << CYAN << "[3]" << RESET << " Palyginti 1 / 2 / 3 gijas\n"
                  << "  " << CYAN << "[4]" << RESET << " Rodyti sanaudu matrica\n"
                  << "  " << CYAN << "[5]" << RESET << " Redaguoti matrica\n"
                  << "  " << CYAN << "[6]" << RESET << " Vykdymu istorija\n"
                  << "  " << RED  << "[0]" << RESET << " Iseiti\n"
                  << "\n";

        if (!history.empty()) {
            auto& last = history.back();
            std::cout << MAGENTA << "  Paskutinis vykdymas: "
                      << last.threadCount << " gijos | "
                      << std::fixed << std::setprecision(1) << last.timeMs << " ms | "
                      << last.totalCost << " EUR\n" << RESET;
        }

        printSeparator();
        int choice = readInt("  Pasirinkimas: ", 0, 6);

        switch (choice) {
            case 1:
                clearScreen();
                printHeader();
                runWithThreadCount(WAREHOUSES);
                std::cout << "\n" << YELLOW << "Spauskite Enter..." << RESET;
                std::cin.get();
                break;

            case 2: {
                int tc = readInt("  Giju skaicius (1-8): ", 1, 8);
                clearScreen();
                printHeader();
                runWithThreadCount(tc);
                std::cout << "\n" << YELLOW << "Spauskite Enter..." << RESET;
                std::cin.get();
                break;
            }

            case 3:
                menuCompareThreads();
                break;

            case 4:
                clearScreen();
                printHeader();
                printMatrix();
                std::cout << "\n" << YELLOW << "Spauskite Enter..." << RESET;
                std::cin.get();
                break;

            case 5:
                menuEditMatrix();
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