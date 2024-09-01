#include "../inc/mode.hpp"

Mode::Mode() {
    mode = "Normal";
}

Mode::~Mode() {
}

void Mode::Command() {
    mode = "Command";
}

void Mode::Normal() {
    mode = "Normal";
}

void Mode::Input() {
    mode = "Input";
}

void Mode::Open() {
    mode = "Open";
}

bool Mode::IsCommand() {
    if(mode == "Command")
        return true;
    return false;
}

bool Mode::IsNormal() {
    if(mode == "Normal")
        return true;
    return false;
}

bool Mode::IsInput() {
    if(mode == "Input")
        return true;
    return false;
}

bool Mode::IsOpen() {
    if(mode == "Open")
        return true;
    return false;
}

std::string Mode::Get() {
    return mode;
}

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

