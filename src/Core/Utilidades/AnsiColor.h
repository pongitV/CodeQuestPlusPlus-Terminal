#pragma once
#include <string>
#include <cstdint>
#include "../../Sistemas/ConfiguracaoTerminal/Aparencia/Cor.h"

inline std::string ansiCor(Cor codigo) {
    if (codigo == Cor::RESET) return "\033[0m";
    if (codigo == Cor::NEGRITO) return "\033[1m";
    uint32_t valor = static_cast<uint32_t>(codigo);
    uint8_t r = (valor >> 16) & 0xFF;
    uint8_t g = (valor >> 8) & 0xFF;
    uint8_t b = valor & 0xFF;
    return "\033[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

inline std::string ansiCorRGB(uint8_t r, uint8_t g, uint8_t b, bool negrito = false) {
    return (negrito ? "\033[1m" : "") + std::string("\033[38;2;") + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

inline std::string ansiBgRGB(uint8_t r, uint8_t g, uint8_t b) {
    return "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}
