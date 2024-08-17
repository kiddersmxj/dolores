#include "../inc/display.hpp"
#include <functional>

namespace fs = std::filesystem;

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

    std::deque<Json> jsons;
    long index = 0;
    for(auto File: Files) {
        Json Json(Db.ReadFile(index), OPENAI_API_KEY_ENV_VAR);
        jsons.push_back(Json);
        index++;
    }

    // std::string tab_content_text = jsons.at(tab_index)
    // k::WriteFileLines(tab_content_text, "tab_content.txt");
    std::string vim_content = GetVimContent("1234");
    std::string previous_vim_content = vim_content;

    int scroll_position = 0;  // Variable to track the scroll position
    int previous_tab_index = tab_index;

    // auto tab_content = Renderer([&] {
    //     // Get terminal width minus the specified width
    //     int max_line_width = Terminal::Size().dimx - LINEWIDTHCONSTRAINT;
    //     Markdown Md(jsons.at(tab_index).GetMessagePairString(), max_line_width);
    //     return vbox(Md.RenderMarkdown()) | yframe;
    // });

    auto tab_content = Renderer([&] {
        // Reset scroll position if the tab has changed
        if (tab_index != previous_tab_index) {
            scroll_position = 0;
            previous_tab_index = tab_index;
        }

        // Get terminal width minus the specified width
        int max_line_width = Terminal::Size().dimx - LINEWIDTHCONSTRAINT;

        // Use the Markdown class with line wrapping handled natively
        Markdown Md(jsons.at(tab_index).GetMessagePairString(), max_line_width);

        // Render the Markdown content into lines
        auto rendered_lines = Md.RenderMarkdown();

        // Create a view starting from the scroll position
        std::vector<Element> elements;
        for (size_t i = scroll_position; i < rendered_lines.size(); ++i) {
            elements.push_back(rendered_lines[i]);
        }

        // Return the view with yframe and the scroll position
        return vbox(elements) | yframe;
    });

    // auto tab_content = Renderer([&] {
    //     if(tab_index != previous_tab_index) {
    //         scroll_position = 0;
    //         previous_tab_index = tab_index;
    //     }
    //     // Split the text content by lines
    //     std::vector<std::string> lines;
    //     std::string Text = jsons.at(tab_index).GetMessagePairString();
    //     std::istringstream iss(Text);
    //     std::string line;
        
    //     // Store the wrapped lines
    //     std::vector<std::string> wrapped_lines;
        
    //     // Get terminal width minus the specified width
    //     int max_line_width = Terminal::Size().dimx - LINEWIDTHCONSTRAINT;

    //     // Wrap lines if they exceed the maximum line width without breaking words
    //     while (std::getline(iss, line)) {
    //         std::string current_line;
    //         std::istringstream word_stream(line);
    //         std::string word;
            
    //         while (word_stream >> word) {
    //             // If adding the next word exceeds the max width, push the current line and start a new one
    //             if (current_line.length() + word.length() + 1 > static_cast<size_t>(max_line_width)) {
    //                 wrapped_lines.push_back(current_line);
    //                 current_line.clear();
    //             }

    //             // Add the word to the current line
    //             if (!current_line.empty()) {
    //                 current_line += " ";
    //             }
    //             current_line += word;
    //         }

    //         // Add any remaining part of the line
    //         if (!current_line.empty()) {
    //             wrapped_lines.push_back(current_line);
    //         }
    //     }

    //     // Create a view starting from the scroll position
    //     std::vector<Element> elements;
    //     for (size_t i = scroll_position; i < wrapped_lines.size(); ++i) {
    //         elements.push_back(text(wrapped_lines[i]));
    //     }

    //     return vbox(elements) | yframe;
    // });

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
                // Update the tab content text when vim_content changes
                // tab_content_text = vim_content;
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

