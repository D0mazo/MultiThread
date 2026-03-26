#pragma once

#include <vector>
#include "../Calculations/Cargo.h"

// Funkcija skirta įvesti siuntų duomenis iš testinio failo
std::vector<Cargo> loadTestData();
// Funkcija skirta įvesti siuntų duomenis iš konsolės
std::vector<Cargo> inputData();