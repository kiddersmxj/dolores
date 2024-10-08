#include "../inc/dolores.hpp"
#include <deque>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <iostream>
#include <sys/stat.h> // For mkdir

using json = nlohmann::json;

std::string GetInput() {
    std::string user_content;
    while(user_content.empty()) {
        std::cout << "Enter your prompt (or type 'exit' to quit): ";
        std::getline(std::cin, user_content);
        if (user_content == "exit")
            break;
        if (user_content.empty()) {
            std::cerr << "Error: No input provided." << std::endl;
            continue;
        }
    }
    return user_content;
}

void outputMarkdownWithGlow(const std::string& markdownText) {
    char command[256];           // Adjust the size of the buffer as needed
    // Format the command string
    sprintf(command, "glow --style=%s", getenv(GlowStylesheet.c_str()));

    // Open a process to run the `glow` command
    FILE* pipe = popen(command, "w");
    // FILE* pipe = popen("glow --style=dark", "w");
    if (!pipe) {
        std::cerr << "Could not start glow.\n";
        return;
    }

    // Write the markdown text to the `glow` process
    fprintf(pipe, "%s", markdownText.c_str());

    // Close the process after writing
    pclose(pipe);
}

int main(int argc, char** argv) {
    int HelpFlag = 0;
    int VersionFlag = 0;
    int TestFlag = 0;
    int ShortFlag = 0;
    int opt;

    std::string ShortString;

    struct option Opts[] = {
        { "help", no_argument, &HelpFlag, 1 },
        { "version", no_argument, &VersionFlag, 1 },
        { "test", no_argument, &TestFlag, 1 },
        { "short", required_argument, &TestFlag, 1 },
    };

    while (1) {
        opt = getopt_long(argc, argv, "hvts:", Opts, 0);

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
        case 's':
            ShortFlag = 1;
            ShortString = optarg;
            break;
        case 't':
            TestFlag = 1;
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

    if(TestFlag) {
        Database Db;

        // Get the API key from the environment variable
        const char* api_key = std::getenv(OPENAI_API_KEY_ENV_VAR);
        if (!api_key) {
            std::cerr << "Error: " << OPENAI_API_KEY_ENV_VAR << " environment variable not set." << std::endl;
            return EXIT_FAILURE;
        }
            std::cout << 0.1 << std::endl;

        // Generate the chats UID
        const std::string uid = Db.generateUID();
            std::cout << 0.2 << std::endl;
        std::string Name = "";
        int MessageIndex = 0;
        std::string system_content = SYSTEMCONTENT;
        // Add the initial system message to the history
        MessageOptions Options = { api_key, DefaultModel, DefaultMaxTokens, 0.4, 0.5 };
        Messages Message(system_content, 1, Options);

        // Interactive loop for repeated user input
        while (true) {
            std::string user_content = GetInput();
            if (user_content == "exit")
                break;

            Message.Add(user_content, USER);

            /* if(MessageIndex % 5 == 0) */
            if(MessageIndex == 0) {
                Name = Message.MakeName();
            }

            // Save the JSON data to a file
            Db.SaveFile(Message.GetRequest(), ChatArchiveDir, uid, Name, false);

            // Parse the response and get the assistant's reply
            std::string assistant_reply = Message.ParseResponse(Message.Send());
            /* std::cout << assistant_reply << std::endl; */

            if (!assistant_reply.empty()) {
                // Add assistant's reply to the thread
                Message.Add(assistant_reply, ASSISTANT);

                std::cout << "Total tokens: " << GetTokens(user_content, assistant_reply) << std::endl;

                // Save the updated JSON data after the assistant's reply
                Db.SaveFile(Message.GetRequest(), ChatArchiveDir, uid, Name, false);

                // Output the assistant's response with Markdown formatting
                outputMarkdownWithGlow(assistant_reply);

                MessageIndex++;
            }
        }
        return EXIT_SUCCESS;
    }

    if(ShortFlag) {
        Shorts Short(ShortsDir, ShortString);
        std::cout << Short.Return() << std::endl;
        return EXIT_SUCCESS;
    }

    /* Database Db; */
    Display Display;
    Display.Show();

    return EXIT_SUCCESS;
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

