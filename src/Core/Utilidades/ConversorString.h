#pragma once

#include <vector>
#include <string>

class ConversorString {
public:
    static std::vector<std::string> converterStringBrutaParaVetor(const std::string& textoBruto) {
        std::vector<std::string> linhas;
        std::string linhaAtual = "";
        for (char c : textoBruto) {
            if (c == '\n') {
                if (!linhaAtual.empty() && linhaAtual.back() == '\r') {
                    linhaAtual.pop_back();
                }
                linhas.push_back(linhaAtual);
                linhaAtual = "";
            } else {
                linhaAtual += c;
            }
        }
        if (!linhaAtual.empty()) {
            if (linhaAtual.back() == '\r') linhaAtual.pop_back();
            linhas.push_back(linhaAtual);
        }
        return linhas;
    }
};
