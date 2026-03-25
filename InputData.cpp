#include "InputData.h"
#include <iostream>
#include <vector>
#include <fstream>


std::vector<Cargo> inputData()
{
    int n;
    std::cout << "\n=== DUOMENU IVEDIMAS ===\n";
    std::cout << "Kiek siuntu? ";
    std::cin >> n;

    if (n <= 0)
    {
        std::cout << "[!] Siuntu skaicius turi buti teigiamas.\n";
        return {};
    }

    std::vector<Cargo> data(n);

    for (int i = 0; i < n; i++)
    {
        std::cout << "\nSiunta #" << i + 1 << ":\n";
        std::cout << "  Svoris (kg)   : ";
        std::cin >> data[i].weight;
        std::cout << "  Atstumas (km) : ";
        std::cin >> data[i].distance;
    }

    std::cout << "\n[OK] " << n << " siuntos sekmingai ivesta.\n";
    return data;
}


std::vector<Cargo> loadTestData()
{
    std::vector<Cargo> data;
    std::ifstream file("../DataGenerator/test_data.txt");

    if (!file.is_open())
    {
        std::cout << "[!] Failas 'test_data.txt' nerastas!\n";
        std::cout << "    Paleiskite DataGenerator programa, kad sukurtumete faila.\n";
        return data;
    }

    int n;
    if (!(file >> n) || n <= 0)
    {
        std::cout << "[!] Netinkamas failo formatas. Pirma eilute turi buti siuntu kiekis.\n";
        return data;
    }


    data.resize(n);

    for (int i = 0; i < n; ++i)
    {
        if (!(file >> data[i].weight >> data[i].distance))
        {
            std::cout << "[!] Klaida skaitant siunta #" << i + 1 << ". Failas gali buti sugadintas.\n";
            data.clear();
            return data;
        }
    }

    std::cout << "[+] Ikelta " << n << " siuntu is test_data.txt\n";
    std::cout << "    Paruosta skaiciuoti. Pasirinkite varianta 3.\n\n";
    return data;
}