#pragma once

#include <string>
#include <vector>
#include <fstream>

namespace AnsiFormatter {

    inline std::string keyword(const std::string& text) {
        return "\033[38;2;86;156;214m" + text + "\033[0m";
    }

    inline std::string type(const std::string& text) {
        return "\033[38;2;78;201;176m" + text + "\033[0m";
    }

    inline std::string stringLit(const std::string& text) {
        return "\033[38;2;214;157;133m\"" + text + "\"\033[0m";
    }

    inline std::string comment(const std::string& text) {
        return "\033[38;2;96;139;78m" + text + "\033[0m";
    }

    inline std::string function(const std::string& text) {
        return "\033[38;2;220;220;170m" + text + "\033[0m()";
    }

    inline std::string colorRGB(const std::string& text, int r, int g, int b) {
        return "\033[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m" + text + "\033[0m";
    }

    inline std::string backgroundRGB(const std::string& text, int r, int g, int b) {
        return "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m" + text + "\033[0m";
    }

    inline std::vector<std::string> formatGOText(const std::vector<std::string>& text) {
        std::vector<std::string> formatted;
        formatted.push_back(keyword("public:"));
        for (const auto& line : text) {
            if (line.empty() || line == " ") {
                formatted.push_back("");
            } else {
                formatted.push_back("    " + keyword("string") + " msg = " + stringLit(line) + ";");
            }
        }
        formatted.push_back("};");
        return formatted;
    }

    inline std::vector<std::string> formatGOArt(const std::vector<std::string>& art) {
        if (art.empty()) return art;
        std::vector<std::string> formatted;
        formatted.push_back(comment("/* =================================="));
        for (const auto& l : art) {
            formatted.push_back(l);
        }
        formatted.push_back(comment("================================== */"));
        return formatted;
    }

    inline std::string formatGOTitle(const std::string& title) {
        return keyword("class") + " " + type(title) + " {";
    }

    inline std::vector<std::string> formatGOOptions(const std::vector<std::string>& options) {
        std::vector<std::string> ideOptions;
        for (const auto& op : options) {
            ideOptions.push_back(function(op));
        }
        return ideOptions;
    }

    inline std::string formatYESDText(const std::string& text) {
        return comment("// ") + text;
    }

}
