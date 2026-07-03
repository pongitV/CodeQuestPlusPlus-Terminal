#include "TelaTutorial.h"
#include "TelaMenu.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>

#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Core/Utilidades/GeradorAleatorio.h"
#include "../../../Sistemas/Combate/Parry.h"

namespace {
    void exibirInfoBox(const std::string& infoBox) {
        if (!infoBox.empty()) {
            std::string cleanInfoBox = infoBox;
            size_t pos = 0;
            while ((pos = cleanInfoBox.find('|', pos)) != std::string::npos) {
                cleanInfoBox.replace(pos, 1, "\xe2\x95\x91");
                pos += 3;
            }

            int infoLength = Aparencia::obterComprimentoVisual(infoBox);
            std::string tracos = "";
            for (int i = 0; i < infoLength - 2; ++i) tracos += "\xe2\x95\x90";

            std::string margem = Aparencia::espacosParaCentralizar(infoLength);
            std::cout << margem << "\xe2\x95\x94" << tracos << "\xe2\x95\x97\n";
            std::cout << margem << cleanInfoBox << "\n";
            std::cout << margem << "\xe2\x95\x9a" << tracos << "\xe2\x95\x9d\n\n";
        }
    }

    void rodarTutorialDigitacao(const std::string& infoBox) {
        struct NivelTutorial {
            std::string nomeInimigo;
            int digitos;
            int tempoLimiteMs;
        };

        std::vector<NivelTutorial> niveis = {
            {"(Nivel 1)", 3, 3000},
            {"(Nivel 2)", 4, 2500},
            {"(Nivel 3)", 5, 2000},
            {"(Nivel Extra)", 6, 1500}
        };

        Aparencia::limparTela();
        TelaMenu::exibirPainelLogoJogo("TUTORIAL DE PARRY DIGITACAO", false);
        if (infoBox.empty()) std::cout << "\n";
        else exibirInfoBox(infoBox);

        std::vector<std::string> explicacao = {
            "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90 COMO FUNCIONA O PARRY DIGITACAO \xe2\x95\x90\xe2\x95\x90\xe2\x95\x90",
            "Uma sequencia de numeros aparecera na tela com um limite de tempo.",
            "Digite os numeros rapidamente na sequencia correta e pressione ENTER.",
            "Se for rapido o suficiente e nao errar, o dano sera reduzido/anulado!"
        };

        Aparencia::imprimirBlocoCentralizado(explicacao);
        std::cout << "\n";
        ControleDeInput::aguardarEnter();

        for (size_t i = 0; i < niveis.size(); ++i) {
            Aparencia::limparTela();
            TelaMenu::exibirPainelLogoJogo("TUTORIAL DE PARRY DIGITACAO - " + niveis[i].nomeInimigo);

            if (infoBox.empty()) std::cout << "\n";
            else exibirInfoBox(infoBox);

            Aparencia::imprimirCentralizado("Inimigo: " + niveis[i].nomeInimigo);
            Aparencia::imprimirCentralizado("Sequencia: " + std::to_string(niveis[i].digitos) + " digitos | Tempo limite: " + std::to_string(niveis[i].tempoLimiteMs / 1000) + "s");
            ControleDeInput::aguardarEnter();

            int acertos = 0;
            for (int teste = 1; teste <= 5; ++teste) {
                std::cout << "\n";
                Aparencia::imprimirCentralizado("[ Teste " + std::to_string(teste) + "/5 ] Prepare-se...");
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                std::string sequencia = "";
                for(int d = 0; d < niveis[i].digitos; d++){
                    sequencia += std::to_string(GeradorAleatorio::obterInteiro(0, 9));
                }

                Aparencia::imprimirCentralizado("O " + niveis[i].nomeInimigo + " ataca! DIGITE: " + Aparencia::cor(Cor::CIANO) + sequencia + Aparencia::cor(Cor::RESET));

                auto inicio = std::chrono::steady_clock::now();
                std::string entrada = ControleDeInput::lerEntradaProtegida("> ");
                auto fim = std::chrono::steady_clock::now();

                int tempoGastoMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(fim - inicio).count());

                if (entrada == sequencia) {
                    if (tempoGastoMs <= niveis[i].tempoLimiteMs) {
                        Aparencia::imprimirCentralizado(Aparencia::cor(Cor::VERDE) + "Parry Perfeito! (" + std::to_string(tempoGastoMs) + "ms)" + Aparencia::cor(Cor::RESET));
                        acertos++;
                    } else {
                        Aparencia::imprimirCentralizado(Aparencia::cor(Cor::AMARELO) + "Muito lento! (" + std::to_string(tempoGastoMs) + "ms / " + std::to_string(niveis[i].tempoLimiteMs) + "ms)" + Aparencia::cor(Cor::RESET));
                    }
                } else {
                    Aparencia::imprimirCentralizado(Aparencia::cor(Cor::VERMELHO) + "Errou a sequencia! (Voce digitou: " + entrada + ")" + Aparencia::cor(Cor::RESET));
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
            }

            std::cout << "\n";
            Aparencia::imprimirCentralizado("Resultado do nivel: " + std::to_string(acertos) + "/5 acertos.");
            ControleDeInput::aguardarEnter();
        }
    }

    void rodarTutorialMovimento(const std::string& infoBox) {
        struct NivelTutorialMovimento {
            std::string nomeInimigo;
            int dificuldade;
        };

        std::vector<NivelTutorialMovimento> niveisMov = {
            {"(Nivel 1)", 3},
            {"(Nivel 2)", 6},
            {"(Nivel 3)", 10},
            {"(Nivel Extra)", 14}
        };

        Aparencia::limparTela();
        TelaMenu::exibirPainelLogoJogo("TUTORIAL DE PARRY MOVIMENTO", false);
        if (infoBox.empty()) std::cout << "\n";
        else exibirInfoBox(infoBox);

        std::vector<std::string> explicacao = {
            "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90 COMO FUNCIONA O PARRY MOVIMENTO \xe2\x95\x90\xe2\x95\x90\xe2\x95\x90",
            "Uma barra horizontal com uma zona verde central surgira na tela.",
            "Um cursor '>' percorrera a barra da esquerda para a direita.",
            "Pressione [ESPACO] no momento exato em que o cursor estiver na zona verde!",
            "Se acertar bem no centro, sera um Parry Perfeito (anula o dano!)."
        };

        Aparencia::imprimirBlocoCentralizado(explicacao);
        std::cout << "\n";
        ControleDeInput::aguardarEnter();

        for (size_t i = 0; i < niveisMov.size(); ++i) {
            Aparencia::limparTela();
            TelaMenu::exibirPainelLogoJogo("TUTORIAL DE PARRY MOVIMENTO - " + niveisMov[i].nomeInimigo);

            if (infoBox.empty()) std::cout << "\n";
            else exibirInfoBox(infoBox);

            Aparencia::imprimirCentralizado("Inimigo: " + niveisMov[i].nomeInimigo);
            Aparencia::imprimirCentralizado("Pressione [ESPACO] no momento exato na area verde!");
            ControleDeInput::aguardarEnter();

            int acertos = 0;
            for (int teste = 1; teste <= 5; ++teste) {
                std::cout << "\n";
                Aparencia::imprimirCentralizado("[ Teste " + std::to_string(teste) + "/5 ] Prepare-se...");
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                int dmgRed = 0;
                bool sucesso = Parry::executarMinigameMovimento(niveisMov[i].dificuldade, 100, dmgRed);

                std::cout << "\n";
                if (sucesso) {
                    if (dmgRed == 100) {
                        Aparencia::imprimirCentralizado(Aparencia::cor(Cor::VERDE) + "Parry Perfeito!" + Aparencia::cor(Cor::RESET));
                    } else {
                        Aparencia::imprimirCentralizado(Aparencia::cor(Cor::AMARELO) + "Parry Efetivo!" + Aparencia::cor(Cor::RESET));
                    }
                    acertos++;
                } else {
                    Aparencia::imprimirCentralizado(Aparencia::cor(Cor::VERMELHO) + "Parry Falhou!" + Aparencia::cor(Cor::RESET));
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }

            std::cout << "\n";
            Aparencia::imprimirCentralizado("Resultado do nivel: " + std::to_string(acertos) + "/5 acertos.");
            ControleDeInput::aguardarEnter();
        }
    }
}

void TelaTutorial::exibirTutorialDeParry(const std::string& infoBox) {
    std::cout << "\033[?25l";

    while (true) {
        Aparencia::limparTela();
        TelaMenu::exibirPainelLogoJogo("TUTORIAL DE PARRY", false);
        if (infoBox.empty()) std::cout << "\n";
        else exibirInfoBox(infoBox);

        std::vector<std::string> explicacao = {
            "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90 COMO FUNCIONA O PARRY \xe2\x95\x90\xe2\x95\x90\xe2\x95\x90",
            "Quando um inimigo atacar, voce deve estar pronto para reagir.",
            "Existem dois modos de Parry no jogo:",
            "  1. MOVIMENTO (Minigame de Barra de Tempo/Espaco)",
            "  2. DIGITACAO (Minigame de Sequencia Numerica)",
            "",
            "Escolha qual tutorial deseja realizar:"
        };

        Aparencia::imprimirBlocoCentralizado(explicacao);
        std::cout << "\n";

        std::vector<std::string> opcoesTutorial = {
            "TUTORIAL DE PARRY MOVIMENTO (Barra/Espaco)",
            "TUTORIAL DE PARRY DIGITACAO (Numeros/Enter)",
            "REALIZAR AMBOS OS TUTORIAIS",
            "PULAR TUTORIAL"
        };

        int escolha = ControleDeInput::lerSelecaoMenuComSetas(opcoesTutorial, true);

        if (escolha == 0) {
            rodarTutorialMovimento(infoBox);
            break;
        } else if (escolha == 1) {
            rodarTutorialDigitacao(infoBox);
            break;
        } else if (escolha == 2) {
            rodarTutorialMovimento(infoBox);
            rodarTutorialDigitacao(infoBox);
            break;
        } else {
            return;
        }
    }

    std::cout << "\033[?25l";
    Aparencia::limparTela();
    TelaMenu::exibirPainelLogoJogo("TUTORIAL CONCLUIDO");
    if (infoBox.empty()) std::cout << "\n";
    else exibirInfoBox(infoBox);

    Aparencia::imprimirCentralizado("Voce concluiu as etapas selecionadas do tutorial de Parry!");

    ControleDeInput::aguardarEnter();
}
