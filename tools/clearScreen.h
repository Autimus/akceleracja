#pragma once
#include <cstdlib>

// Funkcja czyszcząca terminal i linux i windows
inline void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
