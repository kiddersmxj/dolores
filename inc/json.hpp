#include "config.hpp"
#include "curl.hpp"
#include <deque>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Json {
    public:
        Json();
        ~Json();
    private:
};

std::string ParseResponse(const std::string& response);
std::string GetName(std::deque<json> message_history, std::string model, \
        std::string system_content, std::string user_content, std::string api_key);

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

