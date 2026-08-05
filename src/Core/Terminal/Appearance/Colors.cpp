#include "Core/Terminal/Appearance/Appearance.h"
#include <string>

std::string Appearance::color(Color code) {
    if (code == Color::RESET) {
        if (colorBackgroundActive != Color::RESET) {
            return "\033[0m" + color(colorBackgroundActive);
        }
        return "\033[0m";
    }
    if (code == Color::BOLD) return "\033[1m";

    uint32_t val = static_cast<uint32_t>(code);
    bool isBg = (val & 0x80000000) != 0;
    uint8_t r = (val >> 16) & 0xFF;
    uint8_t g = (val >> 8) & 0xFF;
    uint8_t b = val & 0xFF;

    std::string prefix = isBg ? "\033[48;2;" : "\033[38;2;";
    return prefix + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

std::string Appearance::color(Color style, Color code) {
    std::string styleStr = "";
    if (style == Color::BOLD) styleStr = "1;";
    else if (style == Color::RESET) styleStr = "0;";

    if (code == Color::RESET) {
        if (colorBackgroundActive != Color::RESET) {
            return "\033[" + styleStr + "0m" + color(colorBackgroundActive);
        }
        return "\033[" + styleStr + "0m";
    }
    if (code == Color::BOLD) return "\033[" + styleStr + "1m";

    uint32_t val = static_cast<uint32_t>(code);
    bool isBg = (val & 0x80000000) != 0;
    uint8_t r = (val >> 16) & 0xFF;
    uint8_t g = (val >> 8) & 0xFF;
    uint8_t b = val & 0xFF;

    std::string type = isBg ? "48;2;" : "38;2;";
    return "\033[" + styleStr + type + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

std::string Appearance::colorRGB(uint8_t r, uint8_t g, uint8_t b, bool bold) {
    std::string styleStr = bold ? "1;" : "0;";
    return "\033[" + styleStr + "38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

std::string Appearance::bgRGB(uint8_t r, uint8_t g, uint8_t b) {
    return "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

std::string Appearance::getColorRGBFade(Color themeColor, int intensity) {
    uint32_t val = static_cast<uint32_t>(themeColor);
    uint8_t baseR = (val >> 16) & 0xFF;
    uint8_t baseG = (val >> 8) & 0xFF;
    uint8_t baseB = val & 0xFF;
    
    int r = (baseR * intensity) / 255;
    int g = (baseG * intensity) / 255;
    int b = (baseB * intensity) / 255;

    return "\033[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

std::string Appearance::fatarLineAnsi(const std::string& line, float ratio) {
    std::string res;
    res.reserve(line.size());
    size_t i = 0;
    while (i < line.size()) {
        if (i + 7 < line.size() && line[i] == '\033' && line[i+1] == '[') {
            bool isBg = false;
            bool isFg = false;
            size_t startColor = i + 2;
            if (line.compare(startColor, 5, "48;2;") == 0) {
                isBg = true;
            } else if (line.compare(startColor, 5, "38;2;") == 0) {
                isFg = true;
            }
            
            if (isBg || isFg) {
                size_t p = startColor + 5;
                int r = 0, g = 0, b = 0;
                while (p < line.size() && line[p] >= '0' && line[p] <= '9') {
                    r = r * 10 + (line[p] - '0');
                    p++;
                }
                if (p < line.size() && line[p] == ';') {
                    p++;
                    while (p < line.size() && line[p] >= '0' && line[p] <= '9') {
                        g = g * 10 + (line[p] - '0');
                        p++;
                    }
                    if (p < line.size() && line[p] == ';') {
                        p++;
                        while (p < line.size() && line[p] >= '0' && line[p] <= '9') {
                            b = b * 10 + (line[p] - '0');
                            p++;
                        }
                    }
                }
                
                if (p < line.size() && line[p] == 'm') {
                    int fr = static_cast<int>(r * ratio);
                    int fg = static_cast<int>(g * ratio);
                    int fb = static_cast<int>(b * ratio);
                    res += "\033[";
                    if (isBg) res += "48;2;";
                    else res += "38;2;";
                    res += std::to_string(fr) + ";" + std::to_string(fg) + ";" + std::to_string(fb) + "m";
                    i = p + 1;
                    continue;
                }
            }
        }
        res.push_back(line[i]);
        i++;
    }
    return res;
}
