#include "Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <sstream>

namespace {
    int popupMinLarguraAtual = 0;
    int popupMinAlturaAtual = 0;
}

void Aparencia::iniciarInteracaoPopup() {
    popupMinLarguraAtual = 0;
    popupMinAlturaAtual = 0;
}
void Aparencia::atualizarMinTamanhoPopup(int largura, int altura) {
    if (largura > popupMinLarguraAtual) popupMinLarguraAtual = largura;
    if (altura > popupMinAlturaAtual) popupMinAlturaAtual = altura;
}
int Aparencia::obterMinLarguraPopup() { return popupMinLarguraAtual; }
int Aparencia::obterMinAlturaPopup() { return popupMinAlturaAtual; }

void Aparencia::exibirPopup(const std::string& titulo, const std::vector<std::string>& texto, Cor corTema, const std::vector<std::string>& arteOriginal) {
    std::vector<std::string> arte = arteOriginal;
    if (static_cast<int>(arte.size()) > 10) {
        arte = reduzirEscalaAscii(arteOriginal, FATOR_COMPRESSAO_GLOBAL, FATOR_COMPRESSAO_GLOBAL);
    }

    int larguraArte = 0;
    for (const auto& l : arte) {
        int len = obterComprimentoVisual(l);
        if (len > larguraArte) larguraArte = len;
    }

    std::vector<std::string> linhasTexto = texto;
    
    int minBoxHeight = Aparencia::obterMinAlturaPopup() - 2;
    int minTotalWidth = Aparencia::obterMinLarguraPopup() - 4;
    if (minBoxHeight < 0) minBoxHeight = 0;
    if (minTotalWidth < 0) minTotalWidth = 0;

    while (std::max(arte.size(), linhasTexto.size() + 2) < static_cast<size_t>(minBoxHeight)) {
        linhasTexto.push_back("");
    }
    linhasTexto.push_back("");
    linhasTexto.push_back(cor(Cor::CINZA) + "[ Pressione ENTER para continuar ]" + cor(Cor::RESET));

    int larguraTexto = 0;
    for (const auto& l : linhasTexto) {
        int len = obterComprimentoVisual(l);
        if (len > larguraTexto) larguraTexto = len;
    }

    int totalWidth = larguraArte + (larguraArte > 0 ? 3 : 0) + larguraTexto; 
    
    if (totalWidth < minTotalWidth) {
        larguraTexto += (minTotalWidth - totalWidth);
        totalWidth = minTotalWidth;
    }

    int boxHeight = std::max(static_cast<int>(arte.size()), static_cast<int>(linhasTexto.size()));

    std::vector<std::string> caixa;
    std::string corStr = cor(corTema);
    std::string resetStr = cor(Cor::RESET);

    std::string top = "╔";
    int tituloLen = obterComprimentoVisual(titulo);
    
    if (tituloLen > 0) {
        top += "══ " + titulo + " ";
        int restantes = totalWidth + 2 - (tituloLen + 4);
        if (restantes < 0) restantes = 0;
        for (int i = 0; i < restantes; ++i) top += "═";
    } else {
        for (int i = 0; i < totalWidth + 2; ++i) top += "═";
    }
    top += "╗";
    caixa.push_back(corStr + top + resetStr);

    for (int i = 0; i < boxHeight; ++i) {
        std::string linhaArte = (i < static_cast<int>(arte.size())) ? arte[i] : "";
        int padArte = larguraArte - obterComprimentoVisual(linhaArte);
        std::string artePart = linhaArte + std::string(padArte > 0 ? padArte : 0, ' ');

        std::string linhaTexto = (i < static_cast<int>(linhasTexto.size())) ? linhasTexto[i] : "";
        int padTexto = larguraTexto - obterComprimentoVisual(linhaTexto);
        std::string textoPart = linhaTexto + std::string(padTexto > 0 ? padTexto : 0, ' ');

        if (larguraArte > 0) {
            caixa.push_back(corStr + "║ " + resetStr + artePart + corStr + " ║ " + resetStr + textoPart + corStr + " ║" + resetStr);
        } else {
            caixa.push_back(corStr + "║ " + resetStr + textoPart + corStr + " ║" + resetStr);
        }
    }

    std::string bottom = "╚";
    for (int i = 0; i < totalWidth + 2; ++i) bottom += "═";
    bottom += "╝";
    caixa.push_back(corStr + bottom + resetStr);

    int finalBoxWidth = obterComprimentoVisual(caixa[0]);
    int finalBoxHeight = caixa.size();
    Aparencia::atualizarMinTamanhoPopup(finalBoxWidth, finalBoxHeight);
    
    int larguraTerm = obterLarguraTerminal();
    int alturaTerm = obterAlturaTerminal();
    int startX = (larguraTerm - finalBoxWidth) / 2;
    int startY = (alturaTerm - finalBoxHeight) / 2;
    if (startX < 0) startX = 0;
    if (startY < 0) startY = 0;
    
    renderizarCaixaPopupAnimada(caixa, startX, startY, true);
    
    ControleDeInput::limparBuffer();
    while (true) {
        char c = ControleDeInput::lerTecla();
        if (c == '\r' || c == '\n') break;
    }
}

void Aparencia::renderizarCaixaPopupAnimada(const std::vector<std::string>& caixa, int startX, int startY, bool animar) {
    if (caixa.empty()) return;
    int finalBoxHeight = caixa.size();
    std::string bgPopup = "\033[48;2;15;15;15m"; 
    
    auto formatarLinha = [&](const std::string& linhaOriginal) {
        std::string linha = linhaOriginal;
        size_t pos = 0;
        while ((pos = linha.find("\033[0m", pos)) != std::string::npos) {
            linha.replace(pos, 4, "\033[0m" + bgPopup);
            pos += 4 + bgPopup.length(); 
        }
        return bgPopup + linha + "\033[0m"; 
    };

    if (animar) {
        int metade = finalBoxHeight / 2;
        for (int expansao = 0; expansao <= metade; expansao++) {
            int inicioSlice = metade - expansao;
            int fimSlice = metade + expansao;
            if (fimSlice >= finalBoxHeight) fimSlice = finalBoxHeight - 1;

            moverCursor(startX, startY + inicioSlice);
            std::cout << formatarLinha(caixa[0]);

            for (int i = inicioSlice + 1; i < fimSlice; i++) {
                moverCursor(startX, startY + i);
                std::cout << formatarLinha(caixa[i]);
            }

            if (fimSlice > inicioSlice) {
                moverCursor(startX, startY + fimSlice);
                std::cout << formatarLinha(caixa[finalBoxHeight - 1]);
            }

            std::cout << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
        }
    }

    for (int i = 0; i < finalBoxHeight; ++i) {
        moverCursor(startX, startY + i);
        std::cout << formatarLinha(caixa[i]);
    }
    std::cout << std::flush;
}

int Aparencia::lerInteiroEmPopupFlutuante(const std::string& mensagem, int limiteMin, int limiteMax, Cor corTema) {
    int termW = obterLarguraTerminal();
    int termH = obterAlturaTerminal();
    int msgLen = obterComprimentoVisual(mensagem);
    int boxW = msgLen + 8;
    
    int startX = std::max(0, (termW - boxW) / 2);
    int startY = std::max(0, termH / 2);
    
    std::string corBox = cor(corTema);
    std::string reset = cor(Cor::RESET);
    std::string bg = "\033[48;2;15;15;15m";

    moverCursor(startX, startY - 1);
    std::string top = "╔"; for(int i = 0; i < boxW - 2; ++i) top += "═"; top += "╗";
    std::cout << bg << corBox << top << reset;
    
    moverCursor(startX, startY);
    std::cout << bg << corBox << "║ " << reset << bg << mensagem << "    " << corBox << "║" << reset;
    
    moverCursor(startX, startY + 1);
    std::string bottom = "╚"; for(int i = 0; i < boxW - 2; ++i) bottom += "═"; bottom += "╝";
    std::cout << bg << corBox << bottom << reset;

    moverCursor(startX + 2 + msgLen, startY);
    std::cout << bg << reset;
    int resultado = ControleDeInput::lerInteiroComLimites("", limiteMin, limiteMax, false);
    std::cout << "\033[0m";
    return resultado;
}
