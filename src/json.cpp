#include "../inc/json.hpp"

Json::Json(std::string system_content, std::string api_key) : api_key(api_key) {
    // Add the initial system message to the history
    messages.push_back({
        {"role", "system"},
        {"content", system_content}
    });
    messagePairs = parseMessages(messages);
}

Json::Json(json messages, std::string api_key) : api_key(api_key), messages(messages) {
    messagePairs = parseMessages(messages);
}

Json::~Json() {
}

std::deque<Json::MessagePair> Json::parseMessages(const json& j) {
    std::deque<MessagePair> messagePairs;
    std::string current_user_message;
    std::string current_assistant_message;

    for (const auto& item : j) {
        std::string role = item.at("role").get<std::string>();
        std::string content = item.at("content").get<std::string>();

        if (role == USER) {
            if (!current_user_message.empty() && !current_assistant_message.empty()) {
                messagePairs.push_back({current_user_message, current_assistant_message});
                current_user_message.clear();
                current_assistant_message.clear();
            }
            current_user_message = content;
        } else if (role == ASSISTANT) {
            current_assistant_message = content;
        }
    }

    // Add the last pair if available
    if (!current_user_message.empty() && !current_assistant_message.empty()) {
        messagePairs.push_back({current_user_message, current_assistant_message});
    }

    return messagePairs;
}

std::deque<std::string> Json::GetUserMessages() {
    std::deque<std::string> UserMessages;
    for (const auto& pair : messagePairs) {
        UserMessages.push_back(pair.user_message);
    }
    return UserMessages;
}

std::deque<std::string> Json::GetAssistantMessages() {
    std::deque<std::string> AssistantMessages;
    for (const auto& pair : messagePairs) {
        AssistantMessages.push_back(pair.assistant_message);
    }
    return AssistantMessages;
}

std::string Json::GetMessagePairString() {
    std::stringstream ss;
    for (const auto& pair : messagePairs) {
        ss << pair.user_message << "\n";
        ss << pair.assistant_message << "\n";
    }
    return ss.str();
}

void Json::Add(std::string user_content, std::string role) {
    // Add user's message to the history
    messages.push_back({
        {"role", role},
        {"content", user_content}
    });
}

std::string Json::Send() {
    // Prepare the request payload with the message history
    json request_payload = {
        {"model", model},
        {"messages", messages}
    };

    // Send the request and get the response
    return sendOpenAIRequest(api_key, request_payload.dump());
}

json Json::GetRequest() {
    json request_payload = {
        {"model", model},
        {"messages", messages}
    };
    return request_payload;
}

json Json::GetMessages() {
    return messages;
}

std::string Json::Name() {
    // Create the NameRequest
    json NameRequest = {
        {"model", model},
        {"messages", messages}
    };

    // Add the user message
    NameRequest["messages"].push_back({
        {"role", "user"},
        {"content", NAMECONTENTPREFIX}
    });
    // std::cout << NameRequest.dump(4) << std::endl;
    return ParseResponse(sendOpenAIRequest(api_key, NameRequest.dump()));
}

// Function to parse the JSON response and return the assistant's response content
std::string Json::ParseResponse(const std::string& response) {
    try {
        json jsonResponse = json::parse(response);

        if (jsonResponse.contains("choices") && !jsonResponse["choices"].is_null() &&
            jsonResponse["choices"].size() > 0 && jsonResponse["choices"][0].contains("message") &&
            !jsonResponse["choices"][0]["message"].is_null()) {

            std::string assistantContent = jsonResponse["choices"][0]["message"]["content"].get<std::string>();
            return assistantContent;
        } else {
            std::cerr << "Error: The expected JSON structure is not present." << std::endl;
            return "";
        }
    } catch (const json::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        return "";
    }
}

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

