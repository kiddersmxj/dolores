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

	std::string Model = DefaultModel;

    std::deque<Messages> AllMessages;
    long index = 0;
    const char* api_key = std::getenv(OPENAI_API_KEY_ENV_VAR);
    for(auto File: Files) {
        // if(tab_entries.at(index) == "New Chat") {
			// MessageOptions Options = { api_key, Model, 0, 0.4, 0.5 };
            // Messages Messages(SYSTEMCONTENT, 1, Options);
            // AllMessages.push_back(Messages);
        // } else {
			MessageOptions Options = { api_key, Model, 0, 0.4, 0.5 };
            Messages Messages(Db.ReadFile(index), Options);
            AllMessages.push_back(Messages);
        // }
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

    bool input_string_changed = false;
    std::string input_content = "";
    std::string input_string = "";


    Mode Mode;
    std::string InputPrefix = "";

    std::string input_placeholder = "";

    ftxui::Component tab_selection = Container::Vertical({}, &tab_index);

    auto rebuild_ui = [&]() {
        Db.Get();
        Files = Db.GetFileNames();
        tab_entries = Db.GetNames();
		
		tab_selection->DetachAllChildren();

        std::vector<ftxui::Component> entries;
        for (size_t i = 0; i < tab_entries.size(); ++i) {
            tab_selection->Add(MenuEntry(tab_entries[i]) | color(Color::GrayDark) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 25));
        }

        if(Mode.IsNormal()) {
            InputPrefix = "";
            tab_selection->TakeFocus();
        }
    };

    rebuild_ui();

    auto input_option = InputOption();
    input_option.on_enter = [&] {
        if (!input_string.empty() && input_string.back() == '\n') {
            input_string.pop_back();
        }
        std::string CmdChar;
        std::string Args;
        // Create a stringstream from the input string
        std::istringstream stream(input_string);
        // Extract the first word into CmdChar
        stream >> CmdChar;
        // Get the rest of the sentence and store it in Args
        std::getline(stream, Args);
        // Remove leading spaces from Args
        if (!Args.empty() && Args[0] == ' ') {
            Args.erase(0, 1);
        }

        if(Mode.IsNormal()) {
            input_string = "";
            tab_selection->TakeFocus();
        } else if(Mode.IsCommand()) {
            if(CmdChar == "q") {
                screen.ExitLoopClosure()();
            } else if(CmdChar == "o") {
                Mode.Open();
                if(!Args.empty()) {
                    size_t pos = 0;
                    while (pos < Args.length() && !std::isdigit(Args[pos])) {
                        pos++;
                    }

                    std::string File = ShortsDir + Args + "." + Args.substr(0, pos);
                    Vim VimShort(File);
                    Mode.Normal();
                }
            } else if(CmdChar == "E") {
                VimInput.OpenVim();
                Mode.Normal();
            } else if(CmdChar == "e") {
                Mode.Input();
                if(!Args.empty()) {
                    input_content = Args;
                    input_string_changed = true;
                    Mode.Normal();
                }
            } else if(CmdChar == "m") {
				Model = Args;
                AllMessages.at(tab_index).SetModel(Args);
                Mode.Normal();
            } else if(CmdChar == "d") {
				// Check if the index is within bounds
				if (tab_index < AllMessages.size()) {
					// Use the erase method to remove the element at the specified index
					AllMessages.erase(AllMessages.begin() + tab_index);
					Db.DeleteFile(tab_index);
				} else {
					std::cerr << "Index out of bounds" << std::endl;
				}
				rebuild_ui();
				Mode.Normal();
			} else if (CmdChar == "r") {
				rebuild_ui();
				Mode.Normal();
			}
        } else if(Mode.IsInput()) {
            input_content = input_string;
            input_string_changed = true;
            Mode.Normal();
        } else if(Mode.IsOpen()) {
            size_t pos = 0;
            while (pos < input_string.length() && !std::isdigit(input_string[pos])) {
                pos++;
            }

            std::string File = ShortsDir + input_string + "." + input_string.substr(0, pos);
            Vim VimShort(File);
            Mode.Normal();
        }

        InputPrefix = "";
        input_string = "";
        // tab_selection->TakeFocus();
    };

    ftxui::Component input_box = Input(&input_string, input_placeholder, input_option) | bgcolor(Color::Black);

    auto main_container = Container::Vertical({
        tab_selection,
        tab_content,
        input_box,
    });

    auto main_renderer = Renderer(main_container, [&] {
        return vbox({
            hbox({
                vbox({
                    tab_selection->Render(),
                    emptyElement() | flex,
                    separator() | color(Color::RGB(153, 153, 153)),
                    vbox({
                        hbox({
                                text(" Tokens: ") | color(Color::GrayDark), 
                                text(std::to_string(AllMessages.at(tab_index).GetTotalTokens())) | color(Color::Yellow),
                            }),
                        hbox({
                                text(" Model: ") | color(Color::GrayDark), 
                                text(AllMessages.at(tab_index).GetModel()) | color(Color::Red),
                            }),
                        hbox({
                                text(" Mode: ") | color(Color::GrayDark), 
                                text(Mode.Get()) | color(Color::Magenta),
                            }),
                    }),
                }),
                separator() | color(Color::RGB(153, 153, 153)),
                vbox({
                    vbox({
                        tab_content->Render() | flex,
                    }) | flex,
                    // separator() | color(Color::RGB(153, 153, 153)),
                    hbox(text(InputPrefix), input_box->Render() | color(Color::Black)),
                }) | flex,
            }) | flex
        });
    });

    main_renderer = CatchEvent(main_renderer, [&](Event event) {

        if (event == Event::Escape) {
            Mode.Normal();
            InputPrefix = "";
            tab_selection->TakeFocus();
            return true;
        }

        if(!input_box->Focused()) {
            InputPrefix = "";
            if (event == Event::Character(':')) {
                InputPrefix = ":";
                Mode.Command();
                input_box->TakeFocus();
                return true;
            }

            // if (event == Event::Character('E')) {
                // Mode.Input();
            //     VimInput.OpenVim();
            //     return true;
            // }

            // if (event == Event::Character('e')) {
            //     Mode.Input();
            //     // input_box->TakeFocus();
            //     return true;
            // }

            // if (event == Event::Character('q') || event == Event::Character('Q')) {
            //     screen.ExitLoopClosure()();
            //     return true;
            // }

            // if (event == Event::Character('O')) {
            //     input_placeholder = "Enter short name...";
            //     Mode.Open();
            //     // input_box->TakeFocus();
            //     return true;
            // }
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
            std::this_thread::sleep_for(0.05s);

            if(Mode.IsNormal()) {
                input_string = "";
                tab_selection->TakeFocus();
            }
            // } else if(Mode.IsInput() || Mode.IsOpen()) {
            //     input_box->TakeFocus();
            // } else if(Mode.IsNormal()) {
            //     tab_selection->TakeFocus();
            // }

            vim_content = VimInput.GetVimContent();
            if(vim_content != "" || input_string_changed) {
                Mode.Normal();
                tab_selection->TakeFocus();

                input_string_changed = false;
                if(vim_content == "")
                    vim_content = input_content;
                input_content = "";

                VimInput.RemoveVimFile();
                int ti = tab_index;
                if(tab_entries.at(ti) == "New Chat") {
                    ti++;
					prependDebugFile("0");
					MessageOptions Options = { api_key, Model, 0, 0.4, 0.5 };
					prependDebugFile(".1");
                    Messages Messages(SYSTEMCONTENT, 1, Options);
					prependDebugFile(".2");
                    Messages.Add(vim_content, USER);
					prependDebugFile(".3");

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
					prependDebugFile(AllMessages.at(ti).GetMessages().dump(4));
					prependDebugFile("out23");
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

