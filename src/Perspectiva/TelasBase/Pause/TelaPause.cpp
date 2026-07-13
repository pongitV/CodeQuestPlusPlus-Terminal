#include "TelaPause.h"
#include "../RegistroTelas.h"
#include <iostream>
#include "../../../Entidades/Personagem.h"
#include "../../GerenciadorPerspectiva.h"
#include "../../PerspectivaIDE/TelasIDE/Pause/TelaPauseIDE.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"

static int obterEscolhaMenuPause() {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        return GerenciadorPerspectiva::obterPauseUI().renderizarMenuPause();
    }
    return TelaPauseIDE::renderizarMenuPause();
}

static int obterEscolhaConfiguracoes(Personagem* jogador) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        return GerenciadorPerspectiva::obterPauseUI().renderizarMenuConfiguracoes(jogador);
    }
    return TelaPauseIDE::renderizarMenuConfiguracoes(jogador);
}

static int obterEscolhaAparencia(Personagem* jogador) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        return GerenciadorPerspectiva::obterPauseUI().renderizarMenuAparencia(jogador);
    }
    return TelaPauseIDE::renderizarMenuAparencia(jogador);
}

static int obterEscolhaFundo(int corFundoAtualIndex) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        return GerenciadorPerspectiva::obterPauseUI().renderizarMenuFundo(corFundoAtualIndex);
    }
    return TelaPauseIDE::renderizarMenuFundo(corFundoAtualIndex);
}

static int obterEscolhaSensibilidade(int percX, int percY) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        return GerenciadorPerspectiva::obterPauseUI().renderizarMenuSensibilidade(percX, percY);
    }
    return TelaPauseIDE::renderizarMenuSensibilidade(percX, percY);
}

void TelaPause::exibir(Personagem* jogador) {
    int corFundoAtualIndex = 0;
    bool continuar = true;

    while (continuar && !jogador->obterVoltarProMenu()) {
        int escolha = obterEscolhaMenuPause();

        if (escolha == 0) {
            continuar = false;
        } else if (escolha == 1) {
            bool configAberta = true;
            while (configAberta) {
                int confEscolha = obterEscolhaConfiguracoes(jogador);

                if (confEscolha == 0) {
                    int dificuldadeAtual = static_cast<int>(jogador->obterDificuldade());
                    dificuldadeAtual++;
                    if (dificuldadeAtual > 3) dificuldadeAtual = 1;
                    jogador->definirDificuldade(static_cast<DificuldadeJogo>(dificuldadeAtual));
                } else if (confEscolha == 1) {
                    jogador->definirParryAtivado(!jogador->obterParryAtivado());
                } else if (confEscolha == 2) {
                    bool isTerminal = !GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva();
                    if (isTerminal) {
                        std::cout << "\n";
                        Aparencia::imprimirCentralizado(Aparencia::cor(Cor::AMARELO) + "[AVISO]: Na visao de terminal (IDE), apenas o sistema de parry de digitacao esta disponivel." + Aparencia::cor(Cor::RESET));
                        Aparencia::imprimirCentralizado(Aparencia::cor(Cor::CINZA) + "[AVISO]: A alternancia de parry (movimento/digitacao) so esta disponivel fora da visao de terminal." + Aparencia::cor(Cor::RESET));
                        ControleDeInput::aguardarEnter();
                    } else {
                        jogador->definirParryModerno(!jogador->obterParryModerno());
                    }
                } else if (confEscolha == 3) {
                    bool aparenciaAberta = true;
                    while (aparenciaAberta) {
                        int apEscolha = obterEscolhaAparencia(jogador);

                        if (apEscolha == 0) {
                            Cor cores[] = {Cor::VERDE, Cor::AZUL, Cor::AMARELO, Cor::BRANCO, Cor::MAGENTA, Cor::CIANO, Cor::VERMELHO};
                            int numCores = 7;
                            int idxAtual = 0;
                            for (int i = 0; i < numCores; ++i) {
                                if (Aparencia::corJogadorPersonalizada == cores[i]) {
                                    idxAtual = i;
                                    break;
                                }
                            }
                            idxAtual = (idxAtual + 1) % numCores;
                            Aparencia::corJogadorPersonalizada = cores[idxAtual];
                        } else if (apEscolha == 1) {
                            std::cout << "\n";
                            std::string promptMsg = "Digite o novo icone (1 caractere): ";
                            std::cout << Aparencia::espacosParaCentralizar(promptMsg.length()) << promptMsg;
                            std::string novoIcone = ControleDeInput::lerEntradaProtegida();
                            if (!novoIcone.empty() && novoIcone[0] != ' ') {
                                Aparencia::iconeJogadorPersonalizado = novoIcone[0];
                            }
                        } else {
                            aparenciaAberta = false;
                        }
                    }
                } else if (confEscolha == 4) {
                    bool fundoAberto = true;
                    while (fundoAberto) {
                        int fundoEscolha = obterEscolhaFundo(corFundoAtualIndex);

                        if (fundoEscolha >= 0 && fundoEscolha <= 5) {
                            corFundoAtualIndex = fundoEscolha;
                            std::string hexColor;
                            switch (fundoEscolha) {
                                case 0: hexColor = "#0C0C0C"; break; case 1: hexColor = "#1A1A1A"; break;
                                case 2: hexColor = "#000022"; break; case 3: hexColor = "#220000"; break;
                                case 4: hexColor = "#002200"; break; case 5: hexColor = "#220022"; break;
                            }
                            Aparencia::definirCorFundoTerminal(hexColor);
                        } else {
                            fundoAberto = false;
                        }
                    }
                } else if (confEscolha == 5) {
                    bool sensibilidadeAberta = true;
                    while (sensibilidadeAberta) {
                        int percX = (int)((GerenciadorPerspectiva::obterSensibilidadeMouseX() / 0.002f) * 100);
                        int percY = (int)((GerenciadorPerspectiva::obterSensibilidadeMouseY() / 0.08f) * 100);

                        int sensEscolha = obterEscolhaSensibilidade(percX, percY);

                        if (sensEscolha == 0) {
                            std::cout << "\n";
                            std::string promptMsg = "Digite o novo valor em porcentagem (ex: 50, 100, 150): ";
                            std::cout << Aparencia::espacosParaCentralizar(promptMsg.length()) << promptMsg;
                            std::string entrada = ControleDeInput::lerEntradaProtegida();
                            try {
                                int novoValor = std::stoi(entrada);
                                if (novoValor > 0) GerenciadorPerspectiva::definirSensibilidadeMouse((novoValor / 100.0f) * 0.002f, GerenciadorPerspectiva::obterSensibilidadeMouseY());
                            } catch (...) {}
                        } else if (sensEscolha == 1) {
                            std::cout << "\n";
                            std::string promptMsg = "Digite o novo valor em porcentagem (ex: 50, 100, 150): ";
                            std::cout << Aparencia::espacosParaCentralizar(promptMsg.length()) << promptMsg;
                            std::string entrada = ControleDeInput::lerEntradaProtegida();
                            try {
                                int novoValor = std::stoi(entrada);
                                if (novoValor > 0) GerenciadorPerspectiva::definirSensibilidadeMouse(GerenciadorPerspectiva::obterSensibilidadeMouseX(), (novoValor / 100.0f) * 0.08f);
                            } catch (...) {}
                        } else {
                            sensibilidadeAberta = false;
                        }
                    }
                } else {
                    configAberta = false;
                }
            }
        } else if (escolha == 2) {
            if (RegistroTelas::confirmarSaida()) {
                std::exit(0);
            }
        }
    }
}
