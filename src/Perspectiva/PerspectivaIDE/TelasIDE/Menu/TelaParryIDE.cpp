#include "TelaParryIDE.h"
#include "TelaMenuIDE.h"
#include <iostream>
#include <vector>
#include "../../../../Core/Utilidades/Aparencia.h"
#include "../../../../Core/Utilidades/ControleDeInput.h"
#include "../../../../Sistemas/Combate/Parry.h"
#include <chrono>
#include <thread>

namespace {
    void rodarTutorialMovimentoIDE() {
        struct Nivel { std::string nome; int dificuldade; };
        std::vector<Nivel> niveis = {
            {"NIVEL 1 - Facil", 3},
            {"NIVEL 2 - Medio", 6},
            {"NIVEL 3 - Dificil", 10},
            {"NIVEL EXTRA - Desafio", 14}
        };

        for (size_t i = 0; i < niveis.size(); ++i) {
            Aparencia::limparTela();
            std::vector<std::string> intro = {
                "\033[38;2;255;215;0m" + niveis[i].nome + "\033[0m",
                "",
                "Pressione ESPACO quando o marcador estiver na zona verde!",
                "",
                "Pressione ENTER para comecar..."
            };
            int espacos = TelaMenuIDE::calcularEspacoY(intro.size());
            for (int e = 0; e < espacos; e++) std::cout << "\n";
            TelaMenuIDE::imprimirBlocoCentralizadoIDE(intro);
            
            while(true) {
                unsigned char tecla = ControleDeInput::lerTecla();
                if (tecla == '\r' || tecla == '\n') break;
            }

            int acertos = 0;
            for (int teste = 1; teste <= 5; ++teste) {
                std::string msgResultado = "";
                std::string corResultado = "";

                Parry::onUpdateScreen = [&]() {
                    Aparencia::limparTela();
                    std::vector<std::string> tela = {
                        "\033[38;2;255;215;0mPARRY DE MOVIMENTO - DEMO\033[0m",
                        "\033[38;2;180;180;255mTeste " + std::to_string(teste) + "/5 - " + niveis[i].nome + "\033[0m",
                        "",
                        ""
                    };

                    if (!Parry::minigameMessage.empty()) {
                        tela.push_back(Parry::minigameMessage);
                    }
                    if (!Parry::minigameBar.empty()) {
                        tela.push_back(Parry::minigameBar);
                    }
                    tela.push_back("");
                    if (!msgResultado.empty()) {
                        tela.push_back(corResultado + msgResultado + "\033[0m");
                    }

                    int espY = TelaMenuIDE::calcularEspacoY(tela.size());
                    for (int e = 0; e < espY; e++) std::cout << "\n";
                    TelaMenuIDE::imprimirBlocoCentralizadoIDE(tela);
                };

                int dmgRed = 0;
                bool sucesso = Parry::executarMinigameMovimento(niveis[i].dificuldade, 100, dmgRed);

                if (sucesso) {
                    if (dmgRed == 100) {
                        msgResultado = "Parry Perfeito! (dano anulado)";
                        corResultado = "\033[38;2;100;255;100m";
                    } else {
                        msgResultado = "Parry Efetivo! (dano reduzido)";
                        corResultado = "\033[38;2;255;255;100m";
                    }
                    acertos++;
                } else {
                    msgResultado = "Parry Falhou!";
                    corResultado = "\033[38;2;255;100;100m";
                }

                Parry::minigameMessage = "";
                Parry::minigameBar = "";
                
                auto fimTeste = std::chrono::steady_clock::now();
                while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - fimTeste).count() < 1200) {
                    Parry::onUpdateScreen();
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
            }
            Parry::onUpdateScreen = nullptr;
        }
    }

    void rodarTutorialDigitacaoIDE() {
        struct Nivel { std::string nome; int digitos; int tempoMs; };
        std::vector<Nivel> niveis = {
            {"NIVEL 1 - Facil", 3, 3000},
            {"NIVEL 2 - Medio", 4, 2500},
            {"NIVEL 3 - Dificil", 5, 2000},
            {"NIVEL EXTRA - Desafio", 6, 1500}
        };

        for (size_t i = 0; i < niveis.size(); ++i) {
            Aparencia::limparTela();
            std::vector<std::string> intro = {
                "\033[38;2;255;215;0m" + niveis[i].nome + "\033[0m",
                "",
                "Digite a sequencia de numeros que aparecer!",
                std::to_string(niveis[i].digitos) + " digitos | " + std::to_string(niveis[i].tempoMs / 1000) + "s limite",
                "",
                "Pressione ENTER para comecar..."
            };
            int espacos = TelaMenuIDE::calcularEspacoY(intro.size());
            for (int e = 0; e < espacos; e++) std::cout << "\n";
            TelaMenuIDE::imprimirBlocoCentralizadoIDE(intro);
            
            while(true) {
                unsigned char tecla = ControleDeInput::lerTecla();
                if (tecla == '\r' || tecla == '\n') break;
            }

            int acertos = 0;
            for (int teste = 1; teste <= 5; ++teste) {
                std::string msgResultado = "";
                std::string corResultado = "";

                Parry::onUpdateScreen = [&]() {
                    Aparencia::limparTela();
                    std::vector<std::string> tela = {
                        "\033[38;2;255;215;0mPARRY DE DIGITACAO - DEMO\033[0m",
                        "\033[38;2;180;180;255mTeste " + std::to_string(teste) + "/5 - " + niveis[i].nome + "\033[0m",
                        "",
                        ""
                    };

                    if (!Parry::minigameMessage.empty()) {
                        tela.push_back(Parry::minigameMessage);
                    }
                    if (!Parry::minigameBar.empty()) {
                        tela.push_back(Parry::minigameBar);
                    }
                    tela.push_back("");
                    if (!msgResultado.empty()) {
                        tela.push_back(corResultado + msgResultado + "\033[0m");
                    }

                    int espY = TelaMenuIDE::calcularEspacoY(tela.size());
                    for (int e = 0; e < espY; e++) std::cout << "\n";
                    TelaMenuIDE::imprimirBlocoCentralizadoIDE(tela);
                };

                int dmgRed = 0;
                bool sucesso = Parry::executarMinigameDigitacao(niveis[i].digitos * 2, 100, dmgRed);

                if (sucesso) {
                    if (dmgRed == 100) {
                        msgResultado = "Parry Perfeito! (dano anulado)";
                        corResultado = "\033[38;2;100;255;100m";
                    } else {
                        msgResultado = "Parry Efetivo! (dano reduzido)";
                        corResultado = "\033[38;2;255;255;100m";
                    }
                    acertos++;
                } else {
                    msgResultado = "Parry Falhou!";
                    corResultado = "\033[38;2;255;100;100m";
                }

                Parry::minigameMessage = "";
                Parry::minigameBar = "";
                
                auto fimTeste = std::chrono::steady_clock::now();
                while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - fimTeste).count() < 1200) {
                    Parry::onUpdateScreen();
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
            }
            Parry::onUpdateScreen = nullptr;
        }
    }
}

TelaParry::Resultado TelaParryIDE::exibir(const std::string& nomeJogador, const std::string& nomeRaca, const std::string& nomeClasse) {
    (void)nomeJogador;
    (void)nomeRaca;
    (void)nomeClasse;

    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string colorType = "\033[38;2;78;201;176m"; // Cyan
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorComment = "\033[38;2;87;166;74m"; // Green
    std::string colorHighlight = "\033[48;2;38;79;120m\033[38;2;255;255;255m"; // Azul escuro bg
    std::string reset = "\033[0m";

    std::vector<std::string> opcoes = {
        "PARRY DESLIGADO",
        "PARRY MOVIMENTO",
        "PARRY DIGITACAO",
        "VOLTAR"
    };
    
    int selecaoAtual = 0;
    ControleDeInput::limparBuffer();

    while (true) {
        std::cout << "\033[?25l";
        Aparencia::limparTela();
        
        std::vector<std::string> blocoCentral;
        blocoCentral.push_back(colorComment + "// SISTEMA DE COMBATE" + reset);
        blocoCentral.push_back(colorComment + "// O sistema de defesas depende do Parry, que eh um minigame de reacao." + reset);
        blocoCentral.push_back(colorComment + "// Escolha o modo de Parry" + reset);
        blocoCentral.push_back("");
        
        blocoCentral.push_back(colorType + "void " + colorKeyword + "configurarParry" + colorPunct + "() {");
        
        for (int i = 0; i < (int)opcoes.size(); ++i) {
            std::string linha = "    ";
            std::string nomeOpcao = opcoes[i];
            
            if (i == selecaoAtual) {
                linha += colorHighlight + nomeOpcao + reset;
            } else {
                linha += colorPunct + nomeOpcao + reset;
            }
            
            blocoCentral.push_back(linha);
        }
        
        blocoCentral.push_back(colorPunct + "}");

        int espacosY = TelaMenuIDE::calcularEspacoY(blocoCentral.size());
        for (int i = 0; i < espacosY; ++i) std::cout << "\n";
        
        TelaMenuIDE::imprimirBlocoCentralizadoIDE(blocoCentral);

        unsigned char tecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
        if (tecla == 224 || tecla == 0 || tecla == '\033') {
            unsigned char proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (proxTecla == '[') proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (proxTecla == 72 || proxTecla == 'A') tecla = 'w';
            else if (proxTecla == 80 || proxTecla == 'B') tecla = 's';
        }

        if (tecla == 'w' || tecla == 'W') {
            selecaoAtual = (selecaoAtual - 1 + (int)opcoes.size()) % (int)opcoes.size();
        } else if (tecla == 's' || tecla == 'S') {
            selecaoAtual = (selecaoAtual + 1) % (int)opcoes.size();
        } else if (tecla == '\r' || tecla == '\n') {
            break;
        }
    }

    if (selecaoAtual == 3) {
        TelaParry::Resultado r;
        r.voltou = true;
        return r;
    }

    TelaParry::Resultado r;
    if (selecaoAtual == 1) {
        r.modo = TelaParry::Resultado::Modo::Movimento;
        rodarTutorialMovimentoIDE();
    } else if (selecaoAtual == 2) {
        r.modo = TelaParry::Resultado::Modo::Digitacao;
        rodarTutorialDigitacaoIDE();
    } else {
        r.modo = TelaParry::Resultado::Modo::Desligado;
    }
    
    return r;
}