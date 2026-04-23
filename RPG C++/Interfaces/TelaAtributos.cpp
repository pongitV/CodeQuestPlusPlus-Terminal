#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

#ifdef _WIN32
    #include <windows.h>
#endif

#include "TelaAtributos.h"
#include "../Sistema/Menu.h"
#include "../Raças/RacaBase.h"
#include "../Classes/ClasseBase.h"

void TelaAtributos::exibir(Personagem* jogadorAtual)
{
    if (jogadorAtual == nullptr) return;
    Menu::limparTelaDoTerminal();
    
#ifdef _WIN32
    SetConsoleOutputCP(65001); 
#endif

    int largura = Menu::obterLarguraDoTerminalEmColunas();
    std::vector<std::string> logoFicha = 
    {
       "███████████  █████   █████████  █████   █████   █████████   ",
       "░░███░░░░░█ ░░███   ███░░░░░███░░███   ░░███   ███░░░░░███  ",
       " ░███   █ ░  ░███  ███     ░░░  ░███    ░███  ░███    ░███  ",
       " ░███████    ░███ ░███          ░███████████  ░███████████  ",
       " ░███░░░█    ░███ ░███          ░███░░░░░███  ░███░░░░░███  ",
       " ░███  ░     ░███ ░░███     ███ ░███    ░███  ░███    ░███  ",
       " █████       █████ ░░█████████  █████   █████ █████   █████ ",
       " ░░░░░       ░░░░░   ░░░░░░░░░  ░░░░░   ░░░░░ ░░░░░   ░░░░░ "
    };

    std::cout << "\n" << std::string(largura, '=') << "\n\n";
    Menu::imprimirLinhasCentralizadasNaTela(logoFicha, 59, "\033[34m"); 
    std::cout << "\n" << std::string(largura, '=') << "\n\n";

    double multiplicadorDeAtributosAtual = jogadorAtual->obterMultiplicador();
    int quantidadeDeTurnosRestantesDoBuff = jogadorAtual->obterTurnosEfeito("BuffAtributos");
    bool temBuff = (quantidadeDeTurnosRestantesDoBuff > 0 && multiplicadorDeAtributosAtual > 1.0);

    int forcaPerdida = jogadorAtual->possuiEfeito("Fraqueza") ? (jogadorAtual->obterForca() / 3) : 0;
    int destrezaPerdida = jogadorAtual->possuiEfeito("Lentidao") ? jogadorAtual->obterDestreza() : 0; 
    int resPerdida = jogadorAtual->possuiEfeito("QuebraResistencia") ? jogadorAtual->obterResistencia() : 0;
    int constPerdida = jogadorAtual->possuiEfeito("QuebraResistencia") ? (jogadorAtual->obterConstituicao() / 2) : 0;

    int espacos = (largura - 50) / 2;
    std::string margem(espacos > 0 ? espacos : 0, ' ');

    std::cout << margem << "NOME:           " << jogadorAtual->obterNome() << "\n";
    std::cout << margem << "RACA:           " << jogadorAtual->obterRaca()->obterNomeRaca() << "\n";
    std::cout << margem << "CLASSE:         " << jogadorAtual->obterNomeClasse() << "\n";
    std::cout << margem << "NIVEL:          " << jogadorAtual->obterNivel() << " (XP: \033[34m" << jogadorAtual->obterXpAtual() << " / " << jogadorAtual->obterXpParaSubir() << "\033[0m)\n";
    std::cout << margem << "DIFICULDADE:    \033[31m" << (jogadorAtual->obterDificuldade() == 1 ? "Facil" : (jogadorAtual->obterDificuldade() == 2 ? "Normal" : "Dificil")) << "\033[0m\n";
    std::cout << margem << "[PARRY]:        " << (jogadorAtual->obterParryAtivado() ? "\033[32mLigado\033[0m" : "\033[31mDesligado\033[0m") << "\n";
    std::cout << margem << "OURO:           \033[33m" << jogadorAtual->obterInventario()->obterOuro() << "G\033[0m\n\n";

    std::string nomeDaClasse = jogadorAtual->obterNomeClasse();
    std::string passivaNome = "Nenhuma";
    std::string passivaDesc = "";
    std::string recargaHab = "";

    if (nomeDaClasse == "Arqueiro") {
        passivaNome = "Passos leves";
        passivaDesc = "Penalidade de armaduras e debuffs de lentidao reduzidos pela metade.";
        recargaHab = "Recarga: 1 turno.";
    } else if (nomeDaClasse == "Bardo") {
        passivaNome = "Touch the sky";
        passivaDesc = "Curas e buffs recebidos sao 40% mais fortes.";
        recargaHab = "Recarga: 3 turnos (Individuais).";
    } else if (nomeDaClasse == "Guerreiro") {
        passivaNome = "Golpe decisivo";
        passivaDesc = "Causa +10%/+20%/+30% de dano em inimigos com menos de 30%/20%/10% de HP.";
        recargaHab = "Recarga: 3 turnos.";
    } else if (nomeDaClasse == "Mago") {
        passivaNome = "Foco arcano";
        passivaDesc = "Ataques ressoam (25% em area) ou causam +25% de dano em alvo unico.";
        recargaHab = "Recarga: 3 turnos.";
    }

    std::cout << margem << "PASSIVA RACA:   " << jogadorAtual->obterRaca()->obterNomeHabilidadeRaca() << "\n";
    std::cout << margem << "-> " << jogadorAtual->obterRaca()->obterDescricaoHabilidadeRaca() << "\n\n";

    std::cout << margem << "PASSIVA CLASSE: " << passivaNome << "\n";
    std::cout << margem << "-> " << passivaDesc << "\n\n";

    std::cout << margem << "ATIVA CLASSE:   " << jogadorAtual->obterClasse()->obterNomeHabilidadeClasse() << " (" << recargaHab << ")\n";
    std::cout << margem << "-> " << jogadorAtual->obterClasse()->obterDescricaoHabilidadeClasse() << "\n\n";

    std::cout << margem << "--- ATRIBUTOS TOTAIS ---\n";
    std::cout << margem << " > Vida           : " << jogadorAtual->obterVida() << "/" << jogadorAtual->obterVidaMaxima() << "\n";

    auto printAtributo = [&](std::string nome, int valorAtual, int valorPerdido) 
    {
        int valorBase = valorAtual + valorPerdido;
        int bonusBuff = temBuff ? static_cast<int>(valorBase * multiplicadorDeAtributosAtual) - valorBase : 0;
        
        std::cout << margem << " > " << std::left << std::setw(15) << nome << ": " << valorBase;
        
        if (temBuff && bonusBuff > 0) {
            std::cout << " \033[32m(+" << bonusBuff << " Buff)\033[0m";
        }
        if (valorPerdido > 0) {
            std::cout << " \033[31m(-" << valorPerdido << " Debuff)\033[0m";
        }
        if (!temBuff && valorPerdido == 0) {
            std::cout << " (0)";
        }
        std::cout << "\n";
    };

    printAtributo(nomeDaClasse == "Guerreiro" ? "Forca [DANO]" : "Forca", jogadorAtual->obterForca(), forcaPerdida);
    printAtributo(nomeDaClasse == "Arqueiro" ? "Destreza [DANO]" : "Destreza", jogadorAtual->obterDestreza(), destrezaPerdida);
    printAtributo("Resistencia", jogadorAtual->obterResistencia(), resPerdida);
    printAtributo("Constituicao", jogadorAtual->obterConstituicao(), constPerdida);
    printAtributo(nomeDaClasse == "Mago" ? "Inteligencia [DANO]" : "Inteligencia", jogadorAtual->obterInteligencia(), 0);
    printAtributo(nomeDaClasse == "Bardo" ? "Sabedoria [DANO]" : "Sabedoria", jogadorAtual->obterSabedoria(), 0);

    std::cout << "\n" << margem << "--- STATUS ATUAIS ---\n";
    bool temStatus = false;
    if (jogadorAtual->possuiEfeito("BuffAtributos")) { std::cout << margem << "Efeito: \033[32mBuff Atributos (" << jogadorAtual->obterTurnosEfeito("BuffAtributos") << " turnos)\033[0m\n"; temStatus = true; }
    if (jogadorAtual->possuiEfeito("Lentidao")) { std::cout << margem << "Efeito: \033[35mLentidao (" << jogadorAtual->obterTurnosEfeito("Lentidao") << " turnos)\033[0m\n"; temStatus = true; }
    if (jogadorAtual->possuiEfeito("Sangramento")) { std::cout << margem << "Efeito: \033[31mSangramento (" << jogadorAtual->obterTurnosEfeito("Sangramento") << " turnos)\033[0m\n"; temStatus = true; }
    if (jogadorAtual->possuiEfeito("Fraqueza")) { std::cout << margem << "Efeito: \033[31mFraqueza (" << jogadorAtual->obterTurnosEfeito("Fraqueza") << " turnos)\033[0m\n"; temStatus = true; }
    if (jogadorAtual->possuiEfeito("QuebraResistencia")) { std::cout << margem << "Efeito: \033[36mQuebra de Resistencia\033[0m\n"; temStatus = true; }
    if (!temStatus) std::cout << margem << "Nenhum status ativo.\n";

    std::cout << "\n" << std::string(largura, '=') << "\n";
}

void TelaAtributos::gerenciarFichaDoJogador(Personagem* jogadorAtual)
{
    if (jogadorAtual == nullptr) return;
    int larguraDoTerminal = Menu::obterLarguraDoTerminalEmColunas();
    std::string opcaoEscolhidaNoMenuJogador;
    do 
    {
        TelaAtributos::exibir(jogadorAtual);
        std::string mensagemDoPrompt = "[0] VOLTAR | [1] LIGAR/DESLIGAR PARRY";
        if (jogadorAtual->podeSubirDeNivel()) mensagemDoPrompt += " | [2] SUBIR DE NIVEL";
        mensagemDoPrompt += " | [3] VOLTAR AO MENU PRINCIPAL: ";
        int espacosParaCentralizarMensagem = (larguraDoTerminal - (int)mensagemDoPrompt.length()) / 2;
        std::cout << "\n" << std::string(espacosParaCentralizarMensagem > 0 ? espacosParaCentralizarMensagem : 0, ' ') << mensagemDoPrompt;
        std::cin >> opcaoEscolhidaNoMenuJogador;

        if (opcaoEscolhidaNoMenuJogador == "1") {
            jogadorAtual->definirParryAtivado(!jogadorAtual->obterParryAtivado());
        } else if (opcaoEscolhidaNoMenuJogador == "2" && jogadorAtual->podeSubirDeNivel()) {
            int opcaoAtributo;
            std::cout << "\nEscolha o atributo para melhorar:\n";
            std::cout << "1. Vida\n2. Forca\n3. Destreza\n4. Resistencia\n5. Constituicao\n6. Inteligencia\n7. Sabedoria\n";
            std::cout << "Opcao: ";
            if (std::cin >> opcaoAtributo && opcaoAtributo >= 1 && opcaoAtributo <= 7) {
                TipoAtributo atributo = static_cast<TipoAtributo>(opcaoAtributo);
                if (jogadorAtual->subirDeNivel(atributo)) {
                    std::cout << "[SISTEMA]: Nivel subiu! Atributo melhorado.\n";
                    Menu::aguardarPressionamentoDeEnter();
                }
            } else {
                std::cin.clear(); std::cin.ignore(1000, '\n');
                std::cout << "[ERRO]: Opcao invalida.\n";
                Menu::aguardarPressionamentoDeEnter();
            }
        } else if (opcaoEscolhidaNoMenuJogador == "3") {
            std::string confirmacao;
            std::cout << "\n[AVISO]: Deseja realmente voltar ao menu principal? Todo o progresso nao salvo sera perdido. (S/N): ";
            std::cin >> confirmacao;
            if (confirmacao == "S" || confirmacao == "s") {
                std::cout << "[AVISO]: Tem certeza absoluta? (S/N): ";
                std::cin >> confirmacao;
                if (confirmacao == "S" || confirmacao == "s") {
                    jogadorAtual->definirVoltarProMenu(true);
                    return;
                }
            }
        }
    } while (opcaoEscolhidaNoMenuJogador != "0");
}