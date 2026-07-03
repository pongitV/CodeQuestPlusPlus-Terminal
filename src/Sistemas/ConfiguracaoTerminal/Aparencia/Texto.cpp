#include "Aparencia.h"
#include <string>
#include <vector>
#include <algorithm>

std::string Aparencia::removerCoresANSI(const std::string& texto) {
    std::string resultado;
    resultado.reserve(texto.length());
    bool in_sequence = false;

    for (size_t i = 0; i < texto.length(); ++i) {
        if (in_sequence) {
            if (texto[i] >= 0x40 && texto[i] <= 0x7E) {
                in_sequence = false;
            }
        } else {
            if (texto[i] == '\x1b' && i + 1 < texto.length() && texto[i+1] == '[') {
                in_sequence = true;
                i++;
            } else {
                resultado += texto[i];
            }
        }
    }
    return resultado;
}

int Aparencia::obterComprimentoVisual(const std::string& texto) {
    std::string semCores = removerCoresANSI(texto);
    int comprimento = 0;
    for (size_t i = 0; i < semCores.length(); ) {
        unsigned char c = static_cast<unsigned char>(semCores[i]);
        if ((c & 0x80) == 0) i += 1;
        else if ((c & 0xE0) == 0xC0) i += 2;
        else if ((c & 0xF0) == 0xE0) i += 3;
        else if ((c & 0xF8) == 0xF0) i += 4;
        else i += 1;
        comprimento++;
    }
    return comprimento;
}

std::string Aparencia::espacosParaCentralizar(int comprimentoTexto) {
    int espacos = (obterLarguraTerminal() - comprimentoTexto) / 2;
    if (espacos < 0) espacos = 0;
    return std::string(espacos, ' ');
}

std::string Aparencia::centralizarTexto(const std::string& texto) {
    size_t end = texto.find_last_not_of(' ');
    std::string trimmed = (end != std::string::npos) ? texto.substr(0, end + 1) : "";
    return espacosParaCentralizar(obterComprimentoVisual(trimmed)) + texto;
}

void Aparencia::padronizarTamanhoVetor(std::vector<std::string>& linhas) {
    if (linhas.empty()) return;
    size_t maxLen = 0;
    for (const auto& l : linhas)
        if (l.length() > maxLen) maxLen = l.length();
    for (auto& l : linhas)
        if (l.length() < maxLen) l.append(maxLen - l.length(), ' ');
}
