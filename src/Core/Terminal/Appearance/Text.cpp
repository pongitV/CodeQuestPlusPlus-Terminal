#include "Core/Terminal/Appearance/Appearance.h"
#include <string>
#include <vector>
#include <algorithm>

std::string Appearance::removeANSIColors(const std::string& text) {
    std::string result;
    result.reserve(text.length());
    bool in_sequence = false;

    for (size_t i = 0; i < text.length(); ++i) {
        if (in_sequence) {
            if (text[i] >= 0x40 && text[i] <= 0x7E) {
                in_sequence = false;
            }
        } else {
            if (text[i] == '\x1b' && i + 1 < text.length() && text[i+1] == '[') {
                in_sequence = true;
                i++;
            } else {
                result += text[i];
            }
        }
    }
    return result;
}

int Appearance::getVisualLength(const std::string& text) {
    std::string withoutColors = removeANSIColors(text);
    int length = 0;
    for (size_t i = 0; i < withoutColors.length(); ) {
        unsigned char c = static_cast<unsigned char>(withoutColors[i]);
        if ((c & 0x80) == 0) i += 1;
        else if ((c & 0xE0) == 0xC0) i += 2;
        else if ((c & 0xF0) == 0xE0) i += 3;
        else if ((c & 0xF8) == 0xF0) i += 4;
        else i += 1;
        length++;
    }
    return length;
}

std::string Appearance::spacesToCenter(int textLength) {
    int spaces = (getTerminalWidth() - textLength) / 2;
    if (spaces < 0) spaces = 0;
    return std::string(spaces, ' ');
}

std::string Appearance::centerText(const std::string& text) {
    size_t end = text.find_last_not_of(' ');
    std::string trimmed = (end != std::string::npos) ? text.substr(0, end + 1) : "";
    return spacesToCenter(getVisualLength(trimmed)) + text;
}

void Appearance::standardizeVectorSize(std::vector<std::string>& lines) {
    if (lines.empty()) return;
    size_t maxLen = 0;
    for (const auto& l : lines)
        if (l.length() > maxLen) maxLen = l.length();
    for (auto& l : lines)
        if (l.length() < maxLen) l.append(maxLen - l.length(), ' ');
}
