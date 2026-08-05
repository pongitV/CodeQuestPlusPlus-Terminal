#pragma once

#include <string>
#include <vector>

class GOCodeFormatter {
public:
    static std::string formatWordKey(const std::string& text);
    static std::string formatType(const std::string& text);
    static std::string formatString(const std::string& text);
    static std::string formatNumber(const std::string& text);
    static std::string formatComment(const std::string& text);
    static std::string formatFunction(const std::string& text);
    static std::string formatVariable(const std::string& text);

    static std::string formatLineCode(const std::vector<std::pair<std::string, std::string>>& tokens);
    static std::vector<std::string> formatBlock(const std::vector<std::string>& lines);
};
