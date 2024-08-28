#ifndef K_SHORTS_DOLORES
#define K_SHORTS_DOLORES

#include "config.hpp"

#include <iostream>
#include <filesystem>
#include <string>
#include <regex>
#include <set>
#include <sstream>
#include <iomanip>
#include <fstream>

class Shorts {
    public:
        Shorts(std::string ShortsDir);
        Shorts(const std::string& directory, std::string &name);
        ~Shorts();
        // Checks free numbers for the code type and returns the lowest
        std::string Initialise(std::string prefix);
        bool Initialised();
        void AddLine(std::string Line);
        void End();
        std::string Return();
    private:
        std::string Prefix;
        std::string Suffix;
        std::string ShortsDir;
        bool InitialisedBool = false;
        std::string filePath;
        std::deque<std::string> Lines;
        std::string findFile(const std::string& directory, std::string &name);
        std::string readFile();
};

#endif

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

