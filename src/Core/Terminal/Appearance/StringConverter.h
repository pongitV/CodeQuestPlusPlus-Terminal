// [PT-BR] Arquivo: StringConverter.h
// [PT-BR] Proposito: Utilitarios para conversao e manipulacao de strings brutas.
// [EN-US] File: StringConverter.h
// [EN-US] Purpose: Utilities for raw string conversion and manipulation.

#pragma once

#include <vector>
#include <string>
#include <string_view>

class StringConvert {
public:
    static std::vector<std::string> convertRawStringToVector(const std::string& textGross) {
        std::vector<std::string> lines;
        std::string lineCurrent = "";
        for (char c : textGross) {
            if (c == '\n') {
                if (!lineCurrent.empty() && lineCurrent.back() == '\r') {
                    lineCurrent.pop_back();
                }
                lines.push_back(lineCurrent);
                lineCurrent = "";
            } else {
                lineCurrent += c;
            }
        }
        if (!lineCurrent.empty()) {
            if (lineCurrent.back() == '\r') lineCurrent.pop_back();
            lines.push_back(lineCurrent);
        }
        return lines;
    }

    static std::vector<std::string> splitUtf8(std::string_view text) {
        std::vector<std::string> chars;
        size_t i = 0;
        while (i < text.length()) {
            size_t charLen = 1;
            unsigned char c = static_cast<unsigned char>(text[i]);
            if ((c & 0x80) == 0) charLen = 1;
            else if ((c & 0xE0) == 0xC0) charLen = 2;
            else if ((c & 0xF0) == 0xE0) charLen = 3;
            else if ((c & 0xF8) == 0xF0) charLen = 4;

            if (i + charLen > text.length()) charLen = text.length() - i;
            chars.emplace_back(text.substr(i, charLen));
            i += charLen;
        }
        return chars;
    }
};
