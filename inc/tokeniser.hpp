#ifndef K_TOKENISER
#define K_TOKENISER

#include "config.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <regex>

int GetTokens(std::string user_content, std::string assistant_reply);
std::vector<std::string> tokenize(const std::string &text);
int countTokens(const std::vector<std::string>& tokens);

#endif

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

