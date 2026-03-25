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
            case 1:  // Rankinis duomenų įvedimas
            {
                data = inputData();
                break;
            }

            case 2:  // Įkelti testinius duomenis iš failo
            {
                data = loadTestData();
                break;
            }



            case 3:  // Skaiciuoti (viengijis vs daugiagijis)
            {
                if (data.empty())
                {
                    std::cout << "\n[!] Nera duomenu! Pirmiausia iveskite arba sugeneruokite siuntas (3 arba 2).\n\n";
                    break;
                }

                int threads;
                std::cout << "\nKiek giju naudoti? ";
                std::cin >> threads;

                if (threads <= 0)
                {
                    std::cout << "[!] Netinkamas giju skaicius!\n\n";
                    break;
                }

                // Viengijis skaičiavimas
                auto t1 = std::chrono::high_resolution_clock::now();
                long long s1 = calculateSingle(data);
                auto t2 = std::chrono::high_resolution_clock::now();

                // Daugiagijis skaičiavimas
                auto t3 = std::chrono::high_resolution_clock::now();
                long long s2 = calculateMulti(data, threads);
                auto t4 = std::chrono::high_resolution_clock::now();

                // Rezultatai (gražesnis išvedimas)
                double time_single = std::chrono::duration<double, std::milli>(t2 - t1).count();
                double time_multi  = std::chrono::duration<double, std::milli>(t4 - t3).count();
                double speedup = (time_multi > 0.0) ? (time_single / time_multi) : 0.0;

                std::cout << "\n========== REZULTATAI ==========\n";
                std::cout << "\n[Viengijis]\n";
                std::cout << "Bendra kaina : " << s1 << "\n";
                std::cout << "Laikas       : " << time_single << " ms\n";

                std::cout << "\n[Daugiagijis]\n";
                std::cout << "Bendra kaina : " << s2 << "\n";
                std::cout << "Laikas       : " << time_multi << " ms\n";

                std::cout << "\nGreitinimo koeficientas: " << speedup << "x\n";
                std::cout << "\n================================\n\n";
                break;
            }

            case 0:
            {
                std::cout << "\nPrograma baigta.\n";
                break;
            }

            default:
            {
                std::cout << "\n[!] Neteisingas pasirinkimas!\n\n";
            }
        }
    } while (choice != 0);

    return 0;
}