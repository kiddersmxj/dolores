#include "../inc/shorts.hpp"

void PrependToDebugFile(const std::string& text) {
    // Read the existing content of the file
    std::ifstream debugFileIn("debug");
    std::stringstream buffer;
    buffer << debugFileIn.rdbuf();
    std::string oldContent = buffer.str();
    debugFileIn.close();

    // Open the file in truncate mode to overwrite it
    std::ofstream debugFileOut("debug", std::ios::trunc);

    // Check if the file is open
    if (debugFileOut.is_open()) {
        // Write the new text followed by the old content
        debugFileOut << text << std::endl << oldContent;

        // Close the file
        debugFileOut.close();
    } else {
        std::cerr << "Unable to open the file 'debug'" << std::endl;
    }
}


Shorts::Shorts(std::string ShortsDir) : ShortsDir(ShortsDir) {
}

Shorts::~Shorts() {
}

std::string Shorts::Initialise(std::string prefix) {
    Prefix = prefix;
    InitialisedBool = true;
    std::set<int> numbers;
    std::regex pattern(prefix + R"((\d+))");
    std::smatch match;

    // Iterate through the directory to get the filenames
    for (const auto& entry : std::filesystem::directory_iterator(ShortsDir)) {
        std::string filename = entry.path().filename().string();
        std::string stem = entry.path().stem().string(); // Get filename without extension

        // Check if the filename matches the prefix followed by a number
        if (std::regex_search(stem, match, pattern)) {
            int number = std::stoi(match[1].str());
            if (number >= 1 && number <= 999) {
                numbers.insert(number);
            }
        }
    }

    // Find the smallest missing number
    int nextNumber = 1;
    for (int num : numbers) {
        if (num != nextNumber) {
            break;
        }
        nextNumber++;
    }

    // Return the next available number as a string
    std::ostringstream oss;
    oss << nextNumber;
    Suffix = oss.str();
    return oss.str();
}

bool Shorts::Initialised() {
    return InitialisedBool;
}

void Shorts::AddLine(std::string Line) {
    Lines.push_back(Line);
}

void Shorts::End() {
    // Construct the full file path
    std::string filePath = ShortsDir + "/" + Prefix + Suffix;

    // Open the file for writing
    std::ofstream outFile(filePath);
    
    // Check if the file was opened successfully
    if (!outFile) {
        std::cerr << "Error: Could not open file " << filePath << " for writing." << std::endl;
        return;
    }

    // Write each line from the deque to the file
    for (const auto& line : Lines) {
        outFile << line << std::endl;
    }

    // Close the file
    outFile.close();

    // Optionally, confirm the file was written
    if (outFile.fail()) {
        PrependToDebugFile("Error: Failed to write to file " + filePath);
        // std::cerr << "Error: Failed to write to file " << filePath << "." << std::endl;
    } else {
        PrependToDebugFile("Successfully wrote to file " + filePath);
        // std::cout << "Successfully wrote to file " << filePath << "." << std::endl;
    }

    Prefix = "";
    Suffix = "";
    InitialisedBool = false;
    Lines.clear();
}


// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

