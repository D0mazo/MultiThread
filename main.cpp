#include <iostream>
#include <chrono>
#include <vector>
#include "Cargo.h"
#include "InputData.h"
#include "Menu.h"
#include <iomanip>

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

                calculateSingle(data);
                //namu
                //calculateMulti(data, 16);
                //nešiojamas
                calculateMulti(data, 12);

                // single thread
                // laiko matavimas
                auto t1 = std::chrono::high_resolution_clock::now();
                long long singleResult = calculateSingle(data);
                auto t2 = std::chrono::high_resolution_clock::now();
                double time_single = std::chrono::duration<double, std::milli>(t2 - t1).count();

                // --- Automatinis daugiagijis testavimas: 2, 4, 8, 16 gijų namų kompiuteris---
                //int threadCounts[] = {2, 4, 8, 16};
                // --- Automatinis daugiagijis testavimas: 2, 4, 8, 12 nešiojimas---
                int threadCounts[] = {2, 4, 6, 12};
                double times[4];
                long long results[4];

                for (int i = 0; i < 4; i++)
                {
                    // įrašome laiką prieš daugiagijį skaičiavimą
                    auto ts = std::chrono::high_resolution_clock::now();
                    // vykdome daugiagijį skaičiavimą su threadCounts[i] gijų
                    results[i] = calculateMulti(data, threadCounts[i]);
                    // įrašome laiką po daugiagijio skaičiavimo
                    auto te = std::chrono::high_resolution_clock::now();
                    // apskaičiuojame trukmę milisekundėmis
                    times[i] = std::chrono::duration<double, std::milli>(te - ts).count();
                }

                // --- Rezultatų lentelė ---
                std::cout << "\n========== REZULTATAI ==========\n";
                std::cout << "\n[Viengijis]\n";
                std::cout << "  Bendra kaina : " << singleResult << "\n";
                std::cout << "  Laikas       : " << time_single << " ms\n";
                std::cout << "  Vienas branduolys, nuoseklsus skaiciavimas\n";

                std::cout << "\n[Daugiagijis palyginimas]\n";
                std::cout << "  Gijos  | Laikas (ms)  | Pagreitis | Bendra kaina\n";
                std::cout << "  -------|--------------|-----------|-------------\n";

                for (int i = 0; i < 4; i++)
                {
                    double speedup = (times[i] > 0.0) ? (time_single / times[i]) : 0.0;
                    std::cout << "  " << std::setw(5)  << threadCounts[i]
                              << "  | " << std::setw(12) << std::fixed << std::setprecision(2) << times[i]
                              << "  | " << std::setw(9)  << std::fixed << std::setprecision(3) << speedup << "x"
                              << "  | " << std::setw(13) << results[i] << "\n";
                }

                // --- Geriausio varianto nustatymas ---
                // Randame kuriam gijų skaičiui užtruko mažiausiai laiko.
                int bestIdx = 0;
                for (int i = 1; i < 4; i++)
                    if (times[i] < times[bestIdx]) bestIdx = i;

                double bestSpeedup = (times[bestIdx] > 0.0) ? (time_single / times[bestIdx]) : 0.0;

                std::cout << "\n[Isvada]\n";
                std::cout << "  >> Geriausias variantas: " << threadCounts[bestIdx] << " gijos ("
                          << bestSpeedup << "x pagreitis).\n";


                // --- Rezultatų teisingumas ---
                // Tikriname ar visos gijos grąžino tą pačią sumą — jei ne, yra klaida formulėje.
                bool allMatch = true;
                for (int i = 0; i < 4; i++)
                    if (results[i] != singleResult) allMatch = false;

                if (allMatch)
                    std::cout << "  [OK] Visi metodai grazina vienoda suma - skaiciavimai teisingi.\n";
                else
                    std::cout << "  [!] ISPEJIMAS: Rezultatai nesutampa.\n";
                break;
            }

            case 0: std::cout << "\nPrograma baigta.\n"; break;
            default: std::cout << "\n[!] Neteisingas pasirinkimas!\n"; break;
        }
    } while (choice != 0);

    return 0;
}