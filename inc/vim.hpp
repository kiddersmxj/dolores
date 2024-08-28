#include "config.hpp"

#include <iostream>
#include <fstream>

class Vim {
    public:
        Vim();
        Vim(std::string File);
        ~Vim();
        std::string GetVimContent();
        bool RemoveVimFile();
        int OpenVim();
    private:
        std::string uid;
        std::string filename;
};

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

