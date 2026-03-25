#include "InputData.h"
#include <iostream>
#include <vector>
#include <fstream>

std::vector<Cargo> inputData() {
    int n;
    std::cout << "\n=== DUOMENU IVEDIMAS ===\n";
    std::cout << "Kiek siuntu? ";
    std::cin >> n;

    std::vector<Cargo> data(n);

    for (int i = 0; i < n; i++) {
        std::cout << "\nSiunta #" << i + 1 << ":\n";
        std::cout << "  Svoris: ";
        std::cin >> data[i].weight;
        std::cout << "  Atstumas: ";
        std::cin >> data[i].distance;
    }

    std::cout << "\nDuomenys sekmingai ivesti!\n";
    return data;
}
std::vector<Cargo> loadTestData()
{
    std::vector<Cargo> data;
    std::ifstream file("../test_data.txt");

    if (!file.is_open())
    {
        std::cout << "[!] Failas 'test_data.txt' nerastas!\n";
        return data;
    }

    int n;
    if (!(file >> n) || n <= 0)
    {
        std::cout << "[!] Netinkamas failo formatas! Pirma eilute turi buti siuntu kiekis.\n";
        return data;
    }

    data.resize(n);
    for (int i = 0; i < n; ++i)
    {
        if (!(file >> data[i].weight >> data[i].distance))
        {
            std::cout << "[!] Klaida skaitant siunta #" << i + 1 << "\n";
            data.clear();
            return data;
        }
    }

    std::cout << "[+] Sekmingai ikelta " << n << " siuntu is test_data.txt\n\n";
    return data;
}