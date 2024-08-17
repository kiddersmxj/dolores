#include "config.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>
#include <vector>

class Markdown {
    public:
        Markdown(const std::string& markdownContent, int maxWidth = 80);
        std::vector<ftxui::Element> RenderMarkdown();

    private:
        std::vector<std::string> SplitString(const std::string& str, char delimiter);
        std::vector<std::string> WrapText(const std::string& text, int maxWidth);
        std::vector<std::string> WrapCode(const std::string& text, int maxWidth);
        std::vector<ftxui::Element> ParseMarkdownContent();
        ftxui::Element ApplyFormatting(const std::string& line, const std::vector<std::string>& flags);
        bool StartsWith(const std::string& str, const std::string& prefix);
        std::string CreateTopBorder(int length);
        std::string CreateBottomBorder(int length);
        std::string markdownContent;
        int maxWidth;
};

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

