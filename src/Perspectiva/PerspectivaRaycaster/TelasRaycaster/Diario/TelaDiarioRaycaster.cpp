#include "TelaDiarioRaycaster.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "../../../../Core/Utilidades/Aparencia.h"
#include "../../../TelasBase/TelaBase.h"
#include "../../../TelasBase/Diario/TelaDiarioLayout.h"
#include "../../EngineRaycaster/Raycaster.h"

void TelaDiarioRaycaster::renderizarFundo() {
    Raycaster::restaurarUltimoQuadro();
}

void TelaDiarioRaycaster::exibirCabecalho(int startY) {
    int larguraConsole = Aparencia::obterLarguraTerminal();
    int logoHeight = ArtesDiario::logoDiario.size();
    int logoY = startY > 0 ? (startY - 1 - logoHeight) : 2;
    if (logoY < 0) logoY = 0;

    int compVisualLogo = 0;
    for (const auto& linha : ArtesDiario::logoDiario) {
        int comp = Aparencia::obterComprimentoVisual(linha);
        if (comp > compVisualLogo) compVisualLogo = comp;
    }
    int logoX = (larguraConsole - compVisualLogo) / 2;
    if (logoX < 0) logoX = 0;

    std::string corTitulo = Aparencia::cor(Cor::AMARELO);
    for (int i = 0; i < (int)ArtesDiario::logoDiario.size(); ++i) {
        Aparencia::moverCursor(logoX, logoY + i);
        const std::string& linha = ArtesDiario::logoDiario[i];

        std::string buffer = corTitulo;
        int spaceCount = 0;

        for (size_t j = 0; j < linha.length(); ) {
            if (linha[j] == ' ') {
                spaceCount++;
                j++;
            } else {
                if (spaceCount > 0) {
                    buffer += "\033[" + std::to_string(spaceCount) + "C";
                    spaceCount = 0;
                }
                unsigned char uc = linha[j];
                int charLen = 1;
                if ((uc & 0x80) == 0) charLen = 1;
                else if ((uc & 0xE0) == 0xC0) charLen = 2;
                else if ((uc & 0xF0) == 0xE0) charLen = 3;
                else if ((uc & 0xF8) == 0xF0) charLen = 4;
                buffer += linha.substr(j, charLen);
                j += charLen;
            }
        }
        buffer += "\033[0m";
        std::cout << buffer;
    }
    std::cout << std::flush;
}

void TelaDiarioRaycaster::renderizarCaixa(const std::vector<std::string>& linhas, const std::string& titulo, Cor corCaixa, int minY, int startYOverride) {
    std::vector<std::string> caixaFinal = TelaBase::criarCaixa(linhas, titulo, 0, corCaixa, "\033[48;2;25;25;25m");
    int outW = Aparencia::obterComprimentoVisual(caixaFinal[0]);
    int outH = caixaFinal.size();

    int startX = (Aparencia::obterLarguraTerminal() - outW) / 2;
    int startY = startYOverride >= 0 ? startYOverride : (Aparencia::obterAlturaTerminal() - outH) / 2;
    if (startX < 0) startX = 0;
    if (startY < minY) startY = minY;
    if (startY < 8) startY = 8;

    std::cout << "\033[?25l";
    for (size_t i = 0; i < caixaFinal.size(); ++i) {
        Aparencia::moverCursor(startX, startY + i);
        std::cout << caixaFinal[i];
    }
    std::cout << std::flush;
}

void TelaDiarioRaycaster::renderizarPopupMensagem(const std::string& titulo, const std::vector<std::string>& texto) {
    std::vector<std::string> linhas;
    for (const auto& t : texto) {
        linhas.push_back(" " + t + " ");
    }
    linhas.push_back("");
    linhas.push_back("   " + Aparencia::cor(Cor::VERDE) + "> Voltar" + Aparencia::cor(Cor::BRANCO) + "\033[48;2;25;25;25m");

    std::vector<std::string> caixaFinal = TelaBase::criarCaixa(linhas, titulo, 0, Cor::AMARELO, "\033[48;2;25;25;25m");
    int outW = Aparencia::obterComprimentoVisual(caixaFinal[0]);
    int outH = caixaFinal.size();

    int startX = (Aparencia::obterLarguraTerminal() - outW) / 2;
    int alturaTerm = Aparencia::obterAlturaTerminal();
    int startY = (alturaTerm - outH) / 2;
    if (startX < 0) startX = 0;
    if (startY < 8) startY = 8;
    if (startY + outH > alturaTerm) startY = std::max(0, alturaTerm - outH);

    for (size_t i = 0; i < caixaFinal.size(); ++i) {
        Aparencia::moverCursor(startX, startY + i);
        std::cout << caixaFinal[i];
    }
    std::cout << std::flush;
}

void TelaDiarioRaycaster::renderizarPopupInspecaoComArte(const std::string& titulo, const std::vector<std::string>& arte, const std::vector<std::string>& info, const std::string& subtitulo) {
    std::vector<std::string> textoPopup;

    if (!subtitulo.empty()) {
        textoPopup.push_back(Aparencia::cor(Cor::CIANO) + " === " + subtitulo + " ===" + Aparencia::cor(Cor::RESET));
        textoPopup.push_back("");
    }
    for (const auto& l : info) textoPopup.push_back(l);

    int maxArt = std::max(arte.size(), textoPopup.size());
    int largArteBase = 0;
    for (const auto& l : arte) {
        int w = Aparencia::obterComprimentoVisual(l);
        if (w > largArteBase) largArteBase = w;
    }
    if (largArteBase > 0) largArteBase += 4;
    if (arte.empty()) largArteBase = 0;

    std::vector<std::string> linhasFinal;
    for (int i = 0; i < maxArt; ++i) {
        std::string rL = (i < (int)arte.size()) ? arte[i] : "";
        std::string cL = (i < (int)textoPopup.size()) ? textoPopup[i] : "";
        int lenR = Aparencia::obterComprimentoVisual(rL);
        if (lenR < largArteBase) rL += std::string(largArteBase - lenR, ' ');
        linhasFinal.push_back((largArteBase > 0 ? " " : "") + rL + " " + cL);
    }
    linhasFinal.push_back("");
    linhasFinal.push_back("   " + Aparencia::cor(Cor::VERDE) + "> Voltar" + Aparencia::cor(Cor::BRANCO) + "\033[48;2;25;25;25m");

    std::vector<std::string> caixaFinal = TelaBase::criarCaixa(linhasFinal, titulo, 0, Cor::AMARELO, "\033[48;2;25;25;25m");
    int outW = Aparencia::obterComprimentoVisual(caixaFinal[0]);
    int outH = caixaFinal.size();

    int startX = (Aparencia::obterLarguraTerminal() - outW) / 2;
    int alturaTerm = Aparencia::obterAlturaTerminal();
    int startY = (alturaTerm - outH) / 2;
    if (startX < 0) startX = 0;
    if (startY < 8) startY = 8;
    if (startY + outH > alturaTerm) startY = std::max(0, alturaTerm - outH);

    for (size_t i = 0; i < caixaFinal.size(); ++i) {
        Aparencia::moverCursor(startX, startY + i);
        std::cout << caixaFinal[i];
    }
    std::cout << std::flush;
}
