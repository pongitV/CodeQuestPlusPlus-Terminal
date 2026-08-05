#include "UI/Renderers/IDE/EngineIDE/IDECodeFormatter.h"

std::string GOCodeFormatter::formatWordKey(const std::string& text) {
    return "\033[38;2;86;156;214m" + text + "\033[0m";
}

std::string GOCodeFormatter::formatType(const std::string& text) {
    return "\033[38;2;78;201;176m" + text + "\033[0m";
}

std::string GOCodeFormatter::formatString(const std::string& text) {
    return "\033[38;2;214;157;133m\"" + text + "\"\033[0m";
}

std::string GOCodeFormatter::formatNumber(const std::string& text) {
    return "\033[38;2;181;206;168m" + text + "\033[0m";
}

std::string GOCodeFormatter::formatComment(const std::string& text) {
    return "\033[38;2;96;139;78m// " + text + "\033[0m";
}

std::string GOCodeFormatter::formatFunction(const std::string& text) {
    return "\033[38;2;220;220;170m" + text + "\033[0m";
}

std::string GOCodeFormatter::formatVariable(const std::string& text) {
    return "\033[38;2;156;220;254m" + text + "\033[0m";
}

std::string GOCodeFormatter::formatLineCode(const std::vector<std::pair<std::string, std::string>>& tokens) {
    std::string result;
    for (const auto& [type, text] : tokens) {
        if (type == "keyword") result += formatWordKey(text);
        else if (type == "type") result += formatType(text);
        else if (type == "string") result += formatString(text);
        else if (type == "number") result += formatNumber(text);
        else if (type == "comment") result += formatComment(text);
        else if (type == "function") result += formatFunction(text);
        else if (type == "variable") result += formatVariable(text);
        else result += text;
    }
    return result;
}

std::vector<std::string> GOCodeFormatter::formatBlock(const std::vector<std::string>& lines) {
    std::vector<std::string> result;
    for (const auto& line : lines) {
        result.push_back("    " + line);
    }
    return result;
}
