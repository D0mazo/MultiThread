#include "InputData.h"
#include <iostream>

std::vector<Cargo> inputData() {
    int n;
    std::cout << "Kiek siuntu? ";
    std::cin >> n;

    std::vector<Cargo> data(n);

    std::cout << "Iveskite (svoris atstumas):\n";
    for (int i = 0; i < n; i++)
        std::cin >> data[i].weight >> data[i].distance;

    return data;
}