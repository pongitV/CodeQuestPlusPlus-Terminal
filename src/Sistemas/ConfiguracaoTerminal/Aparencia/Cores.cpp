#include "Aparencia.h"
#include <string>

std::string Aparencia::cor(Cor codigo) {
    if (codigo == Cor::RESET) {
        if (corFundoAtiva != Cor::RESET) {
            return "\033[0m" + cor(corFundoAtiva);
        }
        return "\033[0m";
    }
    if (codigo == Cor::NEGRITO) return "\033[1m";

    uint32_t val = static_cast<uint32_t>(codigo);
    bool isBg = (val & 0x80000000) != 0;
    uint8_t r = (val >> 16) & 0xFF;
    uint8_t g = (val >> 8) & 0xFF;
    uint8_t b = val & 0xFF;

    std::string prefix = isBg ? "\033[48;2;" : "\033[38;2;";
    return prefix + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

std::string Aparencia::cor(Cor estilo, Cor codigo) {
    std::string estiloStr = "";
    if (estilo == Cor::NEGRITO) estiloStr = "1;";
    else if (estilo == Cor::RESET) estiloStr = "0;";

    if (codigo == Cor::RESET) {
        if (corFundoAtiva != Cor::RESET) {
            return "\033[" + estiloStr + "0m" + cor(corFundoAtiva);
        }
        return "\033[" + estiloStr + "0m";
    }
    if (codigo == Cor::NEGRITO) return "\033[" + estiloStr + "1m";

    uint32_t val = static_cast<uint32_t>(codigo);
    bool isBg = (val & 0x80000000) != 0;
    uint8_t r = (val >> 16) & 0xFF;
    uint8_t g = (val >> 8) & 0xFF;
    uint8_t b = val & 0xFF;

    std::string type = isBg ? "48;2;" : "38;2;";
    return "\033[" + estiloStr + type + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

std::string Aparencia::corRGB(uint8_t r, uint8_t g, uint8_t b, bool negrito) {
    std::string estiloStr = negrito ? "1;" : "0;";
    return "\033[" + estiloStr + "38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

std::string Aparencia::bgRGB(uint8_t r, uint8_t g, uint8_t b) {
    return "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

std::string Aparencia::obterCorRGBFade(Cor corTema, int intensidade) {
    uint32_t val = static_cast<uint32_t>(corTema);
    uint8_t baseR = (val >> 16) & 0xFF;
    uint8_t baseG = (val >> 8) & 0xFF;
    uint8_t baseB = val & 0xFF;
    
    int r = (baseR * intensidade) / 255;
    int g = (baseG * intensidade) / 255;
    int b = (baseB * intensidade) / 255;

    return "\033[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

std::string Aparencia::fadarLinhaAnsi(const std::string& linha, float ratio) {
    std::string res;
    res.reserve(linha.size());
    size_t i = 0;
    while (i < linha.size()) {
        if (i + 7 < linha.size() && linha[i] == '\033' && linha[i+1] == '[') {
            bool isBg = false;
            bool isFg = false;
            size_t startColor = i + 2;
            if (linha.compare(startColor, 5, "48;2;") == 0) {
                isBg = true;
            } else if (linha.compare(startColor, 5, "38;2;") == 0) {
                isFg = true;
            }
            
            if (isBg || isFg) {
                size_t p = startColor + 5;
                int r = 0, g = 0, b = 0;
                while (p < linha.size() && linha[p] >= '0' && linha[p] <= '9') {
                    r = r * 10 + (linha[p] - '0');
                    p++;
                }
                if (p < linha.size() && linha[p] == ';') {
                    p++;
                    while (p < linha.size() && linha[p] >= '0' && linha[p] <= '9') {
                        g = g * 10 + (linha[p] - '0');
                        p++;
                    }
                    if (p < linha.size() && linha[p] == ';') {
                        p++;
                        while (p < linha.size() && linha[p] >= '0' && linha[p] <= '9') {
                            b = b * 10 + (linha[p] - '0');
                            p++;
                        }
                    }
                }
                
                if (p < linha.size() && linha[p] == 'm') {
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
        res.push_back(linha[i]);
        i++;
    }
    return res;
}
