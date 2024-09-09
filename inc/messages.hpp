#ifndef K_MESSAGES_DOLORES
#define K_MESSAGES_DOLORES

#include "config.hpp"
#include "curl.hpp"
#include "shorts.hpp"

#include <deque>
#include <fstream>
#include <regex>
#include <nlohmann/json.hpp>
#include <random>

using json = nlohmann::json;

struct MessageOptions {
    std::string APIKey;
    std::string Model;
    int MaxTokens;
    float Temperature;
    float TopP;

    std::string Name;
    std::string UID;
    bool Stared;
};

class Messages {
    public:
        Messages(std::string system_content, bool NewChat, MessageOptions Options);
        Messages(json messagesjson, MessageOptions Options);
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
        int GetTotalTokens();
        void SetModel(std::string NewModel);
        std::string GetModel();
        void Delete();
        void parseOptions(const json& jconfig, const json& j);
        void ToggleStar();
        bool Stared();
        std::string CreateSeparator(int length, std::string Title);
        std::string EditUserMessage(int MessageIndex);

    private:
        std::vector<std::string> basicTokenize(const std::string& text);
        int countTokens(const std::string& text);
        std::vector<std::string> SplitString(const std::string& str, char delimiter);
        std::string CatchParseCode(std::string Response);
        std::string generateUID();
        json GetConfig();
        MessageOptions Options;
        int Tokens = 0;
        bool NewChat;
        std::deque<json> messages;
        std::deque<Messages::MessagePair> messagePairs;
};

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

#endif
