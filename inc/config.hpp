#ifndef K_CONFIG_DOLORES
#define K_CONFIG_DOLORES

#include <iostream>
#include <std-k.hpp>

const std::string GlowStylesheet = "glowstyle";
const std::string ChatArchiveDir = HOME + "/.local/share/dolores/";
const std::string model = "gpt-4o-mini";

const std::string ProgramName = "dolores";
const std::string Version = "0.0.0";
const std::string UsageNotes = R"(usage: dolores [ -h/-v ]
options:
    -h / --help         show help and usage notes
    -v / --version      print version and exit)";

// Environment variable to store the OpenAI API key
#define OPENAI_API_KEY_ENV_VAR "OPENAIMainKey"

#define LINEWIDTHCONSTRAINT 28
// Adds empty text so that component takes up all space
#define SPACE " "

// role defenitions
#define USER "user"
#define ASSISTANT "assistant"

#define NAMECONTENTPREFIX "maximum 25 char name for this whole message thread (no quotes)"

#define UIDRANGE 1000000000LL, 9999999999LL

#define SYSTEMCONTENT "You are Dolores, an AI assistant focused on code help, primarily in C++, with occasional support for JS and Python. Default to C++ if not specified. Max, your owner, uses a custom DWM UI on Arch Linux and prefers open-source, DIY computing"

#endif

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

