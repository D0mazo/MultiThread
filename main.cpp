#include <iostream>
#include <vector>
#include "Cargo.h"
#include "InputData.h"
#include "Menu.h"
#include "Functions/Benchmark.h"

int main()
{
    std::vector<Cargo> data;
    int choice;

    do
    {
        showMenu();
        std::cin >> choice;

        switch (choice)
        {
            case 1: data = inputData(); break;
            case 2: data = loadTestData(); break;

            case 3:
            {
                if (data.empty())
                {
                    std::cout << "\n[!] Nera duomenu! Pirmiausia iveskite arba sugeneruokite siuntas.\n";
                    break;
                }
                runBenchmark(data);
                break;
            }

            case 0: std::cout << "\nPrograma baigta.\n"; break;
            default: std::cout << "\n[!] Neteisingas pasirinkimas!\n"; break;
        }
    } while (choice != 0);

    return 0;
}