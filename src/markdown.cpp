#include "../inc/markdown.hpp"
#include <codecvt>
#include <regex>

#include <iostream>
#include <fstream>
#include <string>

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

using namespace ftxui;

Markdown::Markdown(const std::string& markdownContent, int maxWidth)
    : markdownContent(markdownContent), maxWidth(maxWidth) {
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
                text(line) | color(Color::Magenta),
                text("│") | color(Color::GrayDark),
            });
        } else if (flag == "Bold") {
            element = text(line) | bold;
        } else if (flag == "Italic") {
            element = text(line) | dim;
        } else if (flag == "InlineCode") {
            element = text(line) | color(Color::GrayDark);
        }
    }

    return element;
}

std::vector<Element> Markdown::ParseMarkdownContent() {
    std::vector<Element> elements;
    auto lines = SplitString(markdownContent, '\n');
    bool inCodeBlock = false;
    std::string codeLanguage;

    std::regex codeBlockRegex(R"(^\s*```.*)");  // Regex to match lines starting with optional spaces followed by ```
    std::regex inlineCodeRegex("`([^`]*)`");    // Regex to match inline code sections

    for (const auto& line : lines) {
        std::vector<std::string> flags;

        if (std::regex_match(line, codeBlockRegex)) {
            inCodeBlock = !inCodeBlock;
            if (inCodeBlock) {
                codeLanguage = line.substr(line.find("```") + 3);  // Capture the language
                flags.push_back("CodeBlock");
                elements.push_back(text(codeLanguage + ":"));
                elements.push_back(text(CreateTopBorder(maxWidth)) | color(Color::GrayDark));
            } else {
                codeLanguage.clear();
                elements.push_back(text(CreateBottomBorder(maxWidth)) | color(Color::GrayDark));
            }
        } else if (inCodeBlock) {
            flags.push_back("CodeBlock");
            auto wrappedLines = WrapCode(line, maxWidth);
            for (const auto& wrappedLine : wrappedLines) {
                elements.push_back(ApplyFormatting(wrappedLine, flags));
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
            for(auto remainingLine: WrapText(line, maxWidth)) {
                std::vector<Element> formattedElements;
                std::smatch match;

                // First, handle bold text
                std::regex boldRegex("\\*\\*(.*?)\\*\\*");
                while (std::regex_search(remainingLine, match, boldRegex)) {
                    if (!match.prefix().str().empty()) {
                        formattedElements.push_back(text(match.prefix().str()));
                    }
                    formattedElements.push_back(text(match.str(1) + "") | bold);
                    remainingLine = match.suffix().str();
                }

                // Handle italic text within the remaining text
                std::regex italicRegex("\\*(.*?)\\*");
                while (std::regex_search(remainingLine, match, italicRegex)) {
                    if (!match.prefix().str().empty()) {
                        formattedElements.push_back(text(match.prefix().str()));
                    }
                    formattedElements.push_back(text(match.str(1)) | dim);
                    remainingLine = match.suffix().str();
                }

                // Handle inline code
                while (std::regex_search(remainingLine, match, inlineCodeRegex)) {
                    if (!match.prefix().str().empty()) {
                        formattedElements.push_back(text(match.prefix().str()));
                    }
                    formattedElements.push_back(text(match.str(1)) | bgcolor(Color::GrayDark) | color(Color::Black));
                    remainingLine = match.suffix().str();
                }

                // Add any remaining text after handling all special formatting
                if (!remainingLine.empty()) {
                    formattedElements.push_back(text(remainingLine));
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

std::vector<Element> Markdown::RenderMarkdown() {
    return ParseMarkdownContent();
}

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

