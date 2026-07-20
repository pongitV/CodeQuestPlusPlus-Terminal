#include "Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Perspectiva/TelasBase/TelaBase.h"
#include <iostream>
#include <algorithm>
#include <sstream>

namespace {
    int popupMinLarguraAtual = 0;
    int popupMinAlturaAtual = 0;
    int ultimoPopupX = -1;
    int ultimoPopupY = -1;
    int ultimoPopupW = -1;
    int ultimoPopupH = -1;
    std::string ultimoPopupBg = "\033[48;2;25;25;25m";
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

void Aparencia::limparPopupAnterior() {
    if (ultimoPopupH <= 0 || ultimoPopupW <= 0) return;
    std::string linha = ultimoPopupBg + std::string(ultimoPopupW, ' ') + "\033[0m";
    for (int i = 0; i < ultimoPopupH; ++i) {
        moverCursor(ultimoPopupX, ultimoPopupY + i);
        std::cout << linha;
    }
    std::cout << std::flush;
    ultimoPopupX = -1; ultimoPopupY = -1; ultimoPopupW = -1; ultimoPopupH = -1;
}

void Aparencia::definirUltimoPopup(int x, int y, int w, int h, const std::string& bgAnsi) {
    ultimoPopupX = x; ultimoPopupY = y; ultimoPopupW = w; ultimoPopupH = h; ultimoPopupBg = bgAnsi;
}

void Aparencia::exibirPopup(const std::string& titulo, const std::vector<std::string>& texto, Cor corTema, const std::vector<std::string>& arteOriginal) {
    limparPopupAnterior();

    std::vector<std::string> arte = arteOriginal;
    if (static_cast<int>(arte.size()) > 10) {
        arte = reduzirEscalaAscii(arteOriginal, FATOR_COMPRESSAO_GLOBAL, FATOR_COMPRESSAO_GLOBAL);
    }

    std::vector<std::string> linhasTexto = texto;

    std::string bgPopup = "\033[48;2;25;25;25m";
    std::vector<std::string> caixa = TelaBase::criarCaixaComArte(arte, linhasTexto, titulo, 0, corTema, bgPopup);

    int finalBoxW = obterComprimentoVisual(caixa[0]);
    int finalBoxH = caixa.size();

    int larguraTerm = obterLarguraTerminal();
    int alturaTerm = obterAlturaTerminal();
    int startX = (larguraTerm - finalBoxW) / 2;
    int startY = (alturaTerm - finalBoxH) / 2;
    if (startY < 0) startY = 0;

    for (int i = 0; i < finalBoxH; ++i) {
        moverCursor(startX, startY + i);
        std::cout << caixa[i];
    }
    std::cout << std::flush;

    definirUltimoPopup(startX, startY, finalBoxW, finalBoxH, bgPopup);

    ControleDeInput::limparBuffer();
    while (true) {
        char c = ControleDeInput::lerTecla();
        if (c == '\r' || c == '\n') break;
    }
}

void Aparencia::renderizarCaixaPopupAnimada(const std::vector<std::string>& caixa, int startX, int startY, bool /*animar*/) {
    if (caixa.empty()) return;
    for (int i = 0; i < static_cast<int>(caixa.size()); ++i) {
        moverCursor(startX, startY + i);
        std::cout << caixa[i];
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

void Aparencia::removerCaixaPopupAnimada(int finalBoxWidth, int finalBoxHeight, int startX, int startY, bool /*animar*/) {
    if (finalBoxWidth <= 0 || finalBoxHeight <= 0) return;
    for (int i = 0; i < finalBoxHeight; ++i) {
        moverCursor(startX, startY + i);
        std::cout << "\033[0m" << std::string(finalBoxWidth, ' ') << "\033[0m";
    }
    std::cout << std::flush;
}
