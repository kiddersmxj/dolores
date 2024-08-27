#include "config.hpp"
#include "shorts.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <vector>

using namespace ftxui;

class Markdown {
    public:
        Markdown(const std::string& markdownContent, int maxWidth = 80);
        std::deque<ftxui::Element> RenderMarkdown();

    private:
        std::vector<std::string> SplitString(const std::string& str, char delimiter);
        std::vector<std::string> WrapText(const std::string& text, int maxWidth);
        std::vector<std::string> WrapCode(const std::string& text, int maxWidth);
        std::deque<ftxui::Element> ParseMarkdownContent();
        ftxui::Element ApplyFormatting(const std::string& line, const std::vector<std::string>& flags);
        ftxui::Element ApplyFormatting(const std::string& line, std::string language);
        ftxui::Element ParseTextWithStyles(const std::string& text);
        bool StartsWith(const std::string& str, const std::string& prefix);
        std::string CreateTopBorder(int length);
        std::string CreateTopBorder(int length, std::string Title);
        std::string CreateBottomBorder(int length);
        std::string markdownContent;
        int maxWidth;

        bool inside_comment_block = false;

        ftxui::Element parse_code(const std::string& line, bool& inside_comment_block);
        std::vector<ftxui::Element> parse_line(const std::string& line, bool& inside_comment_block);

        // Color settings for each syntax type
        ftxui::Color KeywordColour;
        ftxui::Color TypeColour;
        ftxui::Color FunctionColour;
        ftxui::Color VariableColour;
        ftxui::Color DeclaredVariableColour;
        ftxui::Color DeclaredClassColour;
        ftxui::Color StringLiteralColour;
        ftxui::Color CommentColour;
        ftxui::Color PreprocessorColour;
        ftxui::Color DefaultColour;
        ftxui::Color SymbolColour;
        ftxui::Color NamespaceColour;

        std::unordered_set<std::string> keywords;
        std::unordered_set<std::string> types;
        std::unordered_set<char> symbols;
        std::unordered_map<std::string, ftxui::Color> declared_identifiers;

        bool is_keyword(const std::string& word);
        bool is_type(const std::string& word);
        bool is_function(const std::string& word, const std::string& next_word);
        bool is_variable(const std::string& word);
        bool is_preprocessor(const std::string& word);
        bool is_comment(const std::string& word);
        bool is_declared_identifier(const std::string& word);
        std::vector<std::string> split_word(const std::string& word);
        ftxui::Element parse_word(const std::string& word, const std::string& next_word = "", bool inside_string_literal = false, bool inside_include = false, bool inside_comment = false);
        void track_identifier(const std::string& word, const std::string& previous_word);
};

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

