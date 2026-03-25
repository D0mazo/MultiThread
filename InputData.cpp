#include "InputData.h"
#include "Cargo.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <limits>

// ---------------------------------------------------------------------------
// Spalvu makrosai (tie patys kaip Menu.cpp)
// ---------------------------------------------------------------------------

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define CYAN    "\033[36m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"

// ---------------------------------------------------------------------------
// Dinaminiai duomenys (gali keistis per InputData)
// ---------------------------------------------------------------------------

// Siuos kintamuosius naudosime vietoj const masyvų iš Cargo.cpp
// Cargo.cpp globalus kintamieji bus atnaujinami per šias kopijas

static int inputCost[4][4]     = {
    {4, 8, 1, 0},
    {7, 2, 3, 0},
    {3, 6, 9, 0},
    {0, 0, 0, 0}
};
static int inputSupply[4]      = {120, 80, 100, 0};
static int inputDemand[4]      = {150, 100, 50, 0};
static int inputWarehouses     = 3;
static int inputDestinations   = 3;

static std::string inputWarehouseNames[4] = {"Sandelis 1","Sandelis 2","Sandelis 3",""};
static std::string inputDestNames[4]      = {"Taskas A","Taskas B","Taskas C",""};

// ---------------------------------------------------------------------------
// Pagalbines funkcijos
// ---------------------------------------------------------------------------

static void printSep(char c = '-', int n = 60) {
    std::cout << std::string(n, c) << "\n";
}

static int readInt(const std::string& prompt, int lo, int hi) {
    int v;
    while (true) {
        std::cout << YELLOW << prompt << RESET;
        if (std::cin >> v && v >= lo && v <= hi) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return v;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << RED << "  Klaida: iveskite skaiciu nuo "
                  << lo << " iki " << hi << "\n" << RESET;
    }
}

static std::string readString(const std::string& prompt) {
    std::string s;
    std::cout << YELLOW << prompt << RESET;
    std::getline(std::cin, s);
    if (s.empty()) return "?";
    return s;
}

// ---------------------------------------------------------------------------
// Perkelti vartotojo duomenis į Cargo globalius kintamuosius
// ---------------------------------------------------------------------------

static void applyToCargo() {
    // Atnaujiname Cargo.h extern kintamuosius
    // (jie deklaruoti kaip extern, bet apibrėžti Cargo.cpp —
    //  modifikuojame per žymeklius)

    // sandėlių skaičius
    const_cast<int&>(WAREHOUSES)    = inputWarehouses;
    const_cast<int&>(DESTINATIONS)  = inputDestinations;

    for (int i = 0; i < inputWarehouses; i++) {
        for (int j = 0; j < inputDestinations; j++)
            const_cast<int&>(cost[i][j]) = inputCost[i][j];
        const_cast<int&>(supply[i])      = inputSupply[i];
        warehouseNames[i]                = inputWarehouseNames[i];
    }
    for (int j = 0; j < inputDestinations; j++) {
        demand[j]    = inputDemand[j];
        destNames[j] = inputDestNames[j];
    }
}

// ---------------------------------------------------------------------------
// Vartotojo ivesti duomenys — pagrindinė funkcija
// ---------------------------------------------------------------------------

void inputData() {
    std::cout << "\n";
    printSep('=');
    std::cout << BOLD << CYAN
              << "  DUOMENU SUVEDIMAS\n" << RESET;
    printSep('=');

    // 1. Sandėlių skaičius
    std::cout << "\n" << BOLD << "1. Sandėliai\n" << RESET;
    inputWarehouses = readInt("   Kiek sandėlių? (1-4): ", 1, 4);

    for (int i = 0; i < inputWarehouses; i++) {
        std::cout << "\n" << CYAN << "  Sandelis " << (i+1) << ":\n" << RESET;
        inputWarehouseNames[i] = readString(
            "    Pavadinimas (Enter = Sandelis " + std::to_string(i+1) + "): ");
        if (inputWarehouseNames[i] == "?")
            inputWarehouseNames[i] = "Sandelis " + std::to_string(i+1);
        inputSupply[i] = readInt("    Tiekimas (tonos, 1-9999): ", 1, 9999);
    }

    // 2. Paskirties taškų skaičius
    std::cout << "\n" << BOLD << "2. Paskirties taskai\n" << RESET;
    inputDestinations = readInt("   Kiek tasku? (1-4): ", 1, 4);

    for (int j = 0; j < inputDestinations; j++) {
        std::cout << "\n" << CYAN << "  Taskas " << (j+1) << ":\n" << RESET;
        inputDestNames[j] = readString(
            "    Pavadinimas (Enter = Taskas " + std::to_string(j+1) + "): ");
        if (inputDestNames[j] == "?")
            inputDestNames[j] = "Taskas " + std::to_string(j+1);
        inputDemand[j] = readInt("    Poreikis (tonos, 1-9999): ", 1, 9999);
    }

    // 3. Sąnaudų matrica
    std::cout << "\n" << BOLD << "3. Sanudu matrica (EUR/t)\n" << RESET;
    std::cout << "   Iveskite pervezimo kaina is kiekvieno sandelio i kiekviena taska:\n\n";

    for (int i = 0; i < inputWarehouses; i++) {
        for (int j = 0; j < inputDestinations; j++) {
            std::string prompt = "   cost[" + inputWarehouseNames[i]
                               + " -> " + inputDestNames[j] + "]: ";
            inputCost[i][j] = readInt(prompt, 1, 9999);
        }
        std::cout << "\n";
    }

    // 4. Perkeliame į Cargo
    applyToCargo();

    // 5. Suvestinė
    std::cout << "\n";
    printSep();
    std::cout << GREEN << BOLD << "  Duomenys isaugoti!\n\n" << RESET;
    showCurrentData();

    std::cout << "\n" << YELLOW << "Spauskite Enter..." << RESET;
    std::cin.get();
}

// ---------------------------------------------------------------------------
// Rodyti dabartinius duomenis
// ---------------------------------------------------------------------------

void showCurrentData() {
    std::cout << "\n" << BOLD << "  DABARTINIAI DUOMENYS\n" << RESET;
    printSep();

    // Antraštė
    std::cout << std::setw(16) << " ";
    for (int j = 0; j < DESTINATIONS; j++)
        std::cout << std::setw(12) << destNames[j];
    std::cout << std::setw(12) << "Tiekimas\n";
    printSep();

    // Eilutės
    for (int i = 0; i < WAREHOUSES; i++) {
        std::cout << std::setw(16) << warehouseNames[i];
        for (int j = 0; j < DESTINATIONS; j++)
            std::cout << std::setw(12) << cost[i][j];
        std::cout << std::setw(11) << supply[i] << " t\n";
    }

    // Poreikiai
    printSep();
    std::cout << std::setw(16) << "Poreikis";
    for (int j = 0; j < DESTINATIONS; j++)
        std::cout << std::setw(11) << demand[j] << " t";
    std::cout << "\n";
    printSep();

    // Balansas
    int totalSupply = 0, totalDemand = 0;
    for (int i = 0; i < WAREHOUSES; i++)    totalSupply += supply[i];
    for (int j = 0; j < DESTINATIONS; j++) totalDemand += demand[j];

    std::cout << "\n"
              << "  Bendras tiekimas : " << CYAN << totalSupply << " t\n" << RESET
              << "  Bendras poreikis : " << CYAN << totalDemand << " t\n" << RESET;

    if (totalSupply == totalDemand)
        std::cout << GREEN << "  Balansas: SUBALANSUOTAS\n" << RESET;
    else if (totalSupply > totalDemand)
        std::cout << YELLOW << "  Balansas: Perteklius +" << (totalSupply - totalDemand) << " t\n" << RESET;
    else
        std::cout << RED    << "  Balansas: Trukumas -" << (totalDemand - totalSupply) << " t\n" << RESET;
}