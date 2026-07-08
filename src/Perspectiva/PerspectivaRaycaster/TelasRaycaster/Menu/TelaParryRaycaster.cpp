#include "TelaParryRaycaster.h"
#include "../Utils/MenuRaycasterLayout.h"
#include "../Utils/MenuRaycasterUtils.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>

#include "../../../TelasBase/Menu/TelaBaseMenu.h"
#include "../../../../Core/Utilidades/FuncoesDialogo.h"
#include "../../../../Core/Utilidades/ControleDeInput.h"
#include "../../../../Core/Utilidades/GeradorAleatorio.h"
#include "../../../../Sistemas/Combate/Parry.h"

namespace {

    int exibirTelaComTexto(int larguraConsole, const std::string& titulo, const std::vector<std::string>& linhas) {
        while (!ControleDeInput::teclaPressionada()) {
            MenuRaycasterUtils::incrementarCicloDia();
            std::ostringstream buf;
            MenuRaycasterUtils::exibirFundo3D(buf);

            int visTit = Aparencia::obterComprimentoVisual(titulo);
            int espTit = std::max(0, (larguraConsole - visTit) / 2);

            int maxW = visTit;
            for (size_t i = 0; i < linhas.size(); ++i) {
                maxW = std::max(maxW, Aparencia::obterComprimentoVisual(linhas[i]));
            }
            int boxW = maxW + 10;
            if (boxW > larguraConsole - 4) boxW = larguraConsole - 4;
            int boxX = TelaBaseMenu::calcularOffsetCentral(boxW, larguraConsole);
            int numLinhas = (int)linhas.size() + 4;
            TelaBaseMenu::desenharCaixaPreta(buf, 2, boxX, boxW, numLinhas);

            MenuRaycasterUtils::sobreporTextoAbsoluto(buf, "\033[38;2;255;215;0m" + titulo + "\033[0m", 3, espTit);

            for (size_t i = 0; i < linhas.size(); ++i) {
                int visLen = Aparencia::obterComprimentoVisual(linhas[i]);
                int esp = TelaBaseMenu::calcularOffsetCentral(visLen, larguraConsole);
                MenuRaycasterUtils::sobreporTextoAbsoluto(buf, linhas[i], 5 + (int)i, esp);
            }

            MenuRaycasterUtils::flushFrameParaConsole(buf.str());
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
        int tecla = ControleDeInput::lerTecla();
        ControleDeInput::limparBuffer();
        return tecla;
    }

    void exibirContagemRegressiva(int larguraConsole, int inicio) {
        for (int i = inicio; i > 0; --i) {
            for (int f = 0; f < 15; ++f) {
                MenuRaycasterUtils::incrementarCicloDia();
                std::ostringstream buf;
                MenuRaycasterUtils::exibirFundo3D(buf);

                std::string num = std::to_string(i);
                int esp = TelaBaseMenu::calcularOffsetCentral(num, larguraConsole);
                int boxX = TelaBaseMenu::calcularOffsetCentral(6, larguraConsole);
                TelaBaseMenu::desenharCaixaPreta(buf, 9, boxX, 6, 3);
                MenuRaycasterUtils::sobreporTextoAbsoluto(buf, "\033[38;2;255;255;100m" + num + "\033[0m", 10, esp);

                MenuRaycasterUtils::flushFrameParaConsole(buf.str());
                std::this_thread::sleep_for(std::chrono::milliseconds(65));
            }
        }
        for (int f = 0; f < 10; ++f) {
            MenuRaycasterUtils::incrementarCicloDia();
            std::ostringstream buf;
            MenuRaycasterUtils::exibirFundo3D(buf);

            std::string go = "VAI!";
            int esp = TelaBaseMenu::calcularOffsetCentral(go, larguraConsole);
            int boxX = TelaBaseMenu::calcularOffsetCentral(8, larguraConsole);
            TelaBaseMenu::desenharCaixaPreta(buf, 9, boxX, 8, 3);
            MenuRaycasterUtils::sobreporTextoAbsoluto(buf, "\033[38;2;100;255;100m" + go + "\033[0m", 10, esp);

            MenuRaycasterUtils::flushFrameParaConsole(buf.str());
            std::this_thread::sleep_for(std::chrono::milliseconds(80));
        }
    }

    void rodarTutorialMovimento(int larguraConsole) {
        struct Nivel { std::string nome; int dificuldade; };
        std::vector<Nivel> niveis = {
            {"NIVEL 1 - Facil", 3},
            {"NIVEL 2 - Medio", 6},
            {"NIVEL 3 - Dificil", 10},
            {"NIVEL EXTRA - Desafio", 14}
        };

        for (size_t i = 0; i < niveis.size(); ++i) {
            std::vector<std::string> intro = {
                "\033[38;2;255;215;0m" + niveis[i].nome + "\033[0m",
                "",
                "\033[38;2;180;180;255mPressione ESPACO quando o marcador estiver na zona verde!\033[0m",
                "",
                "\033[38;2;120;120;120mPressione ENTER para comecar...\033[0m"
            };
            exibirTelaComTexto(larguraConsole, "TUTORIAL DE PARRY MOVIMENTO", intro);
            exibirContagemRegressiva(larguraConsole, 3);

            int acertos = 0;
            for (int teste = 1; teste <= 5; ++teste) {
                std::string msgResultado = "";
                std::string corResultado = "";

                Parry::onUpdateScreen = [&]() {
                    MenuRaycasterUtils::incrementarCicloDia();
                    std::ostringstream buf;
                    MenuRaycasterUtils::exibirFundo3D(buf);

                    std::string titulo = "PARRY DE MOVIMENTO - DEMO";
                    std::string subtitulo = "Teste " + std::to_string(teste) + "/5 - " + niveis[i].nome;

                    int espTit = TelaBaseMenu::calcularOffsetCentral(titulo, larguraConsole);
                    int espSub = TelaBaseMenu::calcularOffsetCentral(subtitulo, larguraConsole);

                    int boxW = std::max((int)titulo.length(), (int)subtitulo.length()) + 50;
                    if (boxW > larguraConsole - 4) boxW = larguraConsole - 4;
                    int boxX = TelaBaseMenu::calcularOffsetCentral(boxW, larguraConsole);
                    TelaBaseMenu::desenharCaixaPreta(buf, 1, boxX, boxW, 21);

                    MenuRaycasterUtils::sobreporTextoAbsoluto(buf, "\033[38;2;255;215;0m" + titulo + "\033[0m", 2, espTit);
                    MenuRaycasterUtils::sobreporTextoAbsoluto(buf, "\033[38;2;180;180;255m" + subtitulo + "\033[0m", 3, espSub);

                    if (!Parry::minigameMessage.empty()) {
                        int compMsg = Aparencia::obterComprimentoVisual(Parry::minigameMessage);
                        int espMsg = TelaBaseMenu::calcularOffsetCentral(compMsg, larguraConsole);
                        MenuRaycasterUtils::sobreporTextoAbsoluto(buf, Parry::minigameMessage, 15, espMsg);
                    }
                    if (!Parry::minigameBar.empty()) {
                        int compBar = Aparencia::obterComprimentoVisual(Parry::minigameBar);
                        int espBar = TelaBaseMenu::calcularOffsetCentral(compBar, larguraConsole);
                        MenuRaycasterUtils::sobreporTextoAbsoluto(buf, Parry::minigameBar, 17, espBar);
                    }

                    if (!msgResultado.empty()) {
                        int espRes = TelaBaseMenu::calcularOffsetCentral(msgResultado, larguraConsole);
                        MenuRaycasterUtils::sobreporTextoAbsoluto(buf, corResultado + msgResultado + "\033[0m", 20, espRes);
                    }

                    MenuRaycasterUtils::flushFrameParaConsole(buf.str());
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

            std::vector<std::string> resultado = {
                "\033[38;2;255;255;255mResultado: " + std::to_string(acertos) + "/5 acertos.\033[0m",
                "",
                "\033[38;2;120;120;120mPressione ENTER para continuar...\033[0m"
            };
            exibirTelaComTexto(larguraConsole, "TUTORIAL DE PARRY MOVIMENTO", resultado);
        }

        std::vector<std::string> concluido = {
            "\033[38;2;100;255;100mTutorial de Parry Movimento concluido!\033[0m",
            "",
            "\033[38;2;120;120;120mPressione ENTER para continuar...\033[0m"
        };
        exibirTelaComTexto(larguraConsole, "PARABENS!", concluido);
    }

    void rodarTutorialDigitacao(int larguraConsole) {
        struct Nivel { std::string nome; int digitos; int tempoMs; };
        std::vector<Nivel> niveis = {
            {"NIVEL 1 - Facil", 3, 3000},
            {"NIVEL 2 - Medio", 4, 2500},
            {"NIVEL 3 - Dificil", 5, 2000},
            {"NIVEL EXTRA - Desafio", 6, 1500}
        };

        for (size_t i = 0; i < niveis.size(); ++i) {
            std::vector<std::string> intro = {
                "\033[38;2;255;215;0m" + niveis[i].nome + "\033[0m",
                "",
                "\033[38;2;180;180;255mDigite a sequencia de numeros que aparecer!\033[0m",
                "\033[38;2;180;180;255m" + std::to_string(niveis[i].digitos) + " digitos | " + std::to_string(niveis[i].tempoMs / 1000) + "s limite\033[0m",
                "",
                "\033[38;2;120;120;120mPressione ENTER para comecar...\033[0m"
            };
            exibirTelaComTexto(larguraConsole, "TUTORIAL DE PARRY DIGITACAO", intro);
            exibirContagemRegressiva(larguraConsole, 3);

            int acertos = 0;
            for (int teste = 1; teste <= 5; ++teste) {
                std::string msgResultado = "";
                std::string corResultado = "";

                Parry::onUpdateScreen = [&]() {
                    MenuRaycasterUtils::incrementarCicloDia();
                    std::ostringstream buf;
                    MenuRaycasterUtils::exibirFundo3D(buf);

                    std::string titulo = "PARRY DE DIGITACAO - DEMO";
                    std::string subtitulo = "Teste " + std::to_string(teste) + "/5 - " + niveis[i].nome;

                    int espTit = TelaBaseMenu::calcularOffsetCentral(titulo, larguraConsole);
                    int espSub = TelaBaseMenu::calcularOffsetCentral(subtitulo, larguraConsole);

                    int boxW = std::max((int)titulo.length(), (int)subtitulo.length()) + 50;
                    if (boxW > larguraConsole - 4) boxW = larguraConsole - 4;
                    int boxX = TelaBaseMenu::calcularOffsetCentral(boxW, larguraConsole);
                    TelaBaseMenu::desenharCaixaPreta(buf, 1, boxX, boxW, 21);

                    MenuRaycasterUtils::sobreporTextoAbsoluto(buf, "\033[38;2;255;215;0m" + titulo + "\033[0m", 2, espTit);
                    MenuRaycasterUtils::sobreporTextoAbsoluto(buf, "\033[38;2;180;180;255m" + subtitulo + "\033[0m", 3, espSub);

                    if (!Parry::minigameMessage.empty()) {
                        int compMsg = Aparencia::obterComprimentoVisual(Parry::minigameMessage);
                        int espMsg = TelaBaseMenu::calcularOffsetCentral(compMsg, larguraConsole);
                        MenuRaycasterUtils::sobreporTextoAbsoluto(buf, Parry::minigameMessage, 15, espMsg);
                    }
                    if (!Parry::minigameBar.empty()) {
                        int compBar = Aparencia::obterComprimentoVisual(Parry::minigameBar);
                        int espBar = TelaBaseMenu::calcularOffsetCentral(compBar, larguraConsole);
                        MenuRaycasterUtils::sobreporTextoAbsoluto(buf, Parry::minigameBar, 17, espBar);
                    }

                    if (!msgResultado.empty()) {
                        int espRes = TelaBaseMenu::calcularOffsetCentral(msgResultado, larguraConsole);
                        MenuRaycasterUtils::sobreporTextoAbsoluto(buf, corResultado + msgResultado + "\033[0m", 20, espRes);
                    }

                    MenuRaycasterUtils::flushFrameParaConsole(buf.str());
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

            std::vector<std::string> resultado = {
                "\033[38;2;255;255;255mResultado: " + std::to_string(acertos) + "/5 acertos.\033[0m",
                "",
                "\033[38;2;120;120;120mPressione ENTER para continuar...\033[0m"
            };
            exibirTelaComTexto(larguraConsole, "TUTORIAL DE PARRY DIGITACAO", resultado);
        }

        std::vector<std::string> concluido = {
            "\033[38;2;100;255;100mTutorial de Parry Digitacao concluido!\033[0m",
            "",
            "\033[38;2;120;120;120mPressione ENTER para continuar...\033[0m"
        };
        exibirTelaComTexto(larguraConsole, "PARABENS!", concluido);
        ControleDeInput::lerTecla();
        ControleDeInput::limparBuffer();
    }

}

TelaParry::Resultado TelaParryRaycaster::exibir(const std::string& nomeJogador, const std::string& nomeRaca, const std::string& nomeClasse) {
    int larguraConsole = Aparencia::obterLarguraTerminal();
    std::vector<std::string> opcoes = {
        "PARRY DESLIGADO",
        "PARRY MOVIMENTO (Barra deslizante)",
        "PARRY DIGITACAO (Digitar por tempo)",
        "VOLTAR"
    };

    int selecaoAtual = 0;

    std::cout << "\033[?25l";
    ControleDeInput::limparBuffer();
    MenuRaycasterUtils::cachearFundo3D("Vila", nullptr);

    while (true) {
        MenuRaycasterUtils::incrementarCicloDia();

        std::ostringstream buffer;
        MenuRaycasterUtils::exibirFundo3D(buffer);

        std::string infoBox = nomeJogador + " | " + nomeRaca + " | " + nomeClasse;
        int boxW = (int)infoBox.length() + 4;
        int boxX = TelaBaseMenu::calcularOffsetCentral(boxW, larguraConsole);
        std::string borda;
        for (int i = 0; i < (int)infoBox.length() + 2; i++) borda += "\u2500";
        MenuRaycasterUtils::sobreporTexto3D(buffer, "\033[38;2;255;255;255m\u250C" + borda + "\u2510\033[0m", 2, boxX, larguraConsole);
        MenuRaycasterUtils::sobreporTexto3D(buffer, "\033[38;2;255;255;255m\u2502 " + infoBox + " \u2502\033[0m", 3, boxX, larguraConsole);
        MenuRaycasterUtils::sobreporTexto3D(buffer, "\033[38;2;255;255;255m\u2514" + borda + "\u2518\033[0m", 4, boxX, larguraConsole);

        int yBase = 7;
        int espOpcao = TelaBaseMenu::calcularOffsetCentral(50, larguraConsole);
        
        TelaBaseMenu::desenharCaixaPreta(buffer, yBase - 1, espOpcao - 2, 54, (int)opcoes.size() + 2);

        for (int i = 0; i < (int)opcoes.size(); ++i) {
            std::string icone = (i == selecaoAtual) ? "> " : "  ";
            std::string cor = (i == selecaoAtual) ? "\033[38;2;0;255;0m" : "\033[38;2;120;120;120m";
            MenuRaycasterUtils::sobreporTextoAbsoluto(buffer, cor + icone + opcoes[i] + "\033[0m", yBase + i, espOpcao);
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
            else if (proxTecla == 27) {
                TelaParry::Resultado r;
                r.voltou = true;
                return r;
            }
        }

        if (tecla == 'w' || tecla == 'W') {
            selecaoAtual = (selecaoAtual - 1 + (int)opcoes.size()) % (int)opcoes.size();
        } else if (tecla == 's' || tecla == 'S') {
            selecaoAtual = (selecaoAtual + 1) % (int)opcoes.size();
        } else if (tecla == '\r' || tecla == '\n') {
            if (selecaoAtual == 3) {
                TelaParry::Resultado r;
                r.voltou = true;
                return r;
            }

            TelaParry::Resultado r;
            if (selecaoAtual == 1) {
                r.modo = TelaParry::Resultado::Modo::Movimento;
                std::vector<std::string> explicacao = {
                    "\033[38;2;180;180;255mUma barra horizontal com uma zona verde central surgira na tela.\033[0m",
                    "\033[38;2;180;180;255mUm cursor percorrera a barra da esquerda para a direita.\033[0m",
                    "\033[38;2;180;180;255mPressione ESPACO no momento exato em que o cursor estiver na zona verde!\033[0m",
                    "",
                    "\033[38;2;120;120;120mPressione ENTER para iniciar o tutorial...\033[0m",
                    "\033[38;2;120;120;120mPressione ESC para pular o tutorial...\033[0m"
                };
                int tecla = exibirTelaComTexto(larguraConsole, "PARRY MOVIMENTO - TUTORIAL", explicacao);
                if (tecla != 27) {
                    rodarTutorialMovimento(larguraConsole);
                }
            } else if (selecaoAtual == 2) {
                r.modo = TelaParry::Resultado::Modo::Digitacao;
                std::vector<std::string> explicacao = {
                    "\033[38;2;180;180;255mUma sequencia de numeros aparecera na tela com um limite de tempo.\033[0m",
                    "\033[38;2;180;180;255mDigite os numeros rapidamente na sequencia correta e pressione ENTER.\033[0m",
                    "\033[38;2;180;180;255mSe for rapido o suficiente, o dano sera reduzido ou anulado!\033[0m",
                    "",
                    "\033[38;2;120;120;120mPressione ENTER para iniciar o tutorial...\033[0m",
                    "\033[38;2;120;120;120mPressione ESC para pular o tutorial...\033[0m"
                };
                int tecla = exibirTelaComTexto(larguraConsole, "PARRY DIGITACAO - TUTORIAL", explicacao);
                if (tecla != 27) {
                    rodarTutorialDigitacao(larguraConsole);
                }
            } else {
                r.modo = TelaParry::Resultado::Modo::Desligado;
            }
            return r;
        }
    }
}
