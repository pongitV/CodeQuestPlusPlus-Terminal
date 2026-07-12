#include "Aparencia.h"
#include "../../../Perspectiva/GerenciadorPerspectiva.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>

int Aparencia::FATOR_COMPRESSAO_GLOBAL = 2;

int Aparencia::atrasoDigitacaoMS = 25;
Cor Aparencia::corFundoAtiva = Cor::RESET;

std::vector<std::string> Aparencia::carregarArte(const std::string& caminhoArquivo) {
    std::vector<std::string> arte;
    
    std::vector<std::string> caminhosBase = {
        "",
        "../",
        "../../",
        "../../../"
    };

    std::ifstream arquivo;
    std::string caminhoSucesso;
    for (const auto& base : caminhosBase) {
        arquivo.open(base + caminhoArquivo);
        if (arquivo.is_open()) {
            caminhoSucesso = base + caminhoArquivo;
            break;
        }
    }

    if (arquivo.is_open()) {
        std::string linha;
        while (std::getline(arquivo, linha)) {
            if (!linha.empty() && linha.back() == '\r') {
                linha.pop_back();
            }
            arte.push_back(linha);
        }
    } else {
        arte.push_back("ERRO: Nao foi possivel carregar a arte.");
        arte.push_back("Caminho procurado: " + caminhoArquivo);
    }
    return arte;
}

std::vector<std::string> Aparencia::reduzirEscalaAscii(const std::vector<std::string>& arteOriginal, float fatorX, float fatorY) {
    std::vector<std::string> arteReduzida;
    if (fatorX <= 0.0f) fatorX = 1.0f;
    if (fatorY <= 0.0f) fatorY = 1.0f;

    for (float i = 0; i < arteOriginal.size(); i += fatorY) {
        std::string novaLinha = "";
        int intI = static_cast<int>(i);
        if (intI >= (int)arteOriginal.size()) break;
        for (float j = 0; j < arteOriginal[intI].length(); j += fatorX) {
            int intJ = static_cast<int>(j);
            if (intJ < (int)arteOriginal[intI].length()) {
                novaLinha += arteOriginal[intI][intJ];
            }
        }
        arteReduzida.push_back(novaLinha);
    }
    return arteReduzida;
}

void Aparencia::ordenarAlfabeticamente(std::vector<std::string>& lista) {
    std::sort(lista.begin(), lista.end());
}
