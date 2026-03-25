#include <iostream>
#include <fstream>
#include <random>
#include <chrono>

int main()
{
    const long long NUM_RECORDS = 50000; // can increase for bigger datasets
    std::cout << "Pradedama generuoti " << NUM_RECORDS << " siuntu...\n";

    std::ofstream file("test_data.txt");
    if (!file.is_open())
    {
        std::cout << "[!] Nepavyko sukurti failo 'test_data.txt'\n";
        return 1;
    }

    file << NUM_RECORDS << '\n';

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> weight_dist(50, 1000);
    std::uniform_int_distribution<int> distance_dist(100, 2000);

    for (long long i = 0; i < NUM_RECORDS; ++i)
    {
        int weight = weight_dist(gen);
        int distance = distance_dist(gen);
        file << weight << " " << distance << '\n';
    }

    file.close();
    std::cout << "Failas sugeneruotas: test_data.txt, " << NUM_RECORDS << " siuntu\n";
    return 0;
}