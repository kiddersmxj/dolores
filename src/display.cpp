#include "../inc/display.hpp"
#include <functional>

namespace fs = std::filesystem;

void prependDebugFile(const std::string& text) {
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

Display::Display() {
}

Display::~Display() {
}

void Display::Show() {
    auto screen = ScreenInteractive::Fullscreen();

    int tab_index = 0;
    std::vector<std::string> tab_entries = Db.GetNames();
    std::vector<std::string> Files = Db.GetFileNames();

    std::deque<Messages> AllMessages;
    long index = 0;
    const char* api_key = std::getenv(OPENAI_API_KEY_ENV_VAR);
    for(auto File: Files) {
        if(tab_entries.at(index) == "New Chat") {
            Messages Messages(SYSTEMCONTENT, api_key, 1);
            AllMessages.push_back(Messages);
        } else {
            Messages Messages(Db.ReadFile(index), api_key);
            AllMessages.push_back(Messages);
        }
        index++;
    }

    Vim VimInput;
    std::string vim_content = VimInput.GetVimContent();

    int scroll_position = -1;  // Variable to track the scroll position
    int previous_tab_index = tab_index;

    Box box; // This will hold the size of the rendered element

    auto tab_content = Renderer([&] {
        // Reset scroll position if the tab has changed
        if (tab_index != previous_tab_index) {
            scroll_position = -1;
            previous_tab_index = tab_index;
        }

        int max_line_width;
        int visible_height;

        if(scroll_position == -1) {
            max_line_width = (box.x_max > box.x_min) ? (box.x_max - box.x_min) : Terminal::Size().dimx - LINEWIDTHCONSTRAINT;
            visible_height = (box.y_max > box.y_min) ? (box.y_max - box.y_min) : Terminal::Size().dimy - 3;
        } else {
            max_line_width = box.x_max - box.x_min;
            visible_height = box.y_max - box.y_min;
        }

        struct Pairs {
            std::deque<ftxui::Element> User;
            std::deque<ftxui::Element> Assistant;
        };

        std::deque<Pairs> Messages;

        auto userMessages = AllMessages.at(tab_index).GetUserMessages();
        auto assistantMessages = AllMessages.at(tab_index).GetAssistantMessages();

        size_t maxMessages = userMessages.size();

        for (size_t i = 0; i < maxMessages; ++i) {
            std::deque<ftxui::Element> userElement;
            std::deque<ftxui::Element> assistantElement;

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

        // Join user and assistant messages
        std::deque<Element> elements;
        for (auto it = Messages.begin(); it != Messages.end(); ++it) {
            auto& Message = *it;
            
            elements.push_back(separator() | color(Color::GrayDark));
            
            for (auto& M : Message.User)
                elements.push_back(M);
            
            elements.push_back(separator() | color(Color::GrayDark));
            
            elements.push_back(text(" "));
            
            for (auto& M : Message.Assistant)
                elements.push_back(M);
            
            if (std::next(it) != Messages.end()) {
                elements.push_back(text(" "));
            }
        }

        size_t OriginalSize = elements.size();
        while(elements.size() < visible_height) {
            elements.push_front(text(" "));
        }

        if (scroll_position == -1) {
            scroll_position = elements.size() > visible_height ? elements.size() - visible_height : 0;
        }

        if (scroll_position < 0) {
            scroll_position = 0;
        } else if(OriginalSize < visible_height) {
            scroll_position = 0;
        } else if(scroll_position > OriginalSize) {
            scroll_position = OriginalSize;
        } else if(elements.size() > visible_height) {
            if (scroll_position >= elements.size() - visible_height) {
                scroll_position = elements.size() > 0 ? elements.size() - visible_height : 0;
            }
        }

        std::vector<Element> ScrollElements;
        for (size_t i = scroll_position; i < elements.size(); ++i) {
            ScrollElements.push_back(elements[i]);
        }

        auto rendered_content = vbox(ScrollElements) | yframe;
        return rendered_content | reflect(box); // Correct usage
    });

    ftxui::Component tab_selection = Container::Vertical({}, &tab_index);

    bool input_string_changed = false;
    std::string input_content = "";
    std::string input_string = "";

    auto input_option = InputOption();
    input_option.on_enter = [&] {
        input_content = input_string;
        input_string = "";
        input_string_changed = true;
        tab_selection->TakeFocus();
    };

    ftxui::Component input_box = Input(&input_string, "Press e to edit or shift-e to open vim-input", input_option) | bgcolor(Color::Black);

    auto rebuild_ui = [&]() {
        Db.Get();
        Files = Db.GetFileNames();
        tab_entries = Db.GetNames();
        std::vector<ftxui::Component> entries;
        for (size_t i = 0; i < tab_entries.size(); ++i) {
            entries.push_back(MenuEntry(tab_entries[i]) | color(Color::GrayDark) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 25));
        }

        tab_selection = Container::Vertical(entries, &tab_index);
    };

    rebuild_ui();

    auto main_container = Container::Vertical({
        tab_selection,
        tab_content,
        input_box,
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
                    input_box->Render(),
                }) | flex,
            }) | flex
        });
    });

    main_renderer = CatchEvent(main_renderer, [&](Event event) {

        if(!input_box->Focused()) {
            if (event == Event::Character('E')) {
                VimInput.OpenVim();
            }

            if (event == Event::Character('e')) {
                input_box->TakeFocus();
                return true;
            }

            if (event == Event::Character('q') || event == Event::Character('Q')) {
                screen.ExitLoopClosure()();
                return true;
            }
        }

        if (event == Event::Special({5})) { // Catch Ctrl+e
            scroll_position++;
            return true;
        }

        if (event == Event::Special({25})) { // Catch Ctrl+y
            if (scroll_position > 0) {
                scroll_position--;
            }
            return true;
        }

        if (event.is_mouse() && event.mouse().button == ftxui::Mouse::WheelDown) {
            scroll_position++;
            return true;
        }

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

            vim_content = VimInput.GetVimContent();
            if(vim_content != "" || input_string_changed) {

                input_string_changed = false;
                if(vim_content == "")
                    vim_content = input_content;
                input_content = "";

                VimInput.RemoveVimFile();
                int ti = tab_index;
                if(tab_entries.at(ti) == "New Chat") {
                    ti++;
                    Messages Messages(SYSTEMCONTENT, api_key, 1);
                    Messages.Add(vim_content, USER);

                    std::string Name = Messages.MakeName();
                    Db.SaveFile(Messages.GetRequest(), ChatArchiveDir, Name);

                    AllMessages.insert(AllMessages.begin() + 1, Messages);

                    Files = Db.GetFileNames();
                    tab_entries = Db.GetNames();

                    if((ti - 1) == tab_index)
                        tab_index = 1;

                    tab_selection->Add(MenuEntry(tab_entries[2]) | color(Color::GrayDark) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 25));
                    tab_selection->Render();

                    rebuild_ui();
                } else {
                    AllMessages.at(ti).Add(vim_content, USER);
                    Db.SaveFile(AllMessages.at(ti).GetRequest(), ChatArchiveDir, Files.at(ti), tab_entries.at(ti));
                }

                auto future_response = std::async(std::launch::async, [&]() {
                    auto response = AllMessages.at(ti).Send();
                    return response;
                });

                std::thread([&, future_response = std::move(future_response)]() mutable {
                    auto response = future_response.get(); // Wait for send to complete and get the result
                    if (!response.empty()) {
                        AllMessages.at(ti).Add(AllMessages.at(ti).ParseResponse(response), ASSISTANT);
                        Db.SaveFile(AllMessages.at(ti).GetRequest(), ChatArchiveDir, Files.at(ti), tab_entries.at(ti));
                        auto assistantMessages = AllMessages.at(ti).GetAssistantMessages();
                    } else {
                        prependDebugFile("Empty response received."); // Debug
                    }
                }).detach();

                vim_content = "";

            }

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

