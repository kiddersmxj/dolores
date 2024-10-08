#include "../inc/tokeniser.hpp"

int GetTokens(std::string user_content, std::string assistant_reply) {
    int total_input_tokens = 0;
    int total_output_tokens = 0;
    // Tokenize the input text
    std::vector<std::string> input_tokens = tokenize(user_content);
    int input_token_count = countTokens(input_tokens);
    total_input_tokens += input_token_count;
    /* std::cout << "Input token count: " << input_token_count << std::endl; */
    /* std::cout << "Total input tokens: " << total_input_tokens << std::endl; */
    // Tokenize the response text
    std::vector<std::string> output_tokens = tokenize(assistant_reply);
    int output_token_count = countTokens(output_tokens);
    total_output_tokens += output_token_count;
    /* std::cout << "Output token count: " << output_token_count << std::endl; */
    /* std::cout << "Total output tokens: " << total_output_tokens << std::endl; */
    return total_output_tokens + total_input_tokens;
}

// A function to split a string into tokens
std::vector<std::string> tokenize(const std::string &text) {
    std::vector<std::string> tokens;
    std::regex wordRegex(R"(\w+|[^\s\w]+)");  // Regular expression to match words and punctuation
    auto words_begin = std::sregex_iterator(text.begin(), text.end(), wordRegex);
    auto words_end = std::sregex_iterator();

    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        tokens.push_back(match.str());
    }

    return tokens;
}

int countTokens(const std::vector<std::string>& tokens) {
    int token_count = 0;
    for (const auto& token : tokens) {
        token_count += token.length();
    }
    return token_count;
}

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

