#include <iostream>
#include <chrono>
#include "Cargo.h"
#include "InputData.h"
#include "Menu.h"

int main() {
    std::vector<Cargo> data;
    int choice;

    do {
        showMenu();
        std::cin >> choice;

        if (choice == 1) {
            data = inputData();
        }
        else if (choice == 2) {
            if (data.empty()) {
                std::cout << "Nera duomenu!\n";
                continue;
            }

            int threads;
            std::cout << "Kiek giju? ";
            std::cin >> threads;

            auto t1 = std::chrono::high_resolution_clock::now();
            long long s1 = calculateSingle(data);
            auto t2 = std::chrono::high_resolution_clock::now();

            auto t3 = std::chrono::high_resolution_clock::now();
            long long s2 = calculateMulti(data, threads);
            auto t4 = std::chrono::high_resolution_clock::now();

            std::cout << "\nViengijis rezultatas: " << s1;
            std::cout << "\nLaikas: "
                      << std::chrono::duration<double>(t2 - t1).count() << " s\n";

            std::cout << "\nDaugiagijis rezultatas: " << s2;
            std::cout << "\nLaikas: "
                      << std::chrono::duration<double>(t4 - t3).count() << " s\n";
        }

    } while (choice != 0);

    return 0;
}