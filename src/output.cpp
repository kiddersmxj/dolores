#include "../inc/output.hpp"

void outputMarkdownWithGlow(const std::string& markdownText) {
    char command[256];           // Adjust the size of the buffer as needed
    // Format the command string
    sprintf(command, "glow --style=%s", getenv(GlowStylesheet.c_str()));

    // Open a process to run the `glow` command
    FILE* pipe = popen(command, "w");
    // FILE* pipe = popen("glow --style=dark", "w");
    if (!pipe) {
        std::cerr << "Could not start glow.\n";
        return;
    }

    // Write the markdown text to the `glow` process
    fprintf(pipe, "%s", markdownText.c_str());

    // Close the process after writing
    pclose(pipe);
}

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

