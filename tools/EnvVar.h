#pragma once

#include <vector>
#include <filesystem>

//Prosta implementacja zmiennych środowiskowych w .env i ich wczytywania.
class EnvVar {
    protected:
        std::filesystem::path runningDir  = std::filesystem::current_path().parent_path();
        std::vector<std::pair<std::string,std::string>> variables;
    public:
        EnvVar();
        void writeToEnvFile();
        void createEnvFile();
        static std::string trim(const std::string &s);
        static std::string to_lower(std::string s);
        void setEnvVar(const std::string& name, const std::string& value);
        void setEnvVar(const std::string& name, int value);
        void setEnvVar(const std::string& name, float value);
        void setEnvVar(const std::string& name, bool value);
        std::string find(const std::string& name);
        int findInt(const std::string& name);
        bool findBool(const std::string& name);
        float findFloat(const std::string& name);
        void print();
};

