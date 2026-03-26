#include <iostream>
#include <fstream>
#include <random>
#include <chrono>

int main()
{

    const long long NUM_RECORDS = 7800000;

    std::cout << "Pradedama generuoti " << NUM_RECORDS << " siuntu...\n";

    std::ofstream file("test_data.txt");
    if (!file.is_open())
    {
        std::cout << "[!] Nepavyko sukurti failo 'test_data.txt'\n";
        return 1;
    }


    // pirma failo eilutė — įrašų kiekis, kurį nuskaitys loadTestData()
    file << NUM_RECORDS << '\n';
    // inicializuojame atsitiktinių skaičių generatorių
    std::random_device rd;
    // Mersenne Twister algoritmas
    std::mt19937 gen(rd());
    // svorio pasiskirstymas: 50–1000 kg
    std::uniform_int_distribution<int> weight_dist(50, 1000);
    // atstumo pasiskirstymas: 100–2000 km
    std::uniform_int_distribution<int> distance_dist(100, 2000);
    // įrašome laiką prieš pradedant generuoti duomenis
    auto start = std::chrono::high_resolution_clock::now();
    // generuojame kiekvieną įrašą ir iš karto rašome į failą
    for (long long i = 0; i < NUM_RECORDS; ++i)
        file << weight_dist(gen) << " " << distance_dist(gen) << '\n';

    file.close();

    // įrašome laiką po generavimo
    auto end = std::chrono::high_resolution_clock::now();
    // apskaičiuojame kiek milisekundžių užtruko visas generavimas
    double ms = std::chrono::duration<double, std::milli>(end - start).count();

    std::cout << "Failas sugeneruotas: test_data.txt\n";
    std::cout << "  Irasai : " << NUM_RECORDS << "\n";
    std::cout << "  Laikas : " << ms << " ms\n";
    return 0;
}