#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

#ifdef _WIN32
    #include <windows.h>
#endif

#include "TelaAtributos.h"
#include "../Sistema/FuncionalidadeMenu.h"
#include "../Raças/RacaBase.h"
#include "../Classes/ClasseBase.h"
#include "../Sistema/Tipos.h"
#include "../Sistema/SimplificacoesAparencia.h"

struct EfeitoInfo {
    const char* efeitoNome;
    int corId;
    const char* exibirNome;
    bool mostrarTurnos;
};

void TelaAtributos::exibir(Personagem* jogadorAtual)
{
    if (jogadorAtual == nullptr) return;
    SimplificacoesAparencia::limparTela();

#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif

    int largura = SimplificacoesAparencia::obterLarguraTerminal();

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
    SimplificacoesAparencia::imprimirCentralizadoMultilinha(logoFicha, 59, SimplificacoesAparencia::cor(Cor::MAGENTA));
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
    std::cout << margem << "NIVEL:          " << jogadorAtual->obterNivel() << " (XP: " << SimplificacoesAparencia::cor(Cor::MAGENTA) << jogadorAtual->obterXpAtual() << " / " << jogadorAtual->obterXpParaSubir() << SimplificacoesAparencia::cor(Cor::RESET) << ")\n";
    std::cout << margem << "DIFICULDADE:    " << SimplificacoesAparencia::cor(Cor::VERMELHO);
    switch (jogadorAtual->obterDificuldade()) {
        case 1: std::cout << "Facil"; break;
        case 2: std::cout << "Normal"; break;
        default: std::cout << "Dificil"; break;
    }
    std::cout << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
    std::cout << margem << "[PARRY]:        ";
    if (jogadorAtual->obterParryAtivado()) {
        std::cout << SimplificacoesAparencia::cor(Cor::VERDE) << "Ligado" << SimplificacoesAparencia::cor(Cor::RESET);
    } else {
        std::cout << SimplificacoesAparencia::cor(Cor::VERMELHO) << "Desligado" << SimplificacoesAparencia::cor(Cor::RESET);
    }
    std::cout << "\n";
    std::cout << margem << "OURO:           " << SimplificacoesAparencia::cor(Cor::AMARELO) << jogadorAtual->obterInventario()->obterOuro() << "G" << SimplificacoesAparencia::cor(Cor::RESET) << "\n\n";

    TipoClasse tipoClasse = jogadorAtual->obterTipoClasse();

    std::cout << margem << "PASSIVA RACA:   " << jogadorAtual->obterRaca()->obterNomeHabilidadeRaca() << "\n";
    std::cout << margem << "-> " << jogadorAtual->obterRaca()->obterDescricaoHabilidadeRaca() << "\n\n";

    std::cout << margem << "PASSIVA CLASSE: " << jogadorAtual->obterClasse()->obterNomePassivaClasse() << "\n";
    std::cout << margem << "-> " << jogadorAtual->obterClasse()->obterDescricaoPassivaClasse() << "\n\n";

    std::cout << margem << "ATIVA CLASSE:   " << jogadorAtual->obterClasse()->obterNomeHabilidadeClasse() << " (" << jogadorAtual->obterClasse()->obterRecargaHabilidadeClasse() << ")\n";
    std::cout << margem << "-> " << jogadorAtual->obterClasse()->obterDescricaoHabilidadeClasse() << "\n\n";

    std::cout << margem << "--- ATRIBUTOS TOTAIS ---\n";
    std::cout << margem << " > Vida           : " << jogadorAtual->obterVida() << "/" << jogadorAtual->obterVidaMaxima() << "\n";

    auto printAtributo = [margem, temBuff, multiplicadorDeAtributosAtual](std::string nome, int valorBase, int valorPerdido, std::string_view sufixo)
    {
        int bonusBuff = temBuff ? static_cast<int>(valorBase * multiplicadorDeAtributosAtual) - valorBase : 0;

        std::cout << margem << " > " << std::left << std::setw(15) << nome << sufixo << ": " << valorBase;

        if (temBuff && bonusBuff > 0) {
            std::cout << " " << SimplificacoesAparencia::cor(Cor::VERDE) << "(+" << bonusBuff << " Buff)" << SimplificacoesAparencia::cor(Cor::RESET);
        }
        if (valorPerdido > 0) {
            std::cout << " " << SimplificacoesAparencia::cor(Cor::VERMELHO) << "(-" << valorPerdido << " Debuff)" << SimplificacoesAparencia::cor(Cor::RESET);
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
        {EfeitoNomes::BUFF_ATRIBUTOS,     Cor::VERDE,   "Buff Atributos",   true},
        {EfeitoNomes::LENTIDAO,           Cor::MAGENTA, "Lentidao",         true},
        {EfeitoNomes::SANGRAMENTO,        Cor::VERMELHO,"Sangramento",      true},
        {EfeitoNomes::FRAQUEZA,           Cor::VERMELHO,"Fraqueza",         true},
        {EfeitoNomes::QUEBRA_RESISTENCIA, Cor::CIANO,   "Quebra de Resistencia", false},
    };

    std::cout << "\n" << margem << "--- STATUS ATUAIS ---\n";
    bool temStatus = false;
    for (const auto& info : efeitosParaExibir) {
        if (jogadorAtual->possuiEfeito(info.efeitoNome)) {
            std::cout << margem << "Efeito: " << SimplificacoesAparencia::cor(info.corId) << info.exibirNome;
            if (info.mostrarTurnos) {
                std::cout << " (" << jogadorAtual->obterTurnosEfeito(info.efeitoNome) << " turnos)";
            }
            std::cout << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
            temStatus = true;
        }
    }
    if (!temStatus) std::cout << margem << "Nenhum status ativo.\n";

    std::cout << "\n" << std::string(largura, '=') << "\n";
}

void TelaAtributos::gerenciarFichaDoJogador(Personagem* jogadorAtual)
{
    if (jogadorAtual == nullptr) return;
    int larguraDoTerminal = SimplificacoesAparencia::obterLarguraTerminal();
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
                    SimplificacoesAparencia::aguardarEnter();
                }
            } else {
                std::cin.clear(); std::cin.ignore(1000, '\n');
                std::cout << "[ERRO]: Opcao invalida.\n";
                SimplificacoesAparencia::aguardarEnter();
            }
        } else if (opcaoEscolhidaNoMenuJogador == "3") {
            std::string confirmacao;
            std::cout << "\n[AVISO]: Deseja salvar jogo e voltar para o menu principal? (S/N): ";
            std::cin >> confirmacao;
            if (confirmacao == "S" || confirmacao == "s") {
                std::cout << "[AVISO]: Tem certeza? (S/N): ";
                std::cin >> confirmacao;
                if (confirmacao == "S" || confirmacao == "s") {
                    jogadorAtual->definirVoltarProMenu(true);
                    return;
                }
            }
        }
    } while (opcaoEscolhidaNoMenuJogador != "0");
}