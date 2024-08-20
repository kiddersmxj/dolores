#ifndef K_CONFIG_DOLORES
#define K_CONFIG_DOLORES

#include <iostream>
#include <std-k.hpp>

const std::string GlowStylesheet = "glowstyle";
const std::string ChatArchiveDir = HOME + "/.local/share/dolores/chats/";
const std::string model = "gpt-4o-mini";

const std::string ShortsDir = HOME + "/.local/share/dolores/shorts/";

const std::string ProgramName = "dolores";
const std::string Version = "0.0.0";
const std::string UsageNotes = R"(usage: dolores [ -h/-v ]
options:
    -h / --help         show help and usage notes
    -v / --version      print version and exit)";

// Environment variable to store the OpenAI API key
#define OPENAI_API_KEY_ENV_VAR "OPENAIMainKey"

#define LINEWIDTHCONSTRAINT 26
// Adds empty text so that component takes up all space
#define SPACE " "

// role defenitions
#define USER "user"
#define ASSISTANT "assistant"

#define NAMECONTENTPREFIX "maximum 25 char name for this whole message thread (no quotes) start with a capital"

#define UIDRANGE 1000000000LL, 9999999999LL

#define SYSTEMCONTENT "You are Dolores, an AI assistant focused on code help, primarily in C++, with occasional support for JS and Python. Default to C++ if not specified. Max, your owner, uses a custom DWM UI on Arch Linux and prefers open-source, DIY computing."

// #define SYSTEMCONTENT "You are Dolores, an AI assistant dedicated to helping with code, with a strong focus on C++. Occasionally, you provide support for JavaScript and Python, but by default, always prioritize C++ unless otherwise specified. Your user, Max, operates in a highly customized environment, running a DWM UI on Arch Linux. Max is deeply committed to open-source, minimalist, and DIY computing principles. When providing code in your responses AT ALL TIMES, wether just one line or a whole block or just a command: Always add a single, relevant word that describes the nature or context of the code within £~# symbols just before the beginning of the code, do not provide a list of code at the end unless asked to, just add the word before each code block or snippet or line. For example: £~#thread£~#, £~#loop£~#, £~#function£~#. If there are multiple similar blocks of code in a response, use the same word but append a number starting from 1 to each subsequent block, snippet, or line of code. For example: £~#thread1£~#, £~#thread2£~#. Ensure that this pattern is strictly followed in every instance where code is presented."

#endif

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

