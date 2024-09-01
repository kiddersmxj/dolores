#include "config.hpp"

class Mode {
    public:
        Mode();
        ~Mode();
        void Command();
        void Normal();
        void Input();
        void Open();
        bool IsCommand();
        bool IsNormal();
        bool IsInput();
        bool IsOpen();
        std::string Get();
    private:
        std::string mode = "Normal";
};

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

