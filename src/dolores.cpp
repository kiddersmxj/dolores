#include "../inc/dolores.hpp"
#include <deque>

using json = nlohmann::json;

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

int countTokens(const std::vector<std::string>& tokens) {
    int token_count = 0;
    for (const auto& token : tokens) {
        token_count += token.length();
    }
    return token_count;
}

int main(int argc, char** argv) {
    int HelpFlag = 0;
    int VersionFlag = 0;
    int opt;

    struct option Opts[] = {
        { "help", no_argument, &HelpFlag, 1 },
        { "version", no_argument, &VersionFlag, 1 },
        { 0 }
    };

    while (1) {
        opt = getopt_long(argc, argv, "hv", Opts, 0);

        if (opt == -1) {
            if (HelpFlag && VersionFlag) {
                Usage();
                return EXIT_FAILURE;
            }
            break;
        }

        switch (opt) {
        case 'h':
            HelpFlag = 1;
            break;
        case 'v':
            VersionFlag = 1;
            break;
        case '?':
            Usage();
            return EXIT_FAILURE;
        default:
            Usage();
            break;
        }
    }

    if (HelpFlag) {
        Usage();
        return EXIT_SUCCESS;
    }
    if (VersionFlag) {
        PrintVersion();
        return EXIT_SUCCESS;
    }

    // Get the API key from the environment variable
    const char* api_key = std::getenv(OPENAI_API_KEY_ENV_VAR);
    if (!api_key) {
        std::cerr << "Error: " << OPENAI_API_KEY_ENV_VAR << " environment variable not set." << std::endl;
        return 1;
    }

    std::string model = "gpt-4o-mini";
    std::string system_content = SYSTEMCONTENT;
    std::string user_content;

    // Message history to maintain the context
    std::deque<json> message_history;
    int total_input_tokens = 0;
    int total_output_tokens = 0;
    
    // Add the initial system message to the history
    message_history.push_back({
        {"role", "system"},
        {"content", system_content}
    });

    // Interactive loop for repeated user input
    while (true) {
        std::cout << "Enter your prompt (or type 'exit' to quit): ";
        std::getline(std::cin, user_content);

        if (user_content == "exit") {
            break;
        }

        if (user_content.empty()) {
            std::cerr << "Error: No input provided." << std::endl;
            continue;
        }

        // Tokenize the input text
        std::vector<std::string> input_tokens = tokenize(user_content);
        int input_token_count = countTokens(input_tokens);
        total_input_tokens += input_token_count;
        std::cout << "Input token count: " << input_token_count << std::endl;
        std::cout << "Total input tokens: " << total_input_tokens << std::endl;

        // Add user's message to the history
        message_history.push_back({
            {"role", "user"},
            {"content", user_content}
        });

        // Prepare the request payload with the message history
        json request_payload = {
            {"model", model},
            {"messages", message_history}
        };

        // Send the request and get the response
        std::string response = sendOpenAIRequest(api_key, request_payload.dump());

        // Parse the response and get the assistant's reply
        std::string assistant_reply = ParseResponse(response);
        std::cout << assistant_reply << std::endl;

        if (!assistant_reply.empty()) {
            // Add assistant's reply to the history
            message_history.push_back({
                {"role", "assistant"},
                {"content", assistant_reply}
            });

            // Tokenize the response text
            std::vector<std::string> output_tokens = tokenize(assistant_reply);
            int output_token_count = countTokens(output_tokens);
            total_output_tokens += output_token_count;
            std::cout << "Output token count: " << output_token_count << std::endl;
            std::cout << "Total output tokens: " << total_output_tokens << std::endl;
            std::cout << "Total tokens: " << (total_input_tokens + total_output_tokens) << std::endl;

            // Output the assistant's response with Markdown formatting
            outputMarkdownWithGlow(assistant_reply);
        }
    }

    return 0;
}

void Usage() {
    std::cout << UsageNotes << std::endl;
}

void Usage(std::string Message) {
    std::cout << Message << std::endl;
    std::cout << UsageNotes << std::endl;
}

void PrintVersion() {
    std::cout << ProgramName << ": version " << Version << std::endl;
}

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.
