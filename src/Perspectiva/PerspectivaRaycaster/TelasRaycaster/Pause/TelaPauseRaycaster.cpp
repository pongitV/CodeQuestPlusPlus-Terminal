#include "TelaPauseRaycaster.h"
#include "../Utils/MenuRaycasterLayout.h"
#include "../Utils/MenuRaycasterUtils.h"
#include "../../../TelasBase/Menu/TelaBaseMenu.h"
#include "../../../../Core/Utilidades/Aparencia.h"
#include "../../../../Core/Utilidades/ControleDeInput.h"
#include "../../../../Entidades/Personagem.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
#include <string>
#include <algorithm>

static int renderizarMenuSimples(const std::string& titulo, const std::vector<std::string>& opcoes) {
    int larguraConsole = Aparencia::obterLarguraTerminal();
    int selecaoAtual = 0;
    ControleDeInput::limparBuffer();
    
    while (true) {
        MenuRaycasterUtils::incrementarCicloDia();
        std::ostringstream buffer;
        MenuRaycasterUtils::exibirFundo3D(buffer);

        int espTit = TelaBaseMenu::calcularOffsetCentral(titulo, larguraConsole);
        
        int boxW = Aparencia::obterComprimentoVisual(titulo) + 10;
        for (const auto& op : opcoes) {
            boxW = std::max(boxW, Aparencia::obterComprimentoVisual(op) + 10);
        }
        if (boxW > larguraConsole - 4) boxW = larguraConsole - 4;
        
        int boxX = TelaBaseMenu::calcularOffsetCentral(boxW, larguraConsole);
        int yBase = 8;
        
        TelaBaseMenu::desenharCaixaPreta(buffer, yBase - 2, boxX, boxW, (int)opcoes.size() + 4);
        MenuRaycasterUtils::sobreporTextoAbsoluto(buffer, "\033[38;2;255;215;0m" + titulo + "\033[0m", yBase - 1, espTit);

        for (int i = 0; i < (int)opcoes.size(); ++i) {
            std::string icone = (i == selecaoAtual) ? "> " : "  ";
            std::string cor = (i == selecaoAtual) ? "\033[38;2;0;255;0m" : "\033[38;2;120;120;120m";
            int espOpcao = TelaBaseMenu::calcularOffsetCentral(opcoes[i] + "  ", larguraConsole);
            MenuRaycasterUtils::sobreporTextoAbsoluto(buffer, cor + icone + opcoes[i] + "\033[0m", yBase + 1 + i, espOpcao - 2);
        }

        MenuRaycasterUtils::flushFrameParaConsole(buffer.str());

        if (!ControleDeInput::teclaPressionada()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            continue;
        }

        unsigned char tecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
        if (tecla == 224 || tecla == 0 || tecla == '\033') {
            unsigned char proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (proxTecla == '[') proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (proxTecla == 72 || proxTecla == 'A') tecla = 'w';
            else if (proxTecla == 80 || proxTecla == 'B') tecla = 's';
            else if (proxTecla == 27) return -1;
        }

        if (tecla == 'w' || tecla == 'W') {
            selecaoAtual = (selecaoAtual - 1 + (int)opcoes.size()) % (int)opcoes.size();
        } else if (tecla == 's' || tecla == 'S') {
            selecaoAtual = (selecaoAtual + 1) % (int)opcoes.size();
        } else if (tecla == '\r' || tecla == '\n') {
            return selecaoAtual;
        }
    }
}

int TelaPauseRaycaster::renderizarMenuPause() {
    std::vector<std::string> opcoes = {
        "VOLTAR AO JOGO",
        "CONFIGURACOES",
        "SAIR DO JOGO"
    };
    int res = renderizarMenuSimples("[ PAUSE ]", opcoes);
    if (res == -1) return 0; 
    return res;
}

int TelaPauseRaycaster::renderizarMenuConfiguracoes(Personagem* jogador) {
    std::string difStr;
    switch (jogador->obterDificuldade()) {
        case DificuldadeJogo::Facil: difStr = "\033[38;2;100;255;100mFACIL\033[0m"; break;
        case DificuldadeJogo::Normal: difStr = "\033[38;2;255;255;100mNORMAL\033[0m"; break;
        case DificuldadeJogo::Dificil: difStr = "\033[38;2;255;100;100mDIFICIL\033[0m"; break;
    }

    std::string parryState = "\033[38;2;255;100;100mDESLIGADO\033[0m"; // Vermelho
    if (jogador->obterParryAtivado()) {
        if (jogador->obterParryModerno()) {
            parryState = "\033[38;2;100;255;100mMOVIMENTO\033[0m"; // Verde
        } else {
            parryState = "\033[38;2;100;100;255mDIGITACAO\033[0m"; // Azul
        }
    }
    std::vector<std::string> opcoes = {
        "ALTERAR DIFICULDADE: " + difStr,
        "ALTERNAR PARRY: " + parryState,
        "APARENCIA / SKIN",
        "FUNDO DO TERMINAL",
        "SENSIBILIDADE DA CAMERA",
        "VOLTAR"
    };
    int res = renderizarMenuSimples("[ CONFIGURACOES ]", opcoes);
    
    if (res == -1) return 6;
    if (res == 0) return 0;
    if (res == 1) {
        if (!jogador->obterParryAtivado()) {
            // DESLIGADO -> MOVIMENTO
            jogador->definirParryModerno(true);
            return 1; // TelaPause toggles Ativado to true
        } else if (jogador->obterParryModerno()) {
            // MOVIMENTO -> DIGITACAO
            return 2; // TelaPause toggles Moderno to false
        } else {
            // DIGITACAO -> DESLIGADO
            return 1; // TelaPause toggles Ativado to false
        }
    }
    if (res == 2) return 3;
    if (res == 3) return 4;
    if (res == 4) return 5;
    if (res == 5) return 6;
    
    return 6;
}

int TelaPauseRaycaster::renderizarMenuAparencia(Personagem*) { return 2; }
int TelaPauseRaycaster::renderizarMenuFundo(int) { return 6; }
int TelaPauseRaycaster::renderizarMenuSensibilidade(int percX, int percY) {
    std::vector<std::string> opcoes = {
        "EIXO X (Yaw): " + std::to_string(percX) + "%",
        "EIXO Y (Pitch): " + std::to_string(percY) + "%",
        "VOLTAR"
    };
    int res = renderizarMenuSimples("[ SENSIBILIDADE ]", opcoes);
    if (res == -1) return 2;
    return res;
}
