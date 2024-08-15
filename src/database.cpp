#include "../inc/database.hpp"

namespace fs = std::filesystem;

Database::Database() {
    // Check if the directory exists, and create it if it doesn't
    try {
        if (!fs::exists(ChatArchiveDir)) {
            if (fs::create_directories(ChatArchiveDir)) {
                std::cout << "Directory created: " << ChatArchiveDir << std::endl;
            } else {
                std::cerr << "Failed to create directory: " << ChatArchiveDir << std::endl;
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }

    // Iterate through the directory and collect .json file names
    try {
        for (const auto& entry : fs::directory_iterator(ChatArchiveDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                jsonFiles.push_back(entry.path().filename().string());
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "General error: " << e.what() << std::endl;
    }
    for(const auto& file: jsonFiles) {
        std::cout << file << std::endl;
    }
}

Database::~Database() {
}

// Function to generate a random 10-digit UID as a string
std::string Database::generateUID() {
    std::random_device rd;  // Seed for the random number engine
    std::mt19937_64 gen(rd()); // Use a 64-bit Mersenne Twister engine
    std::uniform_int_distribution<long long> dis(1000000000LL, 9999999999LL); // Range for 10 digits
    return std::to_string(dis(gen));
}


void Database::SaveFile(const json& request_payload, const std::string& dir, const std::string& uid, std::string Name) {
    // Ensure the directory exists
    struct stat info;
    if (stat(dir.c_str(), &info) != 0) {
        // Directory does not exist, so create it
        if (mkdir(dir.c_str(), 0777) != 0) {
            std::cerr << "Error: Failed to create directory " << dir << std::endl;
            return;
        }
    }

    std::string filename = dir + "/" + uid + ".json";

    json NamedFile = {
        {"Name", Name},
        {"UID", uid},
        {request_payload}
    };

    std::ofstream file(filename); // Open file for writing (will overwrite if it exists)
    if (file.is_open()) {
        file << NamedFile.dump(4); // Pretty-print JSON with an indent of 4 spaces
        file.close();
        std::cout << "Saved conversation to " << filename << std::endl;
    } else {
        std::cerr << "Error: Unable to open file for writing: " << filename << std::endl;
    }
}

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

