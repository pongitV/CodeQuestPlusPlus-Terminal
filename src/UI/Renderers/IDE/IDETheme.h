#pragma once

#include <string>
#include <vector>

namespace IDETheme {

    // [PT-BR] Paleta de cores estilo IDE C++ para o Terminal
    // [EN-US] C++ IDE style color palette for Terminal
    const std::string COLOR_WORD_KEY = "\033[38;2;86;156;214m";
    const std::string COLOR_TYPE = "\033[38;2;78;201;176m";
    const std::string COLOR_STRING = "\033[38;2;214;157;133m";
    const std::string COLOR_NUMBER = "\033[38;2;181;206;168m";
    const std::string COLOR_COMMENT = "\033[38;2;96;139;78m";
    const std::string COLOR_FUNCTION = "\033[38;2;220;220;170m";
    const std::string COLOR_VARIABLE = "\033[38;2;156;220;254m";
    const std::string COLOR_OPERATOR = "\033[38;2;180;180;180m";
    const std::string COLOR_RESET = "\033[0m";

    inline std::string keyword(const std::string& text) {
        return COLOR_WORD_KEY + text + COLOR_RESET;
    }

    inline std::string type(const std::string& text) {
        return COLOR_TYPE + text + COLOR_RESET;
    }

    inline std::string stringLiteral(const std::string& text) {
        return COLOR_STRING + "\"" + text + "\"" + COLOR_RESET;
    }

    inline std::string number(const std::string& text) {
        return COLOR_NUMBER + text + COLOR_RESET;
    }

    inline std::string comment(const std::string& text) {
        return COLOR_COMMENT + "// " + text + COLOR_RESET;
    }

    inline std::string commentBlock(const std::vector<std::string>& text) {
        std::string result = COLOR_COMMENT + "/*\n";
        for (const auto& line : text) {
            result += " * " + line + "\n";
        }
        result += " */" + COLOR_RESET;
        return result;
    }

    inline std::string function(const std::string& text) {
        return COLOR_FUNCTION + text + COLOR_RESET;
    }

    inline std::string variable(const std::string& text) {
        return COLOR_VARIABLE + text + COLOR_RESET;
    }

    inline std::vector<std::string> formatBoxHowClass(const std::string& className, const std::vector<std::string>& content) {
        std::vector<std::string> formatted;
        formatted.push_back(keyword("class ") + type(className) + " {");
        formatted.push_back(keyword("public:"));
        for (const auto& line : content) {
            formatted.push_back("    " + line);
        }
        formatted.push_back("};");
        return formatted;
    }

} // namespace TemaIDE
