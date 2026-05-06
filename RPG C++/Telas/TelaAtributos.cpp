#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

#include "TelaAtributos.h"
#include "../Racas/RacaBase.h"
#include "../Classes/ClasseBase.h"
#include "../Utilidades/SimplificacoesAparencia.h"

struct EfeitoInfo {
    EfeitoID efeitoId;
    Cor corId;
    const char* exibirNome;
    bool mostrarTurnos;
};

namespace {
    void exibirDetalhesAtributos(SistemaPersonagem* jogadorAtual) {
        SimplificacoesAparencia::limparTela();
        SimplificacoesAparencia::exibirCabecalho("DETALHES DOS ATRIBUTOS", Cor::MAGENTA);
        
        std::vector<std::string> linhas;
        linhas.push_back("");
        linhas.push_back("--- EFEITOS DE CADA ATRIBUTO ---");
        linhas.push_back(" Vida         : Pontos de vida (HP) maximos do personagem.");
        linhas.push_back(" Forca        : Aumenta o dano base de ataques fisicos.");
        linhas.push_back(" Destreza     : Aumenta o dano fisico final e define a ordem de turno.");
        linhas.push_back(" Resistencia  : Reduz o dano fisico recebido de forma fixa.");
        linhas.push_back(" Constituicao : Reduz o dano fisico recebido em porcentagem.");
        linhas.push_back(" Inteligencia : Aumenta o dano base de ataques magicos.");
        linhas.push_back(" Sabedoria    : Aumenta o dano magico final e a potencia de curas.");
        linhas.push_back("");
        
        linhas.push_back("--- ATRIBUTOS DE DANO RECOMENDADOS PARA A CLASSE " + jogadorAtual->obterNomeClasse() + " ---");
        
        TipoClasse tipo = jogadorAtual->obterTipoClasse();
        if (tipo == TipoClasse::Guerreiro) {
            linhas.push_back(" 1. Forca    : Aumenta o dano base, essencial para armas pesadas.");
            linhas.push_back(" 2. Destreza : Multiplica o dano final, util para qualquer build fisica.");
        } else if (tipo == TipoClasse::Arqueiro) {
            linhas.push_back(" 1. Destreza : Atributo principal, aumenta o dano e define a ordem de turno.");
            linhas.push_back(" 2. Forca    : Aumenta o dano base, fortalecendo o dano fisico geral.");
        } else if (tipo == TipoClasse::Mago) {
            linhas.push_back(" 1. Inteligencia : Essencial, aumenta drasticamente o dano base de magias.");
            linhas.push_back(" 2. Sabedoria    : Multiplica o dano magico final e fortalece habilidades.");
        } else if (tipo == TipoClasse::Bardo) {
            linhas.push_back(" 1. Sabedoria    : Fortalece intensamente os efeitos das curas e utilidade do Bardo.");
            linhas.push_back(" 2. Inteligencia : Melhora o dano magico, permitindo que o Bardo lute efetivamente.");
        } else {
            linhas.push_back(" Nenhum atributo de dano especifico definido para esta classe.");
        }

        int tamanhoDaLinhaMaisLonga = 0;
        for (const std::string& linha : linhas) {
            if (static_cast<int>(linha.length()) > tamanhoDaLinhaMaisLonga) tamanhoDaLinhaMaisLonga = linha.length();
        }
        
        SimplificacoesAparencia::imprimirCentralizadoMultilinha(linhas, tamanhoDaLinhaMaisLonga, SimplificacoesAparencia::cor(Cor::BRANCO));
        SimplificacoesAparencia::aguardarEnter();
    }
}

void TelaAtributos::exibir(SistemaPersonagem* jogadorAtual)
{
    if (jogadorAtual == nullptr) return;
    SimplificacoesAparencia::limparTela();

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

    std::cout << "\n";
    SimplificacoesAparencia::imprimirLinhaDivisoria();
    std::cout << "\n";
    SimplificacoesAparencia::imprimirCentralizadoMultilinha(logoFicha, 59, SimplificacoesAparencia::cor(Cor::MAGENTA));
    std::cout << "\n";
    SimplificacoesAparencia::imprimirLinhaDivisoria();
    std::cout << "\n";

    double multiplicadorDeAtributosAtual = jogadorAtual->obterMultiplicador();
    int turnosBuff = jogadorAtual->obterTurnosEfeito(EfeitoID::BuffAtributos);
    bool temBuff = (turnosBuff > 0 && multiplicadorDeAtributosAtual > 1.0);

    int forcaPerdida     = jogadorAtual->possuiEfeito(EfeitoID::Fraqueza)     ? (jogadorAtual->obterForca() / 3)      : 0;
    int destrezaPerdida  = jogadorAtual->possuiEfeito(EfeitoID::Lentidao)     ? jogadorAtual->obterDestreza()       : 0;
    int resPerdida       = jogadorAtual->possuiEfeito(EfeitoID::QuebraResistencia) ? jogadorAtual->obterResistencia() : 0;
    int constPerdida     = jogadorAtual->possuiEfeito(EfeitoID::QuebraResistencia) ? (jogadorAtual->obterConstituicao() / 2) : 0;

    std::string margem = SimplificacoesAparencia::espacosParaCentralizar(50);

    std::cout << margem << "NOME:           " << jogadorAtual->obterNome() << "\n";
    std::cout << margem << "RACA:           " << jogadorAtual->obterRaca()->obterNomeRaca() << "\n";
    std::cout << margem << "CLASSE:         " << jogadorAtual->obterNomeClasse() << "\n";
    std::cout << margem << "NIVEL:          " << jogadorAtual->obterNivel() << " (XP: " << SimplificacoesAparencia::cor(Cor::MAGENTA) << jogadorAtual->obterXpAtual() << " / " << jogadorAtual->obterXpParaSubir() << SimplificacoesAparencia::cor(Cor::RESET) << ")\n";
    std::cout << margem << "DIFICULDADE:    " << SimplificacoesAparencia::cor(Cor::VERMELHO);
    switch (jogadorAtual->obterDificuldade()) {
        case DificuldadeJogo::Facil: std::cout << "Facil"; break;
        case DificuldadeJogo::Normal: std::cout << "Normal"; break;
        case DificuldadeJogo::Dificil: std::cout << "Dificil"; break;
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

    auto printAtributo = [margem, temBuff, multiplicadorDeAtributosAtual](std::string nomeDoAtributo, int valorBaseDoAtributo, int valorPerdidoPorDebuff, std::string_view sufixoOpcional)
    {
        int bonusBuff = temBuff ? static_cast<int>(valorBaseDoAtributo * multiplicadorDeAtributosAtual) - valorBaseDoAtributo : 0;

        std::cout << margem << " > " << std::left << std::setw(15) << nomeDoAtributo << sufixoOpcional << ": " << valorBaseDoAtributo;

        if (temBuff && bonusBuff > 0) {
            std::cout << " " << SimplificacoesAparencia::cor(Cor::VERDE) << "(+" << bonusBuff << " Buff)" << SimplificacoesAparencia::cor(Cor::RESET);
        }
        if (valorPerdidoPorDebuff > 0) {
            std::cout << " " << SimplificacoesAparencia::cor(Cor::VERMELHO) << "(-" << valorPerdidoPorDebuff << " Debuff)" << SimplificacoesAparencia::cor(Cor::RESET);
        }
        if (!temBuff && valorPerdidoPorDebuff == 0) {
            std::cout << " (0)";
        }
        std::cout << "\n";
    };

    printAtributo("Forca",     jogadorAtual->obterForca(),     forcaPerdida,     "");
    printAtributo("Destreza",  jogadorAtual->obterDestreza(),  destrezaPerdida,  "");
    printAtributo("Resistencia", jogadorAtual->obterResistencia(), resPerdida, "");
    printAtributo("Constituicao", jogadorAtual->obterConstituicao(), constPerdida, "");
    printAtributo("Inteligencia", jogadorAtual->obterInteligencia(), 0, "");
    printAtributo("Sabedoria",  jogadorAtual->obterSabedoria(), 0, "");

    static const EfeitoInfo efeitosParaExibir[] = {
        {EfeitoID::BuffAtributos,     Cor::VERDE,   "Buff Atributos",   true},
        {EfeitoID::Lentidao,           Cor::MAGENTA, "Lentidao",         true},
        {EfeitoID::Sangramento,        Cor::VERMELHO,"Sangramento",      true},
        {EfeitoID::Fraqueza,           Cor::VERMELHO,"Fraqueza",         true},
        {EfeitoID::QuebraResistencia, Cor::CIANO,   "Quebra de Resistencia", false},
    };

    std::cout << "\n" << margem << "--- STATUS ATUAIS ---\n";
    bool temStatus = false;
    for (const auto& info : efeitosParaExibir) {
        if (jogadorAtual->possuiEfeito(info.efeitoId)) {
            std::cout << margem << "Efeito: " << SimplificacoesAparencia::cor(info.corId) << info.exibirNome;
            if (info.mostrarTurnos) {
                std::cout << " (" << jogadorAtual->obterTurnosEfeito(info.efeitoId) << " turnos)";
            }
            std::cout << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
            temStatus = true;
        }
    }
    if (!temStatus) std::cout << margem << "Nenhum status ativo.\n";

    std::cout << "\n";
    SimplificacoesAparencia::imprimirLinhaDivisoria();
}

void TelaAtributos::gerenciarFichaDoJogador(SistemaPersonagem* jogadorAtual)
{
    if (jogadorAtual == nullptr) return;
    int larguraDoTerminal = SimplificacoesAparencia::obterLarguraTerminal();
    std::string opcaoEscolhidaNoMenuJogador;
    do 
    {
        TelaAtributos::exibir(jogadorAtual);
        std::string mensagemDoPrompt = "[0] VOLTAR | [1] LIGAR/DESLIGAR PARRY";
        if (jogadorAtual->podeSubirDeNivel()) mensagemDoPrompt += " | [2] SUBIR DE NIVEL";
        mensagemDoPrompt += " | [3] VOLTAR AO MENU PRINCIPAL | [4] DETALHES DE ATRIBUTOS: ";
        std::cout << "\n" << SimplificacoesAparencia::espacosParaCentralizar(mensagemDoPrompt.length()) << mensagemDoPrompt;
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
        } else if (opcaoEscolhidaNoMenuJogador == "4") {
            exibirDetalhesAtributos(jogadorAtual);
        }
    } while (opcaoEscolhidaNoMenuJogador != "0");
}