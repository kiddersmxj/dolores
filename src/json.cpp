#include "../inc/json.hpp"

Json::Json() {
}

Json::~Json() {
}

// Function to parse the JSON response and return the assistant's response content
std::string ParseResponse(const std::string& response) {
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

std::string GetName(std::deque<json> message_history, std::string model, \
        std::string system_content, std::string user_content, std::string api_key) {
    // Create the NameRequest
    json NameRequest = {
        {"model", model},
        {"messages", json::array()}
    };

    // Add the system message
    NameRequest["messages"].push_back({
        {"role", "system"},
        {"content", system_content}
    });

    // Add each message from the existing message history
    for (const auto& message : message_history) {
        NameRequest["messages"].push_back(message);
    }

    // Add the user message
    NameRequest["messages"].push_back({
        {"role", "user"},
        {"content", NAMECONTENTPREFIX + user_content}
    });
    /* std::cout << NameRequest.dump(4) << std::endl; */
    return ParseResponse(sendOpenAIRequest(api_key, NameRequest.dump()));
}


// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

