#pragma once
#include <string>
#include <cstdint>
#include "Core/Terminal/Appearance/Color.h"

inline std::string ansiColor(Color code) {
    if (code == Color::RESET) return "\033[0m";
    if (code == Color::BOLD) return "\033[1m";
    uint32_t value = static_cast<uint32_t>(code);
    uint8_t r = (value >> 16) & 0xFF;
    uint8_t g = (value >> 8) & 0xFF;
    uint8_t b = value & 0xFF;
    return "\033[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

inline std::string ansiColorRGB(uint8_t r, uint8_t g, uint8_t b, bool bold = false) {
    return (bold ? "\033[1m" : "") + std::string("\033[38;2;") + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

inline std::string ansiBgRGB(uint8_t r, uint8_t g, uint8_t b) {
    return "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}
