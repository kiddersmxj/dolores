#include "../inc/markdown.hpp"

void appendToDebugFile(const std::string& text) {
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

Markdown::Markdown(const std::string& markdownContent, int maxWidth)
    : markdownContent(markdownContent), maxWidth(maxWidth) {

    // Initialize color settings
    KeywordColour = Color::Magenta;
    TypeColour = Color::Yellow;
    FunctionColour = Color::Green;
    VariableColour = Color::Blue;
    DeclaredVariableColour = Color::Blue;
    DeclaredClassColour = Color::Blue;
    StringLiteralColour = Color::Red;
    CommentColour = Color::GrayDark;
    PreprocessorColour = Color::Red;
    DefaultColour = Color::White;
    SymbolColour = Color::White;
    NamespaceColour = Color::Magenta;

    // Initialize keywords and types
    keywords = {
        "int", "float", "double", "return", "if", "else", "for", "while", "class", "struct", "void", "const", "constexpr", 
        "static", "using", "namespace", "auto", "char", "bool", "public", "private", "protected", "virtual", "override", 
        "template", "typename", "nullptr", "true", "false"
    };

    types = {
        "string", "vector", "map", "set", "unordered_map", "unordered_set", "filesystem", "ofstream", "ifstream", 
        "stringstream", "regex", "smatch", "ostringstream", "deque"
    };

    symbols = {
        '(', ')', '{', '}', '[', ']', ';', ',', '.', '<', '>', '=', '+', '-', '*', '/', '%', '&', '|', '^', '!', '~'
    };
}

std::string Markdown::CreateSeparator(int length, std::string Title) {
    if (length < 2) {
        return ""; // If length is less than 2, return an empty string
    }

    std::wstring Sep = L"";

    // Convert the Title from std::string to std::wstring
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wTitle = converter.from_bytes(Title);

    int titleLength = wTitle.length();

    // Calculate the remaining space after the title
    int remainingSpace = length - 2 - titleLength;

    if (remainingSpace >= 0) {
        Sep += wTitle;
        Sep += std::wstring(remainingSpace, L'─');
    } else {
        // If title is too long, truncate it and fill the remaining space
        wTitle = wTitle.substr(0, length);
        Sep += wTitle;
    }

    // Convert the wstring to a string using UTF-8 encoding
    return converter.to_bytes(Sep);
}


std::string Markdown::CreateTopBorder(int length, std::string Title) {
    if (length < 2) {
        return ""; // If length is less than 2, return an empty string
    }

    std::wstring border = L"╭";

    // Convert the Title from std::string to std::wstring
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wTitle = converter.from_bytes(Title);

    int titleLength = wTitle.length();

    // Calculate the remaining space after the title
    int remainingSpace = length - 2 - titleLength;

    if (remainingSpace >= 0) {
        border += wTitle;
        border += std::wstring(remainingSpace, L'─');
    } else {
        // If title is too long, truncate it and fill the remaining space
        wTitle = wTitle.substr(0, length - 2);
        border += wTitle;
    }

    border += L"╮";

    // Convert the wstring to a string using UTF-8 encoding
    return converter.to_bytes(border);
}

std::string Markdown::CreateTopBorder(int length) {
    if (length < 2) {
        return ""; // If length is less than 2, return an empty string
    }

    std::wstring border = L"╭";
    border += std::wstring(length - 2, L'─');
    border += L"╮";

    // Convert the wstring to a string using UTF-8 encoding
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(border);
}

std::string Markdown::CreateBottomBorder(int length) {
    if (length < 2) {
        return ""; // If length is less than 2, return an empty string
    }

    std::wstring border = L"╰";
    border += std::wstring(length - 2, L'─');
    border += L"╯";

    // Convert the wstring to a string using UTF-8 encoding
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(border);
}

std::vector<std::string> Markdown::SplitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::vector<std::string> Markdown::WrapText(const std::string& text, int maxWidth) {
    std::istringstream iss(text);
    std::string word;
    std::string currentLine;
    std::vector<std::string> wrappedLines;

    if(text.empty()) {
        wrappedLines.push_back(" ");
        return wrappedLines;
    }

    while (iss >> word) {
        if (currentLine.length() + word.length() + 1 > static_cast<size_t>(maxWidth)) {
            wrappedLines.push_back(currentLine);
            currentLine.clear();
        }
        if (!currentLine.empty()) {
            currentLine += " ";
        }
        currentLine += word;
    }

    if (!currentLine.empty()) {
        wrappedLines.push_back(currentLine);
    }

    return wrappedLines;
}

std::vector<std::string> Markdown::WrapCode(const std::string& text, int maxWidth) {
    std::istringstream iss(text);
    std::string line;
    std::vector<std::string> wrappedLines;

    // Adjust maxWidth to account for the │ on each side
    int adjustedMaxWidth = maxWidth - 2;

    if (text.empty()) {
        // wrappedLines.push_back("│" + std::string(adjustedMaxWidth, ' ') + "│");
        wrappedLines.push_back("" + std::string(adjustedMaxWidth, ' ') + "");
        return wrappedLines;
    }

    while (std::getline(iss, line)) {
        // Preserve leading whitespace
        size_t leadingWhitespace = line.find_first_not_of(" \t");
        if (leadingWhitespace == std::string::npos) {
            // The line is either empty or consists only of whitespace
            // wrappedLines.push_back("│" + std::string(adjustedMaxWidth, ' ') + "│");
            wrappedLines.push_back("" + std::string(adjustedMaxWidth, ' ') + "");
            continue;
        }

        std::string currentLine = line.substr(0, leadingWhitespace);
        std::string remainingText = line.substr(leadingWhitespace);

        // Wrap the text within the line while preserving leading whitespace
        std::istringstream lineStream(remainingText);
        std::string word;
        while (lineStream >> word) {
            if (currentLine.length() + word.length() > static_cast<size_t>(adjustedMaxWidth - 2)) {
                // Add backslash and spaces to fill the line before adding │
                int spacesToAdd = adjustedMaxWidth - currentLine.length() - 1;
                if (spacesToAdd < 0) spacesToAdd = 0;
                // wrappedLines.push_back("│" + currentLine + "\\" + std::string(spacesToAdd, ' ') + "│");
                wrappedLines.push_back("" + currentLine + "\\" + std::string(spacesToAdd, ' ') + "");
                currentLine = line.substr(0, leadingWhitespace) + word;
            } else {
                if (!currentLine.empty() && currentLine.length() > leadingWhitespace) {
                    currentLine += " ";
                }
                currentLine += word;
            }
        }

        // Push the last part of the line if it's not empty
        if (!currentLine.empty()) {
            int spacesToAdd = adjustedMaxWidth - currentLine.length();
            if (spacesToAdd < 0) spacesToAdd = 0;
            // wrappedLines.push_back("│" + currentLine + std::string(spacesToAdd, ' ') + "│");
            wrappedLines.push_back("" + currentLine + std::string(spacesToAdd, ' ') + "");
        }
    }

    return wrappedLines;
}

bool Markdown::StartsWith(const std::string& str, const std::string& prefix) {
    return str.rfind(prefix, 0) == 0;
}

ftxui::Element Markdown::ApplyFormatting(const std::string& line, const std::vector<std::string>& flags) {
    auto element = text(line);

    for (const auto& flag : flags) {
        if (flag == "Header1") {
            element = text(line) | bold | color(Color::Blue);
        } else if (flag == "Header2") {
            element = text(line) | bold | color(Color::Green);
        } else if (flag == "Header3") {
            element = text(line) | bold | color(Color::Yellow);
        } else if (flag == "CodeBlock") {
            element = hbox({
                text("│") | color(Color::GrayDark),
                parse_code(line, inside_comment_block),
                text("│") | color(Color::GrayDark),
            });
        } else if (flag == "Bold") {
            element = text(line) | bold;
        } else if (flag == "Italic") {
            element = text(line) | dim;
        } else if (flag == "InlineCode") {
            element = text(line) | bgcolor(Color::RGB(71, 71, 71));
        }
    }

    return element;
}

ftxui::Element Markdown::ApplyFormatting(const std::string& line, std::string language) {
    auto element = text(line);

    if (language.find("cpp") != std::string::npos) {
        element = hbox({
            text("│") | color(Color::GrayDark),
            parse_code(line, inside_comment_block),
            text("│") | color(Color::GrayDark),
        });
    } else {
        element = hbox({
            text("│") | color(Color::GrayDark),
            text(line) | color(Color::Blue),
            text("│") | color(Color::GrayDark),
        });
    }

    return element;
}

std::deque<Element> Markdown::ParseMarkdownContent() {
    std::deque<Element> elements;
    auto lines = SplitString(markdownContent, '\n');
    bool inCodeBlock = false;
    std::string codeLanguage;

    std::regex codeBlockRegex(R"(^\s*.*```.*)");  // Regex to match lines starting with optional spaces followed by ```
    std::regex inlineCodeRegex("`([^`]*)`");    // Regex to match inline code sections
    std::regex boldRegex("\\*\\*(.*?)\\*\\*");
    std::regex italicRegex("\\*(.*?)\\*");

    for (const auto& line : lines) {
        std::vector<std::string> flags;

        if (std::regex_match(line, codeBlockRegex)) {
            inCodeBlock = !inCodeBlock;
            if (inCodeBlock) {
                codeLanguage = line.substr(line.find("```") + 3);  // Capture the language
                flags.push_back("CodeBlock");
                // elements.push_back(text(codeLanguage + ":"));
                elements.push_back(text(CreateTopBorder(maxWidth, codeLanguage)) | color(Color::GrayDark));
            } else {
                codeLanguage.clear();
                elements.push_back(text(CreateBottomBorder(maxWidth)) | color(Color::GrayDark));
            }
        } else if (inCodeBlock) {
            flags.push_back("CodeBlock");
            auto wrappedLines = WrapCode(line, maxWidth);
            for (const auto& wrappedLine : wrappedLines) {
                elements.push_back(ApplyFormatting(wrappedLine, codeLanguage));
            }
        } else if (StartsWith(line, "# ")) {
            flags.push_back("Header1");
            auto wrappedLines = WrapText(line.substr(2), maxWidth);
            for (const auto& wrappedLine : wrappedLines) {
                elements.push_back(ApplyFormatting(wrappedLine, flags));
            }
        } else if (StartsWith(line, "## ")) {
            flags.push_back("Header2");
            auto wrappedLines = WrapText(line.substr(3), maxWidth);
            for (const auto& wrappedLine : wrappedLines) {
                elements.push_back(ApplyFormatting(wrappedLine, flags));
            }
        } else if (StartsWith(line, "### ")) {
            flags.push_back("Header3");
            auto wrappedLines = WrapText(line.substr(4), maxWidth);
            for (const auto& wrappedLine : wrappedLines) {
                elements.push_back(ApplyFormatting(wrappedLine, flags));
            }
        } else if (line.find("**") != std::string::npos || line.find("*") != std::string::npos || std::regex_search(line, inlineCodeRegex)) {
            auto wrappedLines = WrapText(line, maxWidth);
            for (auto& wrappedLine : wrappedLines) {
                std::string remainingLine = wrappedLine;
                std::vector<Element> formattedElements;
                std::smatch match;

                // Process inline code first
                while (std::regex_search(remainingLine, match, inlineCodeRegex)) {
                    if (!match.prefix().str().empty()) {
                        formattedElements.push_back(ParseTextWithStyles(match.prefix().str()));
                    }
                    formattedElements.push_back(text(match.str(1)) | bgcolor(Color::RGB(71, 71, 71)));
                    remainingLine = match.suffix().str();
                }

                // Handle remaining text (which may contain bold/italic)
                if (!remainingLine.empty()) {
                    formattedElements.push_back(ParseTextWithStyles(remainingLine));
                }

                elements.push_back(hbox(formattedElements));
            }
        } else {
            auto wrappedLines = WrapText(line, maxWidth);
            for (const auto& wrappedLine : wrappedLines) {
                elements.push_back(text(wrappedLine));
            }
        }
    }

    return elements;
}

ftxui::Element Markdown::ParseTextWithStyles(const std::string& text) {
    std::vector<Element> formattedElements;
    std::smatch match;
    std::string remainingText = text;
    std::regex boldRegex("\\*\\*(.*?)\\*\\*");
    std::regex italicRegex("\\*(.*?)\\*");

    // Handle bold text
    while (std::regex_search(remainingText, match, boldRegex)) {
        if (!match.prefix().str().empty()) {
            formattedElements.push_back(ParseTextWithStyles(match.prefix().str()));
        }
        formattedElements.push_back(ftxui::text(match.str(1)) | bold);
        remainingText = match.suffix().str();
    }

    // Handle italic text within the remaining text
    while (std::regex_search(remainingText, match, italicRegex)) {
        if (!match.prefix().str().empty()) {
            formattedElements.push_back(ftxui::text(match.prefix().str()));
        }
        formattedElements.push_back(ftxui::text(match.str(1)) | dim);
        remainingText = match.suffix().str();
    }

    // Add any remaining text
    if (!remainingText.empty()) {
        formattedElements.push_back(ftxui::text(remainingText));
    }

    return hbox(formattedElements);
}

std::deque<Element> Markdown::RenderMarkdown() {
    return ParseMarkdownContent();
}

bool Markdown::is_keyword(const std::string& word) {
    return keywords.find(word) != keywords.end();
}

bool Markdown::is_type(const std::string& word) {
    if (types.find(word) != types.end()) {
        return true;
    }
    size_t pos = word.find("::");
    if (pos != std::string::npos) {
        std::string base_type = word.substr(pos + 2);
        return types.find(base_type) != types.end();
    }
    return false;
}

bool Markdown::is_function(const std::string& word, const std::string& next_word) {
    return next_word == "(";
}

bool Markdown::is_variable(const std::string& word) {
    return !word.empty() && (islower(word[0]) || word[0] == '_') && !is_keyword(word);
}

bool Markdown::is_preprocessor(const std::string& word) {
    return !word.empty() && word.front() == '#';
}

bool Markdown::is_comment(const std::string& word) {
    return word.size() >= 2 && word.substr(0, 2) == "//";
}

bool Markdown::is_declared_identifier(const std::string& word) {
    return declared_identifiers.find(word) != declared_identifiers.end();
}

std::vector<std::string> Markdown::split_word(const std::string& word) {
    std::vector<std::string> tokens;
    std::string current_token;
    for (char c : word) {
        if (symbols.find(c) != symbols.end()) {
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token.clear();
            }
            tokens.push_back(std::string(1, c));  // Add the symbol as its own token
        } else {
            current_token += c;
        }
    }
    if (!current_token.empty()) {
        tokens.push_back(current_token);
    }
    return tokens;
}

Element Markdown::parse_word(const std::string& word, const std::string& next_word, bool inside_string_literal, bool inside_include, bool inside_comment) {
    if (inside_comment) {
        return text(word) | color(CommentColour);
    } else if (inside_include) {
        return text(word) | color(PreprocessorColour);
    } else if (inside_string_literal) {
        return text(word) | color(StringLiteralColour);
    } else if (is_preprocessor(word)) {
        return text(word) | color(PreprocessorColour);
    } else if (is_comment(word)) {
        return text(word) | color(CommentColour);
    } else if (is_declared_identifier(word)) {
        return text(word) | color(declared_identifiers[word]);
    } else if (is_function(word, next_word)) {
        return text(word) | color(FunctionColour);
    } else if (is_type(word)) {
        return text(word) | color(TypeColour);  // Types are highlighted with TypeColour
    } else if (is_keyword(word)) {
        return text(word) | color(KeywordColour);
    } else if (is_variable(word)) {
        return text(word) | color(VariableColour);
    } else if (symbols.find(word[0]) != symbols.end()) {
        return text(word) | color(SymbolColour);
    } else if (word.find("::") != std::string::npos) {
        return text(word) | color(NamespaceColour);  // Handle namespace like std::
    } else {
        return text(word) | color(DefaultColour);
    }
}

void Markdown::track_identifier(const std::string& word, const std::string& previous_word) {
    std::string clean_word = word;

    while (!clean_word.empty() && (clean_word.back() == ';' || clean_word.back() == '=' || clean_word.back() == '(')) {
        clean_word.pop_back();
    }

    if (previous_word == "class" || previous_word == "struct") {
        declared_identifiers[clean_word] = DeclaredClassColour;
    }
    else if (is_type(previous_word) || previous_word == "auto" || (is_keyword(previous_word) && clean_word.front() != '(')) {
        declared_identifiers[clean_word] = DeclaredVariableColour;
    }
}

std::vector<Element> Markdown::parse_line(const std::string& line, bool& inside_comment_block) {
    std::vector<Element> elements;
    std::string current_word;
    std::string previous_word;
    bool inside_string_literal = false;
    bool inside_include = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];

        if (inside_comment_block) {
            current_word += c;
            if (current_word.size() >= 2 && current_word.substr(current_word.size() - 2) == "*/") {
                inside_comment_block = false;
                elements.push_back(text(current_word) | color(CommentColour));
                current_word.clear();
            }
        } else if (c == '/' && i + 1 < line.size() && line[i + 1] == '*') {
            if (!current_word.empty()) {
                track_identifier(current_word, previous_word);
                elements.push_back(parse_word(current_word, "", inside_string_literal, inside_include, inside_comment_block));
                previous_word = current_word;
                current_word.clear();
            }
            inside_comment_block = true;
            current_word += "/*";
            i++;  // Skip the '*' character
        } else if (c == '/' && i + 1 < line.size() && line[i + 1] == '/') {
            if (!current_word.empty()) {
                track_identifier(current_word, previous_word);
                elements.push_back(parse_word(current_word, "", inside_string_literal, inside_include, inside_comment_block));
                previous_word = current_word;
                current_word.clear();
            }
            std::string comment = line.substr(i);
            elements.push_back(text(comment) | color(CommentColour));
            break;  // Skip the rest of the line as it's a comment
        } else if (c == '"' && !inside_include && !inside_comment_block) {
            if (!current_word.empty()) {
                auto tokens = split_word(current_word);
                track_identifier(current_word, previous_word);
                for (size_t j = 0; j < tokens.size(); ++j) {
                    elements.push_back(parse_word(tokens[j], (j + 1 < tokens.size()) ? tokens[j + 1] : "", inside_string_literal, inside_include, inside_comment_block));
                }
                previous_word = current_word;
                current_word.clear();
            }
            inside_string_literal = !inside_string_literal;
            elements.push_back(text(std::string(1, c)) | color(StringLiteralColour));
        } else if (inside_include || (current_word == "#include" && c == ' ')) {
            if (current_word == "#include") {
                elements.push_back(text(current_word) | color(PreprocessorColour));
                current_word.clear();
                inside_include = true;
            }

            if (c == '<' || c == '"') {
                current_word += c;
            } else if (c == '>' || c == '"') {
                current_word += c;
                elements.push_back(text(current_word) | color(PreprocessorColour));
                current_word.clear();
                inside_include = false;
            } else {
                current_word += c;
            }
        } else if (std::isspace(c) && !inside_string_literal) {
            if (!current_word.empty()) {
                auto tokens = split_word(current_word);
                track_identifier(current_word, previous_word);
                for (size_t j = 0; j < tokens.size(); ++j) {
                    elements.push_back(parse_word(tokens[j], (j + 1 < tokens.size()) ? tokens[j + 1] : "", inside_string_literal, inside_include, inside_comment_block));
                }
                previous_word = current_word;
                current_word.clear();
            }
            if (c == ' ') {
                elements.push_back(text(" "));
            } else if (c == '\t') {
                elements.push_back(text("    "));  // Represent a tab with four spaces
            }
        } else {
            current_word += c;
        }
    }

    if (!current_word.empty()) {
        auto tokens = split_word(current_word);
        track_identifier(current_word, previous_word);
        for (size_t j = 0; j < tokens.size(); ++j) {
            elements.push_back(parse_word(tokens[j], (j + 1 < tokens.size()) ? tokens[j + 1] : "", inside_string_literal, inside_include, inside_comment_block));
        }
    }

    return elements;
}

Element Markdown::parse_code(const std::string& line, bool& inside_comment_block) {
    std::vector<Element> elements = parse_line(line, inside_comment_block);
    return hbox(elements);
}

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

