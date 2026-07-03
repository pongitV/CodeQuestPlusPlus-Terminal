#include "TelaPauseIDE.h"
#include <iostream>
#include <string>
#include "../TemaIDE.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Entidades/Personagem.h"
#include "../../TelasBase/Menu/TelaMenu.h"
#include "../../GerenciadorPerspectiva.h"

int TelaPauseIDE::renderizarMenuPause() {
    std::cout << "\033[?25l";
    Aparencia::limparTela();
    TelaMenu::exibirPainelLogoJogo("JOGO EM PAUSE", false);
    std::cout << "\n";

    std::vector<std::string> opcoes = {
        "Voltar ao jogo",
        "Configuracoes",
        "Salvar e sair"
    };

    return ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);
}

int TelaPauseIDE::renderizarMenuConfiguracoes(Personagem* jogador) {
    Aparencia::limparTela();
    TelaMenu::exibirPainelLogoJogo("CONFIGURACOES", false);
    std::cout << "\n";

    std::string difStr;
    switch (jogador->obterDificuldade()) {
        case DificuldadeJogo::Facil: difStr = Aparencia::cor(Cor::VERDE) + "Facil" + Aparencia::cor(Cor::RESET); break;
        case DificuldadeJogo::Normal: difStr = Aparencia::cor(Cor::AMARELO) + "Normal" + Aparencia::cor(Cor::RESET); break;
        case DificuldadeJogo::Dificil: difStr = Aparencia::cor(Cor::VERMELHO) + "Dificil" + Aparencia::cor(Cor::RESET); break;
    }

    std::string statusParry = jogador->obterParryAtivado() ? Aparencia::cor(Cor::VERDE) + "LIGADO" + Aparencia::cor(Cor::RESET) : Aparencia::cor(Cor::VERMELHO) + "DESLIGADO" + Aparencia::cor(Cor::RESET);
    std::string tipoParryStr = jogador->obterParryModerno() ? Aparencia::cor(Cor::CIANO) + "Movimento (Moderno)" + Aparencia::cor(Cor::RESET) : Aparencia::cor(Cor::VERMELHO) + "Digitacao (Antigo)" + Aparencia::cor(Cor::RESET);

    std::vector<std::string> opcoesConfig = {
        "Dificuldade do Mundo: " + difStr,
        "Sistema de PARRY: " + statusParry,
        "Metodo do PARRY: " + tipoParryStr,
        "Aparencia do Jogador no Mapa",
        "Cor de Fundo do Terminal",
        "Sensibilidade do Mouse",
        "Voltar"
    };

    return ControleDeInput::lerSelecaoMenuComSetas(opcoesConfig, true);
}

int TelaPauseIDE::renderizarMenuAparencia(Personagem* jogador) {
    Aparencia::limparTela();
    TelaMenu::exibirPainelLogoJogo("APARENCIA NO MAPA", false);
    std::cout << "\n";

    std::string corAtualStr;
    switch (jogador->obterCorJogador()) {
        case Cor::VERDE: corAtualStr = Aparencia::cor(Cor::VERDE) + "Verde" + Aparencia::cor(Cor::RESET); break;
        case Cor::AZUL: corAtualStr = Aparencia::cor(Cor::AZUL) + "Azul" + Aparencia::cor(Cor::RESET); break;
        case Cor::AMARELO: corAtualStr = Aparencia::cor(Cor::AMARELO) + "Amarelo" + Aparencia::cor(Cor::RESET); break;
        case Cor::BRANCO: corAtualStr = Aparencia::cor(Cor::BRANCO) + "Branco" + Aparencia::cor(Cor::RESET); break;
        case Cor::MAGENTA: corAtualStr = Aparencia::cor(Cor::MAGENTA) + "Magenta" + Aparencia::cor(Cor::RESET); break;
        case Cor::CIANO: corAtualStr = Aparencia::cor(Cor::CIANO) + "Ciano" + Aparencia::cor(Cor::RESET); break;
        case Cor::VERMELHO: corAtualStr = Aparencia::cor(Cor::VERMELHO) + "Vermelho" + Aparencia::cor(Cor::RESET); break;
        default: corAtualStr = Aparencia::cor(Cor::BRANCO) + "Padrao" + Aparencia::cor(Cor::RESET); break;
    }

    std::vector<std::string> opcoesAparencia = {
        "Cor do Jogador: " + corAtualStr,
        "Icone do Jogador: " + Aparencia::cor(jogador->obterCorJogador()) + std::string(1, jogador->obterIconeJogador()) + Aparencia::cor(Cor::RESET),
        "Voltar"
    };

    return ControleDeInput::lerSelecaoMenuComSetas(opcoesAparencia, true);
}

int TelaPauseIDE::renderizarMenuFundo(int corFundoAtualIndex) {
    Aparencia::limparTela();
    TelaMenu::exibirPainelLogoJogo("COR DE FUNDO", false);
    std::cout << "\n";

    std::vector<std::string> nomesFundos = {
        "Preto (Padrao)", "Cinza Escuro", "Azul Escuro",
        "Vermelho Escuro", "Verde Escuro", "Roxo Escuro"
    };
    std::vector<std::string> opcoesFundo;
    for (int i = 0; i < 6; ++i) {
        if (i == corFundoAtualIndex) opcoesFundo.push_back(nomesFundos[i] + Aparencia::cor(Cor::VERDE) + " [ATIVO]" + Aparencia::cor(Cor::RESET));
        else opcoesFundo.push_back(nomesFundos[i]);
    }
    opcoesFundo.push_back("Voltar");

    return ControleDeInput::lerSelecaoMenuComSetas(opcoesFundo, true);
}

int TelaPauseIDE::renderizarMenuSensibilidade(int percX, int percY) {
    Aparencia::limparTela();
    TelaMenu::exibirPainelLogoJogo("SENSIBILIDADE", false);
    std::cout << "\n";

    std::vector<std::string> opcoesSens = {
        "Horizontal (X): " + std::to_string(percX) + "%  [ ENTER p/ Digitar Novo Valor ]",
        "Vertical (Y): " + std::to_string(percY) + "%  [ ENTER p/ Digitar Novo Valor ]",
        "Voltar"
    };

    return ControleDeInput::lerSelecaoMenuComSetas(opcoesSens, true);
}

bool TelaPauseIDE::renderizarConfirmacaoSalvar(Personagem* jogador) {
    std::vector<std::string> opcoesSimNao = { "NAO", "SIM" };
    std::cout << "\n";
    Aparencia::imprimirCentralizado("[AVISO]: Deseja salvar o jogo e voltar para o menu principal?");
    std::cout << "\n";
    if (ControleDeInput::lerSelecaoMenuComSetas(opcoesSimNao, true) == 1) {
        Aparencia::imprimirCentralizado("[AVISO]: Tem certeza?");
        std::cout << "\n";
        if (ControleDeInput::lerSelecaoMenuComSetas(opcoesSimNao, true) == 1) {
            return true;
        }
    }
    return false;
}
