// [PT-BR] Utilitarios para formatacao de texto com cores ANSI
// [EN-US] Utilities for text formatting with ANSI colors
#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <format>

namespace AnsiFormatter {

    // [PT-BR] Funcoes auxiliares para colorir texto no terminal com C++23
    // [EN-US] Helper functions to color text in the terminal with C++23
    inline std::string keyword(std::string_view text) {
        return std::format("\033[38;2;86;156;214m{}\033[0m", text);
    }

    inline std::string type(std::string_view text) {
        return std::format("\033[38;2;78;201;176m{}\033[0m", text);
    }

    inline std::string stringLit(std::string_view text) {
        return std::format("\033[38;2;214;157;133m\"{}\"\033[0m", text);
    }

    inline std::string comment(std::string_view text) {
        return std::format("\033[38;2;96;139;78m{}\033[0m", text);
    }

    inline std::string function(std::string_view text) {
        return std::format("\033[38;2;220;220;170m{}()\033[0m", text);
    }

    inline std::string colorRGB(std::string_view text, int r, int g, int b) {
        return std::format("\033[38;2;{};{};{}m{}\033[0m", r, g, b, text);
    }

    inline std::string backgroundRGB(std::string_view text, int r, int g, int b) {
        return std::format("\033[48;2;{};{};{}m{}\033[0m", r, g, b, text);
    }

    inline std::vector<std::string> formatIDEText(const std::vector<std::string>& text) {
        std::vector<std::string> formatted;
        formatted.push_back(keyword("public:"));
        for (const auto& line : text) {
            if (line.empty() || line == " ") {
                formatted.push_back("");
            } else {
                formatted.push_back(std::format("    {} msg = {};", keyword("string"), stringLit(line)));
            }
        }
        formatted.push_back("};");
        return formatted;
    }

    inline std::vector<std::string> formatIDEArt(const std::vector<std::string>& art) {
        if (art.empty()) return art;
        std::vector<std::string> formatted;
        formatted.push_back(comment("/* =================================="));
        for (const auto& l : art) {
            formatted.push_back(l);
        }
        formatted.push_back(comment("================================== */"));
        return formatted;
    }

    inline std::string formatIDETitle(std::string_view title) {
        return std::format("{} {} {{", keyword("class"), type(title));
    }

    inline std::vector<std::string> formatGOOptions(const std::vector<std::string>& options) {
        std::vector<std::string> ideOptions;
        ideOptions.reserve(options.size());
        for (const auto& op : options) {
            ideOptions.push_back(function(op));
        }
        return ideOptions;
    }

    inline std::string formatYESDText(std::string_view text) {
        return std::format("// {}", text);
    }

}
