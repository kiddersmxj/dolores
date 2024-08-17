#include "../inc/display.hpp"
#include <functional>

namespace fs = std::filesystem;

void NewChat(std::string Content, Database Db) {
    const char* api_key = std::getenv(OPENAI_API_KEY_ENV_VAR);
    if (!api_key) {
        std::cerr << "Error: " << OPENAI_API_KEY_ENV_VAR << " environment variable not set." << std::endl;
        return;
    }

    // Generate the chats UID
    const std::string uid = Db.generateUID();
    std::string Name = "";
    int MessageIndex = 0;
    std::string system_content = SYSTEMCONTENT;
    // Add the initial system message to the history
    Json Message(system_content, api_key);
}

void NewMessage(std::string Content) {
}

void appendDebugFile(const std::string& text) {
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

int OpenVim(std::string uid) {
    std::string filename = "/tmp/vim_tmpfile_" + uid + ".txt";
    return k::ExecCmd("st -e vim " + filename + " > /dev/null 2>&1 &");
}

std::string GetVimContent(std::string uid) {
    std::string filename = "/tmp/vim_tmpfile_" + uid + ".txt";
    std::ifstream file(filename);

    if (!file.is_open()) {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();  // Read the file content into the stringstream
    file.close();

    return buffer.str();  // Return the content as a string
}

Display::Display() {
}

Display::~Display() {
}

void Display::Show() {
    auto screen = ScreenInteractive::Fullscreen();
    std::string TestEntry = "";

    int tab_index = 0;
    std::vector<std::string> tab_entries;

    tab_entries = Db.GetNames();
    std::vector<std::string> Files = Db.GetFileNames();

    auto tab_selection = Menu(&tab_entries, &tab_index, MenuOption::VerticalAnimated()) | size(WIDTH, EQUAL, 25) | color(Color::RGB(153, 153, 153));

    std::deque<Json> AllMessages;
    long index = 0;
    for(auto File: Files) {
        Json Messages(Db.ReadFile(index), OPENAI_API_KEY_ENV_VAR);
        // appendDebugFile(Db.ReadFile(index).dump());
        AllMessages.push_back(Messages);
        index++;
    }

    // std::string tab_content_text = jsons.at(tab_index)
    std::string vim_content = GetVimContent("1234");
    std::string previous_vim_content = vim_content;

    int scroll_position = 0;  // Variable to track the scroll position
    int previous_tab_index = tab_index;

    auto tab_content = Renderer([&] {
        // Reset scroll position if the tab has changed
        if (tab_index != previous_tab_index) {
            scroll_position = 0;
            previous_tab_index = tab_index;
        }

        // Get terminal width minus the specified width
        int max_line_width = Terminal::Size().dimx - LINEWIDTHCONSTRAINT;

        struct Pairs {
            std::vector<ftxui::Element> User;
            std::vector<ftxui::Element> Assistant;
        };

        std::deque<Pairs> Messages;

        auto userMessages = AllMessages.at(tab_index).GetUserMessages();
        for(auto M: userMessages)
            appendDebugFile(M);
        auto assistantMessages = AllMessages.at(tab_index).GetAssistantMessages();

        // size_t maxMessages = std::max(userMessages.size(), assistantMessages.size());
        size_t maxMessages = userMessages.size();

        for (size_t i = 0; i < maxMessages; ++i) {
            std::vector<ftxui::Element> userElement;
            std::vector<ftxui::Element> assistantElement;
            
            if (i < userMessages.size()) {
                Markdown UMd(userMessages[i], max_line_width);
                userElement = UMd.RenderMarkdown();
            }
            
            if (i < assistantMessages.size()) {
                Markdown AMd(assistantMessages[i], max_line_width);
                assistantElement = AMd.RenderMarkdown();
            }
            
            Messages.push_back(Pairs{userElement, assistantElement});
        }

        // Render the Markdown content into lines
        std::vector<Element> elements;
        for(auto Message: Messages) {
            elements.push_back(separator() | color(Color::GrayDark));
            for(auto M: Message.User)
                elements.push_back(M);
            elements.push_back(separator() | color(Color::GrayDark));
            elements.push_back(text(" "));
            for(auto M: Message.Assistant)
                elements.push_back(M);
            elements.push_back(text(" "));
        }

        // Create a view starting from the scroll position
        std::vector<Element> ScrollElements;
        for (size_t i = scroll_position; i < elements.size(); ++i) {
            ScrollElements.push_back(elements[i]);
        }

        // Return the view with yframe and the scroll position
        return vbox(ScrollElements) | yframe;
    });

    auto main_container = Container::Vertical({
        tab_selection,
        tab_content,
    });

    auto main_renderer = Renderer(main_container, [&] {
        return vbox({
            hbox({
                tab_selection->Render(),
                separator() | color(Color::RGB(153, 153, 153)),
                vbox({
                    vbox({
                        tab_content->Render() | flex,
                    }) | flex,
                    separator() | color(Color::RGB(153, 153, 153)),
                    paragraph(vim_content) | size(ftxui::HEIGHT, EQUAL, 2),
                }) | flex,
            }) | flex
        });
    });

    main_renderer = CatchEvent(main_renderer, [&](Event event) {
        if (event == Event::Character('q') || event == Event::Character('Q')) {
            screen.ExitLoopClosure()();
            return true;
        }

        if (event == Event::Return) {
            TestEntry = "TEST";
            tab_entries[1] = TestEntry;
            return true;
        }

        if (event == Event::Character('e')) {
            OpenVim("1234");
        }

        // Handle scrolling down with Ctrl+e
        if (event == Event::Special({5})) { // Catch Ctrl+e
            scroll_position++;
            return true;
        }

        // Handle scrolling up with Ctrl+y
        if (event == Event::Special({25})) { // Catch Ctrl+y
            if (scroll_position > 0) {
                scroll_position--;
            }
            return true;
        }

        // Handle touchpad/mouse wheel scroll down
        if (event.is_mouse() && event.mouse().button == ftxui::Mouse::WheelDown) {
            scroll_position++;
            return true;
        }

        // Handle touchpad/mouse wheel scroll up
        if (event.is_mouse() && event.mouse().button == ftxui::Mouse::WheelUp) {
            if (scroll_position > 0) {
                scroll_position--;
            }
            return true;
        }

        return false;
    });

    bool refresh_ui_continue = true;
    std::thread refresh_ui([&] {
        while (refresh_ui_continue) {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(0.5s);

            vim_content = GetVimContent("1234");
            if (vim_content != previous_vim_content && vim_content != "") {
                previous_vim_content = vim_content;
                AllMessages.at(tab_index).Add(vim_content, USER);
                appendDebugFile(AllMessages.at(tab_index).GetRequest().dump(4));
                // Db.SaveFile(AllMessages.at(tab_index).GetRequest(), ChatArchiveDir, Files.at(tab_index), tab_entries.at(tab_index));
                auto userMessages = AllMessages.at(tab_index).GetUserMessages();
                for(auto M: userMessages)
                    appendDebugFile(std::to_string(tab_index) + ":" + M);
            }

            // Update the tab content based on tab_index
            // tab_content_text = Db.ReadFile(tab_index).dump(4);

            screen.Post([&] {
                screen.Post(Event::Custom); 
            });
        }
    });

    screen.Loop(main_renderer);
    refresh_ui_continue = false;
    refresh_ui.join();
}

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

