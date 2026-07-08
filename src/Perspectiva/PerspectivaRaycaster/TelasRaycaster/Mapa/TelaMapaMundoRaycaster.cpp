#include "TelaMapaMundoRaycaster.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>
#include "../../../TelasBase/TelaBase.h"
#include "../../../TelasBase/MapaMundial/TelaMapaMundoLayout.h"
#include "../../EngineRaycaster/Raycaster.h"
#include "../../../../Core/Utilidades/Aparencia.h"

namespace {

struct RGB { int r, g, b; };

RGB corDoChar(char c) {
    if (c == ' ') return {0, 0, 0};
    if (c == '_' || c == '-' || c == '|' || c == '/' || c == '\\'
        || c == '>' || c == '<' || c == '?' || c == '`' || c == '!')
        return {70, 170, 230};
    if (c == '#')
        return {25, 100, 200};
    return {60, 180, 55};
}

}

void TelaMapaMundoRaycaster::renderizarPopup(const std::vector<std::string>& arte, const std::vector<std::string>& lugares, int selecao, bool redesenhoCompleto) {
    int larguraTerm = Aparencia::obterLarguraTerminal();
    int alturaTerm = Aparencia::obterAlturaTerminal();

    int artH = (int)arte.size();
    int artW = 0;
    for (const auto& l : arte) {
        int w = (int)l.length();
        if (w > artW) artW = w;
    }

    int listH = 1 + (int)lugares.size();
    int listX = artW + 4;
    int contW = listX + 24;
    int contH = std::max(artH, listH) + 2;

    std::string bgDark = "\033[48;2;25;25;25m";
    std::string resetBg = "\033[0m";

    std::vector<std::string> conteudo(contH, std::string(contW, ' '));
    auto caixa = TelaBase::criarCaixa(conteudo, " MAPA MUNDI ", 0, Cor::CIANO, bgDark);

    int outW = Aparencia::obterComprimentoVisual(caixa[0]);
    int outH = (int)caixa.size();

    int startX = (larguraTerm - outW) / 2;
    int startY = (alturaTerm - outH) / 2;
    if (startX < 0) startX = 0;
    if (startY < 8) startY = 8;
    if (startY + outH > alturaTerm) startY = std::max(0, alturaTerm - outH);

    std::cout << "\033[?25l";

    if (redesenhoCompleto) {
        Raycaster::restaurarUltimoQuadro();

        for (int i = 0; i < outH; ++i) {
            Aparencia::moverCursor(startX, startY + i);
            std::cout << caixa[i];
        }

        int innerX = startX + 2;
        int innerY = startY + 2;

        for (int y = 0; y < artH; ++y) {
            for (int x = 0; x < (int)arte[y].length(); ++x) {
                char c = arte[y][x];
                if (c == ' ') continue;
                RGB cor = corDoChar(c);
                std::string ansi = "\033[48;2;" + std::to_string(cor.r) + ";" + std::to_string(cor.g) + ";" + std::to_string(cor.b) + "m";
                Aparencia::moverCursor(innerX + x, innerY + y);
                std::cout << ansi << " " << resetBg;
            }
        }
    }

    int innerX = startX + 2;
    int innerY = startY + 2;
    int listOx = innerX + listX;
    int listOy = innerY;

    auto escreverOpcao = [&](int idx, const std::string& texto) {
        Aparencia::moverCursor(listOx, listOy + idx);
        if (idx == selecao)
            std::cout << bgDark << Aparencia::cor(Cor::VERDE) << " > " << Aparencia::cor(Cor::BRANCO) << texto << resetBg;
        else
            std::cout << bgDark << "   " << texto << resetBg;
    };

    escreverOpcao(0, "[ VOLTAR ]");
    for (size_t i = 0; i < lugares.size(); ++i)
        escreverOpcao(1 + (int)i, lugares[i]);

    std::cout << std::flush;
}
