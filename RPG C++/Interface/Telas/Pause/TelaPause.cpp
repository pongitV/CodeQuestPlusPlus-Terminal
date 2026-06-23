#include "TelaPause.h"
#include "../Menu/TelaMenu.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Entidades/Personagem.h"
#include "../../../Sistemas/Progresso/Salvamento.h"
#include "../../../Sistemas/Raycaster/Raycaster.h"
#include <iostream>
#include <vector>

void TelaPause::exibir(Personagem* jogador) {
    static int corFundoAtualIndex = 0;
    bool continuar = true;
    while (continuar && !jogador->obterVoltarProMenu()) {
        std::cout << "\033[?25l"; // Esconde o cursor
        Aparencia::limparTela();
        
        TelaMenu::exibirPainelLogoJogo("JOGO EM PAUSE", false);
        std::cout << "\n";
        
        std::vector<std::string> opcoes = {
            "Voltar ao jogo",
            "Configuracoes",
            "Salvar e sair"
        };
        
        int escolha = ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);
        
        if (escolha == 0) { // Voltar ao jogo
            continuar = false;
        } else if (escolha == 1) { // Configuracoes
            bool configAberta = true;
            while (configAberta) {
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
                
                int confEscolha = ControleDeInput::lerSelecaoMenuComSetas(opcoesConfig, true);
                if (confEscolha == 0) {
                    int dificuldadeAtual = static_cast<int>(jogador->obterDificuldade());
                    dificuldadeAtual++;
                    if (dificuldadeAtual > 3) dificuldadeAtual = 1;
                    jogador->definirDificuldade(static_cast<DificuldadeJogo>(dificuldadeAtual));
                } else if (confEscolha == 1) {
                    jogador->definirParryAtivado(!jogador->obterParryAtivado());
                } else if (confEscolha == 2) {
                    jogador->definirParryModerno(!jogador->obterParryModerno());
                } else if (confEscolha == 3) {
                    bool aparenciaAberta = true;
                    while (aparenciaAberta) {
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
                        
                        int apEscolha = ControleDeInput::lerSelecaoMenuComSetas(opcoesAparencia, true);
                        if (apEscolha == 0) {
                            Cor cores[] = {Cor::VERDE, Cor::AZUL, Cor::AMARELO, Cor::BRANCO, Cor::MAGENTA, Cor::CIANO, Cor::VERMELHO};
                            int numCores = 7;
                            int idxAtual = 0;
                            for (int i = 0; i < numCores; ++i) {
                                if (jogador->obterCorJogador() == cores[i]) {
                                    idxAtual = i;
                                    break;
                                }
                            }
                            idxAtual = (idxAtual + 1) % numCores;
                            jogador->definirCorJogador(cores[idxAtual]);
                        } else if (apEscolha == 1) {
                            std::cout << "\n";
                            std::string promptMsg = "Digite o novo icone (1 caractere): ";
                            std::cout << Aparencia::espacosParaCentralizar(promptMsg.length()) << promptMsg;
                            std::string novoIcone = ControleDeInput::lerEntradaProtegida();
                            if (!novoIcone.empty() && novoIcone[0] != ' ') {
                                jogador->definirIconeJogador(novoIcone[0]);
                            }
                        } else {
                            aparenciaAberta = false;
                        }
                    }
                } else if (confEscolha == 4) {
                    bool fundoAberto = true;
                    while (fundoAberto) {
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
                        
                        int fundoEscolha = ControleDeInput::lerSelecaoMenuComSetas(opcoesFundo, true);
                        if (fundoEscolha >= 0 && fundoEscolha <= 5) {
                            corFundoAtualIndex = fundoEscolha;
                            std::string hexColor;
                            switch (fundoEscolha) {
                                case 0: hexColor = "#0C0C0C"; break; case 1: hexColor = "#1A1A1A"; break;
                                case 2: hexColor = "#000022"; break; case 3: hexColor = "#220000"; break;
                                case 4: hexColor = "#002200"; break; case 5: hexColor = "#220022"; break;
                            }
                            std::cout << "\033]11;" << hexColor << "\007" << std::flush;
                        } else {
                            fundoAberto = false;
                        }
                    }
                } else if (confEscolha == 5) {
                    bool sensibilidadeAberta = true;
                    while (sensibilidadeAberta) {
                        Aparencia::limparTela();
                        TelaMenu::exibirPainelLogoJogo("SENSIBILIDADE", false);
                        std::cout << "\n";
                        
                        // Formatar as sensibilidades para exibicao como porcentagem da base (0.002f para X, 0.08f para Y)
                        int percX = (int)((Raycaster::sensibilidadeX / 0.002f) * 100);
                        int percY = (int)((Raycaster::sensibilidadeY / 0.08f) * 100);
                        
                        std::vector<std::string> opcoesSens = {
                            "Horizontal (X): " + std::to_string(percX) + "%  [ ENTER p/ Digitar Novo Valor ]",
                            "Vertical (Y): " + std::to_string(percY) + "%  [ ENTER p/ Digitar Novo Valor ]",
                            "Voltar"
                        };
                        
                        int sensEscolha = ControleDeInput::lerSelecaoMenuComSetas(opcoesSens, true);
                        if (sensEscolha == 0) {
                            std::cout << "\n";
                            std::string promptMsg = "Digite o novo valor em porcentagem (ex: 50, 100, 150): ";
                            std::cout << Aparencia::espacosParaCentralizar(promptMsg.length()) << promptMsg;
                            std::string entrada = ControleDeInput::lerEntradaProtegida();
                            try {
                                int novoValor = std::stoi(entrada);
                                if (novoValor > 0) Raycaster::sensibilidadeX = (novoValor / 100.0f) * 0.002f;
                            } catch (...) {} // ignora se digitar letras
                        } else if (sensEscolha == 1) {
                            std::cout << "\n";
                            std::string promptMsg = "Digite o novo valor em porcentagem (ex: 50, 100, 150): ";
                            std::cout << Aparencia::espacosParaCentralizar(promptMsg.length()) << promptMsg;
                            std::string entrada = ControleDeInput::lerEntradaProtegida();
                            try {
                                int novoValor = std::stoi(entrada);
                                if (novoValor > 0) Raycaster::sensibilidadeY = (novoValor / 100.0f) * 0.08f;
                            } catch (...) {}
                        } else {
                            sensibilidadeAberta = false;
                        }
                    }
                } else {
                    configAberta = false;
                }
            }
        } else if (escolha == 2) { // Salvar e sair
            std::vector<std::string> opcoesSimNao = { "NAO", "SIM" };
            std::cout << "\n";
            Aparencia::imprimirCentralizado("[AVISO]: Deseja salvar o jogo e voltar para o menu principal?");
            std::cout << "\n";
            if (ControleDeInput::lerSelecaoMenuComSetas(opcoesSimNao, true) == 1) {
                Aparencia::imprimirCentralizado("[AVISO]: Tem certeza?");
                std::cout << "\n";
                if (ControleDeInput::lerSelecaoMenuComSetas(opcoesSimNao, true) == 1) {
                    Salvamento::salvarJogo(jogador);
                    std::cout << "\n";
                    Aparencia::imprimirCentralizado(Aparencia::cor(Cor::VERDE) + "[SISTEMA]: Jogo salvo no arquivo save_" + jogador->obterNome() + ".txt com sucesso!" + Aparencia::cor(Cor::RESET));
                    ControleDeInput::aguardarEnter();
                    jogador->definirVoltarProMenu(true);
                    continuar = false;
                }
            }
        }
    }
}






