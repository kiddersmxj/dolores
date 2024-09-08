#include "config.hpp"
#include <nlohmann/json.hpp>
#include <sys/stat.h> // For mkdir
#include <filesystem>
#include <fstream>
#include <random>

namespace fs = std::filesystem;
using json = nlohmann::json;

class Database {
    public:
        Database();
        ~Database();
        void Get();
        void SaveFile(const json& request_payload, const std::string& dir, const std::string& uid, std::string Name, bool Stared);
        void SaveFile(const json& request_payload, const std::string& dir, std::string Name, bool Stared);
        std::string generateUID();
        std::vector<std::string> GetFileNames();
        std::vector<std::string> GetNames();
        // json ReadFile(std::string Name);
        void DeleteFile(int Index);
        json ReadFile(int Index);
    private:
        std::vector<std::string> jsonFileNames;
        std::vector<std::string> jsonNames;

    struct FileData {
        fs::path filePath;
        std::time_t lastModified;
    };

    static bool compareByModifiedTime(const FileData& a, const FileData& b) {
        return a.lastModified > b.lastModified;
    }
};

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

