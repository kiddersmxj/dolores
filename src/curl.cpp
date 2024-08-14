#include "../inc/curl.hpp"
#include <curl/curl.h>

// Callback function to handle the response data
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to send a request to the OpenAI API
std::string sendOpenAIRequest(const std::string& api_key, const std::string& json_payload) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");

        // Set the headers
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        std::string auth_header = "Authorization: Bearer " + api_key;
        headers = curl_slist_append(headers, auth_header.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the POST data
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload.c_str());

        // Set the callback function to capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request
        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;

        // Cleanup
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    return readBuffer;
}

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

