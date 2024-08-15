#include "config.hpp"
#include <nlohmann/json.hpp>
#include <sys/stat.h> // For mkdir
#include <filesystem>
#include <fstream>
#include <random>

using json = nlohmann::json;

class Database {
    public:
        Database();
        ~Database();
        void SaveFile(const json& request_payload, const std::string& dir, const std::string& uid, std::string Name);
        std::string generateUID();
    private:
        std::vector<std::string> jsonFiles;
        std::vector<std::string> jsonNames;
};

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

