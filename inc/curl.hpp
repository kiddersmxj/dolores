#ifndef K_CURL
#define K_CURL

#include "config.hpp"

#include <string>

// Callback function to handle the response data
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

// Function to send a request to the OpenAI API
std::string sendOpenAIRequest(const std::string& api_key, const std::string& json_payload);



#endif

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

