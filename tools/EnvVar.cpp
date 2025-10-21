#include <fstream>
#include <utility>
#include <cctype>
#include "EnvVar.h"

EnvVar::EnvVar(){
    if (!std::filesystem::exists(runningDir/".env")) {
        createEnvFile();
    }
    std::ifstream file (runningDir/".env");
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#')
            continue;
        auto pos = line.find('=');
        if (pos == std::string::npos)
            continue;
        variables.emplace_back(to_lower(trim(line.substr(0, pos))), to_lower(trim(line.substr(pos + 1))));
    }
}
void EnvVar::writeToEnvFile() {
    std::ofstream file (runningDir/".env",std::ios::out);
    for (auto&[first, second] : variables) {
        file << first << "=" << second << "\n";
    }
}
void EnvVar::createEnvFile() {
    constexpr int size = 9;
    const auto* defNames = new std::string[size] {
        "skipQuestions", "visualize", "randomStart", "howManyRandoms","columns", "rows", "simulationSpeed","fileName", "algorithmName"
    };
    const auto* defValues = new std::string[size] {
        "0","0","0","0","32","32","1","test.txt", "cpulinear"
    };
    variables.clear();
    for (int i = 0; i < size; i++) {
        variables.emplace_back(defNames[i], defValues[i]);
    }

    delete[] defValues;
    delete[] defNames;

    writeToEnvFile();
}

std::string EnvVar::trim(const std::string &s) {
    const auto start = s.find_first_not_of(" \t\r\n");
    const auto end = s.find_last_not_of(" \t\r\n");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}
std::string EnvVar::to_lower(std::string s) {
    for (auto& c : s)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

void EnvVar::setEnvVar(const std::string& name, const std::string& value) {
    bool found = false;
    for (auto&[first, second] : variables) {
        if (first == name) {
            second = value;
            found = true;
            break;
        }
    }
    if (!found) {
        variables.emplace_back(name, value);
    }

    writeToEnvFile();
}
void EnvVar::setEnvVar(const std::string& name, const int value) {
    setEnvVar(name,std::to_string(value));
}
void EnvVar::setEnvVar(const std::string& name, const float value) {
    setEnvVar(name,std::to_string(value));
}
void EnvVar::setEnvVar(const std::string& name, const bool value) {
    setEnvVar(name,static_cast<int>(value));
}

std::string EnvVar::find(const std::string& name) {
    for (auto&[first, second] : variables) {
        if (first == name) {
            return second;
        }
    }
    return "";
}
int EnvVar::findInt(const std::string& name) {
    return stoi(find(name));
}
float EnvVar::findFloat(const std::string& name) {
    return stof(find(name));
}
bool EnvVar::findBool(const std::string& name) {
    return static_cast<bool>(findInt(name));
}