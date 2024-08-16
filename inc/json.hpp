#ifndef K_JSON_DOLORES
#define K_JSON_DOLORES

#include "config.hpp"
#include "curl.hpp"
#include <deque>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Json {
    public:
        Json(std::string system_content, std::string api_key);
        Json(json messages, std::string api_key);
        ~Json();
        void Add(std::string user_content, std::string role);
        std::string Send();
        json GetRequest();
        json GetMessages();
        std::string Name();
        std::string ParseResponse(const std::string& response);
        struct MessagePair {
            std::string user_message;
            std::string assistant_message;
        };
        std::deque<MessagePair> parseMessages(const json& j);
        std::string GetMessagePairString();
    private:
        std::string api_key;
        std::deque<json> messages;
        std::deque<Json::MessagePair> messagePairs;
};

std::string GetName(std::deque<json> message_history, std::string model, \
        std::string system_content, std::string user_content, std::string api_key);

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

#endif
