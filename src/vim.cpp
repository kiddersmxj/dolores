#include "../inc/vim.hpp"

Vim::Vim() {
    uid = "1234";
    filename = "/tmp/vim_tmpfile_" + uid + ".txt";
}

Vim::Vim(std::string File) : filename(File) {
    OpenVim();
}

Vim::~Vim() {
}

int Vim::OpenVim() {
    return k::ExecCmdOrphan("st -e vim " + filename);
}

std::string Vim::GetVimContent() {
    std::ifstream file(filename);

    if (!file.is_open()) {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();  // Read the file content into the stringstream
    file.close();

    return buffer.str();  // Return the content as a string
}

bool Vim::RemoveVimFile() {
    if (std::remove(filename.c_str()) != 0) {
        // If remove fails, return false
        return false;
    } else {
        // If remove succeeds, return true
        return true;
    }
}

void Vim::SetContent(std::string Content) {
    k::ExecCmd("echo \"" + Content + "\" > " + filename);
}

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

