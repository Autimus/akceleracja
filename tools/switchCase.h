#pragma once

#include <string>
#include <iostream>

// Funkcja usystematyzowujaca instrukcje Switch-Case.
// Pierwszy element = komunikat na poczatku, Pozostale elementy to opcje do wyboru. Dlugosc = dlugosc podanej tablicy.
// Zwraca numer wybranej opcji jako int tzn. Pierwsza opcja = 1.
inline int switchCase(const std::string* daneDoInstruckji, int dlugosc) {
    std::string input;
    int output = -1;
    bool pass = false;
    char* opcjeDoWyboru = new char[dlugosc - 1];
    do {
        std::cout << daneDoInstruckji[0] << ":\n"; // Wypisywanie sformatowanych danych w terminalu
        for (int i = 1; i < dlugosc; i++) {
            std::cout << "\n\t" << (i) << ". " << daneDoInstruckji[i];
            opcjeDoWyboru[i - 1] = daneDoInstruckji[i][0];
        }
        std::cout << "\n";
        std::cin >> input;
        for (int i = 1; i < dlugosc; i++) {
            if (input == std::to_string(i) || input[0] == tolower(opcjeDoWyboru[i - 1]) || input[0] == toupper(opcjeDoWyboru[i - 1])) {
                // Dopuszczalny jest numer opcji albo jego pierwsza litera (w przypadku calego wyrazu i tak zczyta tylko pierwsza litere)
                output = i;
                pass = true;
                break;
            }
        }
        if (!pass)
            std::cout << "Podano zla opcje, sprobuj ponownie:\n\n";
    } while (!pass);
    delete[] daneDoInstruckji;
    delete[] opcjeDoWyboru;
    return output; // Zwraca numer wybranej opcji
}