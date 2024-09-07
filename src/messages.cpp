#include "../inc/messages.hpp"

void prependToDebugFile(const std::string& text) {
    // Read the existing content of the file
    std::ifstream debugFileIn("debug");
    std::stringstream buffer;
    buffer << debugFileIn.rdbuf();
    std::string oldContent = buffer.str();
    debugFileIn.close();

    // Open the file in truncate mode to overwrite it
    std::ofstream debugFileOut("debug", std::ios::trunc);

    // Check if the file is open
    if (debugFileOut.is_open()) {
        // Write the new text followed by the old content
        debugFileOut << text << std::endl << oldContent;

        // Close the file
        debugFileOut.close();
    } else {
        std::cerr << "Unable to open the file 'debug'" << std::endl;
    }
}


void appenddebugfile(const std::string& text) {
    // Open the file in append mode (std::ios::app)
    std::ofstream debugFile("debug", std::ios::app);

    // Check if the file is open
    if (debugFile.is_open()) {
        // Write the string to the file
        debugFile << text << std::endl;

        // Close the file
        debugFile.close();
    } else {
        std::cerr << "Unable to open the file 'debug'" << std::endl;
    }
}

Messages::Messages(std::string system_content, bool NewChat, MessageOptions Options) 
    : Options(Options), NewChat(NewChat) {
    messages.push_back({
        {"role", "system"},
        {"content", system_content},
    });

    // Create the final JSON object with the "messages" key
    json j;
    j["messages"] = json::array();  // Create an empty JSON array for messages

    // Insert deque elements into the messages array
    for (const auto& message : messages) {
        j["messages"].push_back(message);  // Push each message into the messages array
    }

    prependToDebugFile(j.dump(4));
    prependToDebugFile("pre");

    messagePairs = parseMessages(j);
}

Messages::Messages(json messagesjson, MessageOptions Options) : Options(Options) {

    messages = messagesjson["messages"];

    parseOptions(messagesjson);
    messagePairs = parseMessages(messagesjson);
}

Messages::~Messages() {
}

void Messages::parseOptions(const json& j) {
    if (j.contains("model")) {
        Options.Model = j.at("model").get<std::string>();
    }
    if (j.contains("max_tokens")) {
        Options.MaxTokens = j.at("max_tokens").get<int>();
    }
    if (j.contains("temperature")) {
        Options.Temperature = j.at("temperature").get<double>();
    }
    if (j.contains("top_p")) {
        Options.TopP = j.at("top_p").get<double>();
    }
}

std::deque<Messages::MessagePair> Messages::parseMessages(const json& j) {

    prependToDebugFile(j.dump(4));
    prependToDebugFile("inparse");
    std::deque<MessagePair> messagePairs;
    std::string current_user_message;
    std::string current_assistant_message;

    for (const auto& item : j["messages"]) {
        std::string role = item.at("role").get<std::string>();
        std::string content = item.at("content").get<std::string>();

        if (role == USER) {
            if (!current_user_message.empty()) {
                // If there's an existing user message without an assistant response, add it with an empty assistant message
                messagePairs.push_back({current_user_message, ""});
                current_user_message.clear();

                Tokens = Tokens + countTokens(current_user_message);
            }
            current_user_message = content;
        } else if (role == ASSISTANT) {
            current_assistant_message = content;

            Tokens = Tokens + countTokens(current_assistant_message);

            // Add the pair immediately after an assistant message
            if (!current_user_message.empty()) {
                messagePairs.push_back({current_user_message, current_assistant_message});
                current_user_message.clear();
                current_assistant_message.clear();

            }
        }
    }

    // Add the last user message if available, even if there's no assistant response --- important
    if (!current_user_message.empty()) {
        messagePairs.push_back({current_user_message, ""});
    }

    // You can use the model, max_tokens, and temperature variables as needed here
    // For example, you can print them or store them in a class member variable

//     for(auto messagePair: messagePairs) {
//         // Add user's message to the history
//         messages.push_back({
//             {"role", USER},
//             {"content", messagePair.user_message}
//         });
//         if(!messagePair.assistant_message.empty()) {
//             messages.push_back({
//                 {"role", ASSISTANT},
//                 {"content", messagePair.assistant_message}
//             });
//         }
//     }

    return messagePairs;
}

std::deque<std::string> Messages::GetUserMessages() {
    std::deque<std::string> UserMessages;
    for (const auto& pair : messagePairs) {
        UserMessages.push_back(pair.user_message);
    }
    return UserMessages;
}

std::deque<std::string> Messages::GetAssistantMessages() {
    std::deque<std::string> AssistantMessages;
    for (const auto& pair : messagePairs) {
        AssistantMessages.push_back(pair.assistant_message);
    }
    return AssistantMessages;
}

std::string Messages::GetMessagePairString() {
    std::stringstream ss;
    for (const auto& pair : messagePairs) {
        ss << pair.user_message << "\n";
        ss << pair.assistant_message << "\n";
    }
    return ss.str();
}

void Messages::Add(std::string user_content, std::string role) {
    for (const auto& message : messages) {
        prependToDebugFile(message.dump(4));
    }
    prependToDebugFile("before");

    // Add user's message to the history
    messages.push_back({
        {"role", role},
        {"content", user_content}
    });

    // Create the final JSON object with the "messages" key
    json j;
    j["messages"] = json::array();  // Create an empty JSON array for messages

    // Insert deque elements into the messages array
    for (const auto& message : messages) {
        j["messages"].push_back(message);  // Push each message into the messages array
        prependToDebugFile(message.dump(4));
    }
    prependToDebugFile("after");

    prependToDebugFile("pre0");
    prependToDebugFile(j.dump(4));
    prependToDebugFile("pre");

    messagePairs = parseMessages(j);
}

std::string Messages::Send() {
    // Prepare the request payload with the message history
    json request_payload = GetRequest();

    // Send the request and get the response
    return sendOpenAIRequest(Options.APIKey, request_payload.dump());
}

json Messages::GetRequest() {
    json request_payload;
    if(Options.MaxTokens > 0) {
        request_payload = {
            {"model", Options.Model},
            {"messages", messages},
            {"max_tokens", Options.MaxTokens},
            {"temperature", 0.4}
        };
    } else {
        request_payload = {
            {"model", Options.Model},
            {"messages", messages},
            {"temperature", 0.4}
        };
    }

    return request_payload;
}

json Messages::GetMessages() {
    return messages;
}

std::string Messages::MakeName() {
    // Create the NameRequest
    json NameRequest = {
        {"model", "gpt-4o-mini"},
        {"messages", messages},
        {"max_tokens", 4},
        {"temperature", 0.3}
    };

    // Add the user message
    NameRequest["messages"].push_back({
        {"role", "user"},
        {"content", NAMECONTENTPREFIX}
    });
    return ParseResponse(sendOpenAIRequest(Options.APIKey, NameRequest.dump()));
}

std::string Messages::CatchParseCode(std::string Response) {
    auto lines = SplitString(Response, '\n');
    std::deque<std::string> Output;

    std::string codeLanguage;

    std::regex codeBlockRegex(R"(\s*```\s*(\w+))");  // Regex to match lines starting with optional spaces followed by ```
    std::regex inlineCodeRegex("`([^`]*)`");    // Regex to match inline code sections
    std::regex CodeEndRegex(R"(\s*```+\s*)");

    std::string Short = "";
    std::smatch Search;
    Shorts Shorts(ShortsDir);

    bool InCode = false;

    for (const auto& line : lines) {
        if (std::regex_match(line, Search, codeBlockRegex)) {
            if(!InCode) {
                codeLanguage = Search[1];
                if(codeLanguage.empty()) {
                    codeLanguage = "unk";
                }
                std::string num = Shorts.Initialise(codeLanguage);
                Output.push_back("```" + codeLanguage + num);
                InCode = !InCode;
            } else {
                Output.push_back(line);
            }
        } else if (std::regex_match(line, Search, CodeEndRegex) && InCode) {
            Shorts.End();
            InCode = !InCode;
            Output.push_back(line);
        } else if(InCode) {
            Shorts.AddLine(line);
            Output.push_back(line);
        } else
            Output.push_back(line);
    }
    std::string OutputString;
    for (const auto& line : Output) {
        OutputString += line + "\n";  // Append each string from the deque
    }
    return OutputString;
}

// Function to parse the JSON response and return the assistant's response content
std::string Messages::ParseResponse(const std::string& response) {
    try {
        json jsonResponse = json::parse(response);

        if (jsonResponse.contains("choices") && !jsonResponse["choices"].is_null() &&
            jsonResponse["choices"].size() > 0 && jsonResponse["choices"][0].contains("message") &&
            !jsonResponse["choices"][0]["message"].is_null()) {

            std::string assistantContent = jsonResponse["choices"][0]["message"]["content"].get<std::string>();
            assistantContent = CatchParseCode(assistantContent);
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

int Messages::GetTotalTokens() {
    return Tokens;
}

std::vector<std::string> Messages::basicTokenize(const std::string& text) {
    std::regex token_regex("\\w+|\\s+|[^\\w\\s]");
    auto tokens_begin = std::sregex_iterator(text.begin(), text.end(), token_regex);
    auto tokens_end = std::sregex_iterator();

    std::vector<std::string> tokens;
    for (std::sregex_iterator i = tokens_begin; i != tokens_end; ++i) {
        tokens.push_back((*i).str());
    }

    return tokens;
}

int Messages::countTokens(const std::string& text) {
    auto tokens = SplitString(text, ' ');
    int count = tokens.size();

    return count;
}

std::vector<std::string> Messages::SplitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void Messages::SetModel(std::string NewModel) {
    Options.Model = NewModel;
}

std::string Messages::GetModel() {
    return Options.Model;
}

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

