// [PT-BR] Arquivo: StringConverter.h
// [PT-BR] Proposito: Utilitarios para conversao e manipulacao de strings brutas.
// [EN-US] File: StringConverter.h
// [EN-US] Purpose: Utilities for raw string conversion and manipulation.

#pragma once

#include <vector>
#include <string>

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
};
