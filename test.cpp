#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

int main() {
    std::string input = "test.txt";
    std::string path = "configFiles/" + input;

    std::cout << "Ścieżka pliku: " << path << "\n";
    std::cout << "Katalog roboczy: " << std::filesystem::current_path() << "\n";

    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "❌ Nie mogę otworzyć pliku!\n";
        if (!std::filesystem::exists(path))
            std::cerr << "Plik NIE istnieje według std::filesystem!\n";
        return 1;
    }

    file.seekg(0, std::ifstream::end);
    auto pos = file.tellg();
    file.seekg(0, std::ifstream::beg);

    std::cout << "✅ Długość pliku: " << pos << " bajtów\n";
}
