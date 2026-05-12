#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <chrono>

#include "TelaAtributos.h"
#include "TelaBase.h"
#include "../Racas/RacaBase.h"
#include "../Classes/ClasseBase.h"
#include "../Utilidades/Aparencia.h"
#include "../Utilidades/ControleDeInput.h"

struct EfeitoInfo {
    EfeitoID efeitoId;
    Cor corId;
    const char* exibirNome;
    bool mostrarTurnos;
};

namespace {
    void exibirDetalhesAtributos(SistemaPersonagem* jogadorAtual) {
        Aparencia::limparTela();
        Aparencia::exibirCabecalho("DETALHES DOS ATRIBUTOS", Cor::MAGENTA);
        
        std::vector<std::string> painelRaca = {"[ RACA: " + jogadorAtual->obterRaca()->obterNomeRaca() + " ]", ""};
        for (const auto& linha : jogadorAtual->obterRaca()->obterAparenciaRaca()) painelRaca.push_back(linha);

        std::vector<std::string> painelClasse = {"[ CLASSE: " + jogadorAtual->obterNomeClasse() + " ]", ""};
        for (const auto& linha : jogadorAtual->obterClasse()->obterAparenciaClasseMenu()) painelClasse.push_back(linha);

        Aparencia::imprimirLadoALado(painelRaca, painelClasse, 0, 10, Cor::BRANCO, Cor::BRANCO, 10);
        std::cout << "\n";
        Aparencia::imprimirLinhaDivisoria();

        std::vector<std::string> linhas;
        linhas.push_back("");
        linhas.push_back("═══ EFEITOS DE CADA ATRIBUTO ═══");
        linhas.push_back(" Vida         : Pontos de vida (HP) maximos do personagem.");
        linhas.push_back(" Forca        : Aumenta o dano base de ataques fisicos.");
        linhas.push_back(" Destreza     : Aumenta o dano fisico final e define a ordem de turno.");
        linhas.push_back(" Resistencia  : Reduz o dano fisico recebido de forma fixa.");
        linhas.push_back(" Constituicao : Reduz o dano fisico recebido em porcentagem.");
        linhas.push_back(" Inteligencia : Aumenta o dano base de ataques magicos.");
        linhas.push_back(" Sabedoria    : Aumenta o dano magico final e a potencia de curas.");
        linhas.push_back("");
        
        linhas.push_back("═══ ATRIBUTOS DE DANO RECOMENDADOS PARA A CLASSE " + jogadorAtual->obterNomeClasse() + " ═══");
        
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

        Aparencia::imprimirBlocoCentralizado(linhas, Aparencia::cor(Cor::BRANCO), 10);
        Aparencia::aguardarEnter();
    }
}

void TelaAtributos::exibir(SistemaPersonagem* jogadorAtual)
{
    if (jogadorAtual == nullptr) return;
    Aparencia::limparTela();

    static auto ultimoAcessoFicha = std::chrono::steady_clock::now() - std::chrono::hours(1);
    int atrasoMs = TelaBase::deveAnimarEntradaDaTela(ultimoAcessoFicha, 300) ? 10 : 0;

    int largura = Aparencia::obterLarguraTerminal();

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

    Aparencia::exibirLogoAscii(logoFicha, 59, Cor::MAGENTA, "", atrasoMs);

    double multiplicadorDeAtributosAtual = jogadorAtual->obterMultiplicador();
    int turnosBuff = jogadorAtual->obterTurnosEfeito(EfeitoID::BuffAtributos);
    bool temBuff = (turnosBuff > 0 && multiplicadorDeAtributosAtual > 1.0);

    int forcaPerdida     = jogadorAtual->possuiEfeito(EfeitoID::Fraqueza)     ? (jogadorAtual->obterForca() / 3)      : 0;
    int destrezaPerdida  = jogadorAtual->possuiEfeito(EfeitoID::Lentidao)     ? jogadorAtual->obterDestreza()       : 0;
    int resPerdida       = jogadorAtual->possuiEfeito(EfeitoID::QuebraResistencia) ? jogadorAtual->obterResistencia() : 0;
    int constPerdida     = jogadorAtual->possuiEfeito(EfeitoID::QuebraResistencia) ? (jogadorAtual->obterConstituicao() / 2) : 0;

    std::vector<std::string> linhasFicha;
    linhasFicha.push_back("NOME:           " + jogadorAtual->obterNome());
    linhasFicha.push_back("RACA:           " + jogadorAtual->obterRaca()->obterNomeRaca());
    linhasFicha.push_back("CLASSE:         " + jogadorAtual->obterNomeClasse());
    linhasFicha.push_back("NIVEL:          " + std::to_string(jogadorAtual->obterNivel()) + " (XP: " + Aparencia::cor(Cor::CIANO) + std::to_string(jogadorAtual->obterXpAtual()) + " / " + std::to_string(jogadorAtual->obterXpParaSubir()) + Aparencia::cor(Cor::RESET) + ")");
    
    std::string difStr = "DIFICULDADE:    " + Aparencia::cor(Cor::VERMELHO);
    switch (jogadorAtual->obterDificuldade()) {
        case DificuldadeJogo::Facil: difStr += "Facil"; break;
        case DificuldadeJogo::Normal: difStr += "Normal"; break;
        case DificuldadeJogo::Dificil: difStr += "Dificil"; break;
    }
    difStr += Aparencia::cor(Cor::RESET);
    linhasFicha.push_back(difStr);

    std::string parryStr = "[PARRY]:        ";
    parryStr += jogadorAtual->obterParryAtivado() ? (Aparencia::cor(Cor::VERDE) + "Ligado" + Aparencia::cor(Cor::RESET)) : (Aparencia::cor(Cor::VERMELHO) + "Desligado" + Aparencia::cor(Cor::RESET));
    linhasFicha.push_back(parryStr);
    
    linhasFicha.push_back("OURO:           " + Aparencia::cor(Cor::AMARELO) + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G" + Aparencia::cor(Cor::RESET));
    linhasFicha.push_back("");

    linhasFicha.push_back("PASSIVA RACA:   " + jogadorAtual->obterRaca()->obterNomeHabilidadeRaca());
    linhasFicha.push_back("-> " + jogadorAtual->obterRaca()->obterDescricaoHabilidadeRaca());
    linhasFicha.push_back("");

    linhasFicha.push_back("PASSIVA CLASSE: " + jogadorAtual->obterClasse()->obterNomePassivaClasse());
    linhasFicha.push_back("-> " + jogadorAtual->obterClasse()->obterDescricaoPassivaClasse());
    linhasFicha.push_back("");

    linhasFicha.push_back("ATIVA CLASSE:   " + jogadorAtual->obterClasse()->obterNomeHabilidadeClasse() + " (" + jogadorAtual->obterClasse()->obterRecargaHabilidadeClasse() + ")");
    linhasFicha.push_back("-> " + jogadorAtual->obterClasse()->obterDescricaoHabilidadeClasse());
    linhasFicha.push_back("");

    Atributos racaAttr = jogadorAtual->obterRaca()->obterAtributosRaca();
    Atributos classeAttr = jogadorAtual->obterClasse()->obterAtributosClasse();

    linhasFicha.push_back("═══ ATRIBUTOS TOTAIS ═══");
    
    std::string hpStr = " > Vida           : " + std::to_string(jogadorAtual->obterVida()) + "/" + std::to_string(jogadorAtual->obterVidaMaxima());
    int padHp = 45 - static_cast<int>(hpStr.length());
    std::ostringstream linhaHp;
    linhaHp << hpStr << std::string(padHp > 0 ? padHp : 0, ' ') 
            << "║ Raca: +" << std::left << std::setw(3) << racaAttr.vida 
            << " ║ Classe: +" << classeAttr.vida;
    linhasFicha.push_back(linhaHp.str());

    auto formatarAtributo = [temBuff, multiplicadorDeAtributosAtual](std::string nomeDoAtributo, int valorBaseDoAtributo, int valorPerdidoPorDebuff, int atrRaca, int atrClasse) -> std::string
    {
        int bonusBuff = temBuff ? static_cast<int>(valorBaseDoAtributo * multiplicadorDeAtributosAtual) - valorBaseDoAtributo : 0;

        std::ostringstream leftPart;
        leftPart << " > " << std::left << std::setw(15) << nomeDoAtributo << ": " << valorBaseDoAtributo;
        std::string leftStr = leftPart.str();

        std::string extraText;
        std::string extraColor;
        if (temBuff && bonusBuff > 0) {
            extraColor = Aparencia::cor(Cor::VERDE_CLARO);
            extraText = "(+" + std::to_string(bonusBuff) + " Buff)";
        } else if (valorPerdidoPorDebuff > 0) {
            extraColor = Aparencia::cor(Cor::VERMELHO);
            extraText = "(-" + std::to_string(valorPerdidoPorDebuff) + " Debuff)";
        } else {
            extraText = "(0)";
        }

        int visualLen = leftStr.length() + 1 + extraText.length();
        int padding = 45 - visualLen;

        std::ostringstream finalLine;
        finalLine << leftStr << " " << extraColor << extraText << (extraColor.empty() ? "" : Aparencia::cor(Cor::RESET))
                  << std::string(padding > 0 ? padding : 0, ' ')
                  << "║ Raca: +" << std::left << std::setw(3) << atrRaca
                  << " ║ Classe: +" << atrClasse;
        return finalLine.str();
    };

    linhasFicha.push_back(formatarAtributo("Forca",     jogadorAtual->obterForca(),     forcaPerdida,     racaAttr.forca, classeAttr.forca));
    linhasFicha.push_back(formatarAtributo("Destreza",  jogadorAtual->obterDestreza(),  destrezaPerdida,  racaAttr.destreza, classeAttr.destreza));
    linhasFicha.push_back(formatarAtributo("Resistencia", jogadorAtual->obterResistencia(), resPerdida, racaAttr.resistencia, classeAttr.resistencia));
    linhasFicha.push_back(formatarAtributo("Constituicao", jogadorAtual->obterConstituicao(), constPerdida, racaAttr.constituicao, classeAttr.constituicao));
    linhasFicha.push_back(formatarAtributo("Inteligencia", jogadorAtual->obterInteligencia(), 0, racaAttr.inteligencia, classeAttr.inteligencia));
    linhasFicha.push_back(formatarAtributo("Sabedoria",  jogadorAtual->obterSabedoria(), 0, racaAttr.sabedoria, classeAttr.sabedoria));

    static const EfeitoInfo efeitosParaExibir[] = {
        {EfeitoID::BuffAtributos,     Cor::VERDE_CLARO,   "Buff Atributos",   true},
        {EfeitoID::Lentidao,           Cor::MAGENTA, "Lentidao",         true},
        {EfeitoID::Sangramento,        Cor::VERMELHO,"Sangramento",      true},
        {EfeitoID::Fraqueza,           Cor::VERMELHO,"Fraqueza",         true},
        {EfeitoID::QuebraResistencia, Cor::CIANO,   "Quebra de Resistencia", false},
    };

    linhasFicha.push_back("");
    linhasFicha.push_back("═══ STATUS ATUAIS ═══");
    bool temStatus = false;
    for (const auto& info : efeitosParaExibir) {
        if (jogadorAtual->possuiEfeito(info.efeitoId)) {
            std::string stStr = "Efeito: " + Aparencia::cor(info.corId) + info.exibirNome;
            if (info.mostrarTurnos) {
                stStr += " (" + std::to_string(jogadorAtual->obterTurnosEfeito(info.efeitoId)) + " turnos)";
            }
            stStr += Aparencia::cor(Cor::RESET);
            linhasFicha.push_back(stStr);
            temStatus = true;
        }
    }
    if (!temStatus) linhasFicha.push_back("Nenhum status ativo.");
    
    Aparencia::imprimirBlocoCentralizado(linhasFicha, "", atrasoMs);

    std::cout << "\n";
    Aparencia::imprimirLinhaDivisoria();
}

void TelaAtributos::gerenciarFichaDoJogador(SistemaPersonagem* jogadorAtual)
{
    if (jogadorAtual == nullptr) return;
    int larguraDoTerminal = Aparencia::obterLarguraTerminal();
    std::string opcaoEscolhidaNoMenuJogador;
    do 
    {
        TelaAtributos::exibir(jogadorAtual);
        std::string mensagemDoPrompt = "[0] VOLTAR | [1] LIGAR/DESLIGAR PARRY";
        if (jogadorAtual->podeSubirDeNivel()) mensagemDoPrompt += " | [2] SUBIR DE NIVEL";
        mensagemDoPrompt += " | [3] SALVAR E SAIR | [4] DETALHES DE ATRIBUTOS: ";
        Aparencia::exibirPrompt(mensagemDoPrompt);
        opcaoEscolhidaNoMenuJogador = ControleDeInput::lerEntradaProtegida();

        if (opcaoEscolhidaNoMenuJogador == "1") {
            jogadorAtual->definirParryAtivado(!jogadorAtual->obterParryAtivado());
        } else if (opcaoEscolhidaNoMenuJogador == "2" && jogadorAtual->podeSubirDeNivel()) {
            int opcaoAtributo;
            std::vector<std::string> opcoes = {
                "Escolha o atributo para melhorar:",
                "",
                "1. Vida", "2. Forca", "3. Destreza", "4. Resistencia", "5. Constituicao", "6. Inteligencia", "7. Sabedoria"
            };
            std::cout << "\n";
            Aparencia::imprimirBlocoCentralizado(opcoes);
            std::cout << "\n";
            Aparencia::exibirPrompt("Opcao: ");

            std::string entradaLvl = ControleDeInput::lerEntradaProtegida();
            try {
                opcaoAtributo = std::stoi(entradaLvl);
                if (opcaoAtributo >= 1 && opcaoAtributo <= 7) {
                    TipoAtributo atributo = static_cast<TipoAtributo>(opcaoAtributo);
                    if (jogadorAtual->subirDeNivel(atributo)) {
                        Aparencia::exibirPrompt("[SISTEMA]: Nivel subiu! Atributo melhorado.");
                        Aparencia::aguardarEnter();
                    }
                }
            } catch (...) {}
        } else if (opcaoEscolhidaNoMenuJogador == "3") {
            std::string confirmacao;
            Aparencia::exibirPrompt("[AVISO]: Deseja salvar jogo e voltar para o menu principal? (S/N): ");
            confirmacao = ControleDeInput::lerEntradaProtegida();
            if (confirmacao == "S" || confirmacao == "s") {
                Aparencia::exibirPrompt("[AVISO]: Tem certeza? (S/N): ");
                confirmacao = ControleDeInput::lerEntradaProtegida();
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