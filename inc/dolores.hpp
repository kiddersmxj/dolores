#ifndef K_DOLORES
#define K_DOLORES

#include "config.hpp"
#include "curl.hpp"
#include "tokeniser.hpp"
#include "output.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <getopt.h>

void Usage();
void Usage(std::string Message);
void PrintVersion();

// Function to parse the JSON response and print the assistant's response content
void parseAndPrintResponse(const std::string& response);


#endif

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

