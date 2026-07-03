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

std::vector<std::string> Aparencia::reduzirEscalaAscii(const std::vector<std::string>& arteOriginal, int fatorX, int fatorY) {
    std::vector<std::string> arteReduzida;
    if (fatorX <= 0) fatorX = 1;
    if (fatorY <= 0) fatorY = 1;

    for (size_t i = 0; i < arteOriginal.size(); i += fatorY) {
        std::string novaLinha = "";
        for (size_t j = 0; j < arteOriginal[i].length(); j += fatorX) {
            novaLinha += arteOriginal[i][j];
        }
        arteReduzida.push_back(novaLinha);
    }
    return arteReduzida;
}

void Aparencia::ordenarAlfabeticamente(std::vector<std::string>& lista) {
    std::sort(lista.begin(), lista.end());
}
