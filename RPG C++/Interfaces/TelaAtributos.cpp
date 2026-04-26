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
#include "../Sistema/Tipos.h"

struct EfeitoInfo {
    const char* efeitoNome;
    const char* cor;
    const char* exibirNome;
    bool mostrarTurnos;
};

// Cores ANSI
constexpr auto COR_VERDE   = "\033[32m";
constexpr auto COR_VERMELHO = "\033[31m";
constexpr auto COR_LARANJA = "\033[33m";
constexpr auto COR_ROXO    = "\033[35m";
constexpr auto COR_CIANO   = "\033[36m";
constexpr auto COR_RESET   = "\033[0m";

void TelaAtributos::exibir(Personagem* jogadorAtual)
{
    if (jogadorAtual == nullptr) return;
    Menu::limparTelaDoTerminal();

#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif

    int largura = Menu::obterLarguraDoTerminalEmColunas();

    static const std::vector<std::string> logoFicha =
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
    Menu::imprimirLinhasCentralizadasNaTela(logoFicha, 59, COR_ROXO);
    std::cout << "\n" << std::string(largura, '=') << "\n\n";

    double multiplicadorDeAtributosAtual = jogadorAtual->obterMultiplicador();
    int turnosBuff = jogadorAtual->obterTurnosEfeito(EfeitoNomes::BUFF_ATRIBUTOS);
    bool temBuff = (turnosBuff > 0 && multiplicadorDeAtributosAtual > 1.0);

    int forcaPerdida     = jogadorAtual->possuiEfeito(EfeitoNomes::FRAQUEZA)     ? (jogadorAtual->obterForca() / 3)      : 0;
    int destrezaPerdida  = jogadorAtual->possuiEfeito(EfeitoNomes::LENTIDAO)     ? jogadorAtual->obterDestreza()       : 0;
    int resPerdida       = jogadorAtual->possuiEfeito(EfeitoNomes::QUEBRA_RESISTENCIA) ? jogadorAtual->obterResistencia() : 0;
    int constPerdida     = jogadorAtual->possuiEfeito(EfeitoNomes::QUEBRA_RESISTENCIA) ? (jogadorAtual->obterConstituicao() / 2) : 0;

    int espacos = (largura - 50) / 2;
    std::string margem(std::max(0, espacos), ' ');

    std::cout << margem << "NOME:           " << jogadorAtual->obterNome() << "\n";
    std::cout << margem << "RACA:           " << jogadorAtual->obterRaca()->obterNomeRaca() << "\n";
    std::cout << margem << "CLASSE:         " << jogadorAtual->obterNomeClasse() << "\n";
    std::cout << margem << "NIVEL:          " << jogadorAtual->obterNivel() << " (XP: " << COR_ROXO << jogadorAtual->obterXpAtual() << " / " << jogadorAtual->obterXpParaSubir() << COR_RESET << ")\n";
    std::cout << margem << "DIFICULDADE:    " << COR_VERMELHO;
    switch (jogadorAtual->obterDificuldade()) {
        case 1: std::cout << "Facil"; break;
        case 2: std::cout << "Normal"; break;
        default: std::cout << "Dificil"; break;
    }
    std::cout << COR_RESET << "\n";
    std::cout << margem << "[PARRY]:        ";
    if (jogadorAtual->obterParryAtivado()) {
        std::cout << COR_VERDE << "Ligado" << COR_RESET;
    } else {
        std::cout << COR_VERMELHO << "Desligado" << COR_RESET;
    }
    std::cout << "\n";
    std::cout << margem << "OURO:           " << COR_LARANJA << jogadorAtual->obterInventario()->obterOuro() << "G" << COR_RESET << "\n\n";

    TipoClasse tipoClasse = jogadorAtual->obterTipoClasse();
    std::string passivaNome = "Nenhuma";
    std::string passivaDesc = "";
    std::string recargaHab = "";

    switch (tipoClasse) {
        case TipoClasse::Arqueiro:
            passivaNome = "Passos leves";
            passivaDesc = "Penalidade de armaduras e debuffs de lentidao reduzidos pela metade.";
            recargaHab = "Recarga: 1 turno.";
            break;
        case TipoClasse::Bardo:
            passivaNome = "Touch the sky";
            passivaDesc = "Curas e buffs recebidos sao 40% mais fortes.";
            recargaHab = "Recarga: 3 turnos (Individuais).";
            break;
        case TipoClasse::Guerreiro:
            passivaNome = "Golpe decisivo";
            passivaDesc = "Causa +10%/+20%/+30% de dano em inimigos com menos de 30%/20%/10% de HP.";
            recargaHab = "Recarga: 3 turnos.";
            break;
        case TipoClasse::Mago:
            passivaNome = "Foco arcano";
            passivaDesc = "Ataques ressoam (25% em area) ou causam +25% de dano em alvo unico.";
            recargaHab = "Recarga: 3 turnos.";
            break;
        default:
            break;
    }

    std::cout << margem << "PASSIVA RACA:   " << jogadorAtual->obterRaca()->obterNomeHabilidadeRaca() << "\n";
    std::cout << margem << "-> " << jogadorAtual->obterRaca()->obterDescricaoHabilidadeRaca() << "\n\n";

    std::cout << margem << "PASSIVA CLASSE: " << passivaNome << "\n";
    std::cout << margem << "-> " << passivaDesc << "\n\n";

    std::cout << margem << "ATIVA CLASSE:   " << jogadorAtual->obterClasse()->obterNomeHabilidadeClasse() << " (" << recargaHab << ")\n";
    std::cout << margem << "-> " << jogadorAtual->obterClasse()->obterDescricaoHabilidadeClasse() << "\n\n";

    std::cout << margem << "--- ATRIBUTOS TOTAIS ---\n";
    std::cout << margem << " > Vida           : " << jogadorAtual->obterVida() << "/" << jogadorAtual->obterVidaMaxima() << "\n";

    auto printAtributo = [margem, temBuff, multiplicadorDeAtributosAtual](std::string nome, int valorBase, int valorPerdido, std::string_view sufixo)
    {
        int bonusBuff = temBuff ? static_cast<int>(valorBase * multiplicadorDeAtributosAtual) - valorBase : 0;

        std::cout << margem << " > " << std::left << std::setw(15) << nome << sufixo << ": " << valorBase;

        if (temBuff && bonusBuff > 0) {
            std::cout << " " << COR_VERDE << "(+" << bonusBuff << " Buff)" << COR_RESET;
        }
        if (valorPerdido > 0) {
            std::cout << " " << COR_VERMELHO << "(-" << valorPerdido << " Debuff)" << COR_RESET;
        }
        if (!temBuff && valorPerdido == 0) {
            std::cout << " (0)";
        }
        std::cout << "\n";
    };

    printAtributo("Forca",     jogadorAtual->obterForca(),     forcaPerdida,     (tipoClasse == TipoClasse::Guerreiro) ? " [DANO]" : "");
    printAtributo("Destreza",  jogadorAtual->obterDestreza(),  destrezaPerdida,  (tipoClasse == TipoClasse::Arqueiro)  ? " [DANO]" : "");
    printAtributo("Resistencia", jogadorAtual->obterResistencia(), resPerdida, "");
    printAtributo("Constituicao", jogadorAtual->obterConstituicao(), constPerdida, "");
    printAtributo("Inteligencia", jogadorAtual->obterInteligencia(), 0, (tipoClasse == TipoClasse::Mago) ? " [DANO]" : "");
    printAtributo("Sabedoria",  jogadorAtual->obterSabedoria(), 0, (tipoClasse == TipoClasse::Bardo)  ? " [DANO]" : "");

    static const EfeitoInfo efeitosParaExibir[] = {
        {EfeitoNomes::BUFF_ATRIBUTOS,   COR_VERDE,   "Buff Atributos",   true},
        {EfeitoNomes::LENTIDAO,         COR_ROXO,    "Lentidao",         true},
        {EfeitoNomes::SANGRAMENTO,      COR_VERMELHO,"Sangramento",      true},
        {EfeitoNomes::FRAQUEZA,         COR_VERMELHO,"Fraqueza",         true},
        {EfeitoNomes::QUEBRA_RESISTENCIA, COR_CIANO, "Quebra de Resistencia", false},
    };

    std::cout << "\n" << margem << "--- STATUS ATUAIS ---\n";
    bool temStatus = false;
    for (const auto& info : efeitosParaExibir) {
        if (jogadorAtual->possuiEfeito(info.efeitoNome)) {
            std::cout << margem << "Efeito: " << info.cor << info.exibirNome;
            if (info.mostrarTurnos) {
                std::cout << " (" << jogadorAtual->obterTurnosEfeito(info.efeitoNome) << " turnos)";
            }
            std::cout << COR_RESET << "\n";
            temStatus = true;
        }
    }
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
        std::cout << "\n" << std::string(std::max(0, espacosParaCentralizarMensagem), ' ') << mensagemDoPrompt;
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
            std::cout << "\n[AVISO]: Deseja realmente voltar ao menu principal? Todo o progresso sera perdido. (S/N): ";
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