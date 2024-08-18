#ifndef K_MESSAGES_DOLORES
#define K_MESSAGES_DOLORES

#include "config.hpp"
#include "curl.hpp"
#include <deque>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Messages {
    public:
        Messages(std::string system_content, std::string api_key, bool NewChat);
        Messages(json messages, std::string api_key);
        ~Messages();
        void Add(std::string user_content, std::string role);
        std::string Send();
        json GetRequest();
        json GetMessages();
        std::string MakeName();
        std::string ParseResponse(const std::string& response);
        struct MessagePair {
            std::string user_message;
            std::string assistant_message;
        };
        std::deque<MessagePair> parseMessages(const json& j);
        std::string GetMessagePairString();
        std::deque<std::string> GetUserMessages();
        std::deque<std::string> GetAssistantMessages();
    private:
        bool NewChat;
        std::string api_key;
        std::deque<json> messages;
        std::deque<Messages::MessagePair> messagePairs;
};

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

#endif
