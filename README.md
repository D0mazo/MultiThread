# MultiThread — Lygiagretaus programavimo laboratorinis darbas

Proejektinis darbas, demonstruojantis viengijio ir daugiagijio skaičiavimo palyginimą C++ aplinkoje.

## Aprašymas

Programa modeliuoja logistikos sistemą, kurioje skaičiuojama krovinių pervežimo kaina pagal svorį ir atstumą. Skaičiavimas atliekamas dviem būdais — vienu giju ir keliais gijais — ir palyginami rezultatai bei vykdymo laikas.

## Projekto struktūra

```
MultiThread/
├── main.cpp            # Pagrindinis programos failas, matavimai ir rezultatų lentelė
├── Cargo.cpp           # Viengijio ir daugiagijio skaičiavimo logika
├── Cargo.h             # Cargo struktūra ir funkcijų deklaracijos
├── InputData.cpp       # Duomenų įvedimas iš konsolės ir failo
├── InputData.h         # Funkcijų deklaracijos
├── Menu.cpp            # Meniu rodymas
├── Menu.h              # Meniu deklaracija
├── CMakeLists.txt      # Projekto konfigūracija
└── DataGenerator/
    ├── generate_data.cpp   # Testinių duomenų generatorius
    └── test_data.txt       # Sugeneruoti testiniai duomenys
```


### Paleidimas

```bash
./MultiThread
```

## Naudojimas

Paleidus programą, pasirodo meniu:

```
1. Ivesti duomenis          — įvesti siuntas ranka
2. Ikelti TESTINIUS duomenis — įkelti iš test_data.txt
3. Skaiciuoti               — palyginti viengijį ir daugiagijį
0. Iseiti
```

Rekomenduojama: pasirinkti `2`, tada `3`.

## Kaip veikia matavimas

1. **Apšilimas (warm-up)** — prieš matavimus programa atleidžia vieną skaičiavimo ciklą, kad CPU cache ir branch predictor būtų paruošti. Tai užtikrina, kad visi matavimai vyksta vienodomis sąlygomis.
2. **Viengijis skaičiavimas** — visi kroviniai skaičiuojami nuosekliai viename gije.
3. **Daugiagijis skaičiavimas** — duomenys padalijami lygiomis dalimis tarp gijų. Gijų skaičius parenkamas automatiškai pagal procesoriaus branduolių kiekį.
4. **Rezultatų validacija** — programa patikrina, ar visų metodų gautos sumos sutampa.

## Rezultatų pavyzdys

```
[Viengijis]
  Bendra kaina : 1234567890
  Laikas       : 312.45 ms

[Daugiagijis palyginimas]
  Gijos  | Laikas (ms)  | Pagreitis | Bendra kaina
  -------|--------------|-----------|-------------
      2  |       178.30  |     1.752x  |   1234567890
      4  |        95.12  |     3.285x  |   1234567890
      6  |        67.43  |     4.634x  |   1234567890
     12  |        41.20  |     7.584x  |   1234567890

[Isvada]
  >> Geriausias variantas: 12 gijos (7.584x pagreitis).
  [OK] Visi metodai grazina vienoda suma - skaiciavimai teisingi.
```
