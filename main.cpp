#include <iostream>
#include <chrono>
#include <vector>
#include "Cargo.h"
#include "InputData.h"
#include "Menu.h"

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

                int threads;
                std::cout << "\nKiek giju naudoti? ";
                std::cin >> threads;

                if (threads <= 0)
                {
                    std::cout << "[!] Netinkamas giju skaicius!\n";
                    break;
                }

                // įrašome laiką prieš viengijį skaičiavimą
                auto t1 = std::chrono::high_resolution_clock::now();
                // vykdome viengijį skaičiavimą
                long long s1 = calculateSingle(data);
                // įrašome laiką po viengijio skaičiavimo
                auto t2 = std::chrono::high_resolution_clock::now();
                // įrašome laiką prieš daugiagijį skaičiavimą
                auto t3 = std::chrono::high_resolution_clock::now();
                // vykdome daugiagijį skaičiavimą
                long long s2 = calculateMulti(data, threads);
                // įrašome laiką po daugiagijio skaičiavimo
                auto t4 = std::chrono::high_resolution_clock::now();
                // apskaičiuojame viengijio trukmę milisekundėmis
                double time_single = std::chrono::duration<double, std::milli>(t2 - t1).count();
                // apskaičiuojame daugiagijio trukmę milisekundėmis
                double time_multi  = std::chrono::duration<double, std::milli>(t4 - t3).count();
                // apskaičiuojame pagreitį: kiek kartų daugiagijis greitesnis
                double speedup = (time_multi > 0.0) ? (time_single / time_multi) : 0.0;

                std::cout << "\n========== REZULTATAI ==========\n";
                std::cout << "[Viengijis]\n";
                std::cout << "  Bendra kaina : " << s1 << "\n";
                std::cout << "  Laikas       : " << time_single << " ms\n";
                std::cout << "  // Vienas branduolys, nuoseklus skaiciavimas.\n";

                std::cout << "\n[Daugiagijis (" << threads << " giju)]\n";
                std::cout << "  Bendra kaina : " << s2 << "\n";
                std::cout << "  Laikas       : " << time_multi << " ms\n";
                std::cout << "  // Darbas padalintas tarp " << threads << " giju lygiagreciu.\n";

                std::cout << "\n[Isvada]\n";
                if (speedup > 1.1)
                {
                    std::cout << "  >> Daugiagijis yra GREITESNIS: " << speedup << "x pagreitis.\n";
                    std::cout << "  >> Kuo daugiau branduoliu ir duomenu, tuo didesnis pokytis.\n";
                }
                else if (speedup < 0.95)
                {
                    std::cout << "  >> Viengijis yra greitesnis si karta (" << speedup << "x).\n";
                    std::cout << "  >> Mazam duomenu kiekiui giju kurimo kastai virsija nauda.\n";
                    std::cout << "  >> Bandykite su didesniu duomenu kiekiu (>500k irasu).\n";
                }
                else
                {
                    std::cout << "  >> Rezultatai panasus (" << speedup << "x).\n";
                    std::cout << "  >> Padidinkite duomenu kieki arba giju skaiciu, kad matytumete skirtuma.\n";
                }

                if (s1 != s2)
                    std::cout << "\n  [!] ISPEJIMAS: Rezultatai nesutampa! Tikrinkite logika.\n";
                else
                    std::cout << "  [OK] Abu metodai grazina vienoda suma - skaiciavimai teisingi.\n";


                break;
            }

            case 0: std::cout << "\nPrograma baigta.\n"; break;
            default: std::cout << "\n[!] Neteisingas pasirinkimas!\n"; break;
        }
    } while (choice != 0);

    return 0;
}