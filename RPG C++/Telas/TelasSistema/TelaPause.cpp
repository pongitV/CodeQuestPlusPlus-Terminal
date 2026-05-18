#include "TelaPause.h"
#include "TelaMenu.h"
#include "../../Sistemas/SistemaPersonagem.h"
#include "../../Sistemas/SistemaSave.h"
#include "../../Utilidades/Aparencia.h"
#include "../../Utilidades/ControleDeInput.h"

#include <iostream>
#include <vector>
#include <string>

void TelaPause::exibirMenuConfiguracoes(SistemaPersonagem* jogador) {
    bool continuar = true;
    do {
        Aparencia::limparTela();
        TelaMenu::exibirLogoDoJogo("CONFIGURACOES", false);
        
        std::string statusParry = jogador->obterParryAtivado() ? Aparencia::cor(Cor::VERDE) + "Ligado" + Aparencia::cor(Cor::RESET) : Aparencia::cor(Cor::CINZA) + "Desligado" + Aparencia::cor(Cor::RESET);

        std::vector<std::string> opcoes = {
            "Ligar/Desligar Parry: " + statusParry,
            "Mudar Nome do Personagem",
            "Mudar Cor do Icone no Mapa",
            "VOLTAR"
        };

        std::cout << "\n";
        int escolha = ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);

        if (escolha == 0) {
            jogador->definirParryAtivado(!jogador->obterParryAtivado());
        } else if (escolha == 1) {
            Aparencia::limparTela();
            TelaMenu::exibirLogoDoJogo("MUDAR NOME", false);
            std::cout << "\n";
            Aparencia::imprimirCentralizado("Digite o novo nome (ou 0 para cancelar):");
            std::cout << "\n";
            std::string novoNome = ControleDeInput::lerEntradaProtegida("> ");
            if (novoNome != "0" && !novoNome.empty()) {
                jogador->definirNome(novoNome);
            }
        } else if (escolha == 2) {
            Aparencia::limparTela();
            TelaMenu::exibirLogoDoJogo("COR DO ICONE", false);
            std::vector<std::string> coresMenu = {
                Aparencia::cor(Cor::VERDE) + "Verde" + Aparencia::cor(Cor::RESET),
                Aparencia::cor(Cor::CIANO) + "Ciano" + Aparencia::cor(Cor::RESET),
                Aparencia::cor(Cor::AMARELO) + "Amarelo" + Aparencia::cor(Cor::RESET),
                Aparencia::cor(Cor::VERMELHO) + "Vermelho" + Aparencia::cor(Cor::RESET),
                Aparencia::cor(Cor::MAGENTA) + "Magenta" + Aparencia::cor(Cor::RESET),
                Aparencia::cor(Cor::BRANCO) + "Branco" + Aparencia::cor(Cor::RESET)
            };
            std::cout << "\n";
            Aparencia::imprimirCentralizado("Escolha a cor do seu icone (@) no mapa:");
            std::cout << "\n";
            int corEscolhida = ControleDeInput::lerSelecaoMenuComSetas(coresMenu, true);
            switch(corEscolhida) {
                case 0: jogador->definirCorIconeMapa(Cor::VERDE); break;
                case 1: jogador->definirCorIconeMapa(Cor::CIANO); break;
                case 2: jogador->definirCorIconeMapa(Cor::AMARELO); break;
                case 3: jogador->definirCorIconeMapa(Cor::VERMELHO); break;
                case 4: jogador->definirCorIconeMapa(Cor::MAGENTA); break;
                case 5: jogador->definirCorIconeMapa(Cor::BRANCO); break;
            }
        } else if (escolha == 3) {
            continuar = false;
        }
    } while (continuar);
}

void TelaPause::exibirMenuDePausa(SistemaPersonagem* jogador) {
    bool continuar = true;
    do {
        Aparencia::limparTela();
        TelaMenu::exibirLogoDoJogo("JOGO PAUSADO", false);
        
        std::vector<std::string> opcoes = {
            "Continuar",
            "Configuracoes",
            "Salvar e Sair"
        };

        std::cout << "\n";
        int escolha = ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);

        if (escolha == 0) {
            continuar = false;
        } else if (escolha == 1) {
            exibirMenuConfiguracoes(jogador);
        } else if (escolha == 2) {
            std::vector<std::string> opcoesSimNao = { "NAO", "SIM" };
            std::cout << "\n";
            Aparencia::imprimirCentralizado("[AVISO]: Deseja salvar o jogo e voltar para o menu principal?");
            std::cout << "\n";
            if (ControleDeInput::lerSelecaoMenuComSetas(opcoesSimNao, true) == 1) {
                SistemaSave::salvarJogo(jogador);
                jogador->definirVoltarProMenu(true);
                continuar = false;
            }
        }
    } while (continuar && !jogador->obterVoltarProMenu());
}
