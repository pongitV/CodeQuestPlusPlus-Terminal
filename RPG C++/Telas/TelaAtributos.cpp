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

    std::vector<std::string> infoGeral;
    infoGeral.push_back("NOME: " + jogadorAtual->obterNome() + "   RACA: " + jogadorAtual->obterRaca()->obterNomeRaca() + "   CLASSE: " + jogadorAtual->obterNomeClasse());
    double porcentagemXp = static_cast<double>(jogadorAtual->obterXpAtual()) / std::max(1, jogadorAtual->obterXpParaSubir());
    std::string barraXp = Aparencia::gerarBarraSuave(porcentagemXp, 10, Aparencia::cor(Cor::CIANO), Aparencia::cor(Cor::CINZA));
    double pctVida = static_cast<double>(jogadorAtual->obterVida()) / std::max(1, jogadorAtual->obterVidaMaxima());
    std::string corVida = (pctVida > 0.7) ? Aparencia::cor(Cor::VERDE) : (pctVida > 0.3) ? Aparencia::cor(Cor::AMARELO) : Aparencia::cor(Cor::VERMELHO);
    std::string barraVida = Aparencia::gerarBarraSuave(pctVida, 10, corVida, Aparencia::cor(Cor::CINZA));

    infoGeral.push_back("NIVEL: " + std::to_string(jogadorAtual->obterNivel()) + " [" + barraXp + Aparencia::cor(Cor::RESET) + "]   HP: [" + barraVida + Aparencia::cor(Cor::RESET) + "] " + std::to_string(jogadorAtual->obterVida()) + "/" + std::to_string(jogadorAtual->obterVidaMaxima()) + "   OURO: " + Aparencia::cor(Cor::AMARELO) + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G" + Aparencia::cor(Cor::RESET));

    std::string difStr = "DIFICULDADE: " + Aparencia::cor(Cor::VERMELHO);
    switch (jogadorAtual->obterDificuldade()) {
        case DificuldadeJogo::Facil: difStr += "Facil"; break;
        case DificuldadeJogo::Normal: difStr += "Normal"; break;
        case DificuldadeJogo::Dificil: difStr += "Dificil"; break;
    }
    difStr += Aparencia::cor(Cor::RESET) + "   [PARRY]: ";
    difStr += jogadorAtual->obterParryAtivado() ? (Aparencia::cor(Cor::VERDE) + "Ligado" + Aparencia::cor(Cor::RESET)) : (Aparencia::cor(Cor::VERMELHO) + "Desligado" + Aparencia::cor(Cor::RESET));
    infoGeral.push_back(difStr);

    std::vector<std::string> caixaGeral = Aparencia::criarCaixa(infoGeral, "", 72, Cor::MAGENTA);

    auto formatarAtr = [temBuff, multiplicadorDeAtributosAtual](std::string nomeDoAtributo, int valorBaseDoAtributo, int valorPerdidoPorDebuff, Cor corBase) -> std::string
    {
        int bonusBuff = temBuff ? static_cast<int>(valorBaseDoAtributo * multiplicadorDeAtributosAtual) - valorBaseDoAtributo : 0;
        double pct = std::min(1.0, valorBaseDoAtributo / 50.0);
        std::string barra = Aparencia::gerarBarraSuave(pct, 10, Aparencia::cor(corBase), Aparencia::cor(Cor::CINZA));

        std::ostringstream ss;
        ss << std::left << std::setw(13) << nomeDoAtributo << ": " << std::setw(3) << valorBaseDoAtributo << " [" << barra << Aparencia::cor(Cor::RESET) << "]";

        if (temBuff && bonusBuff > 0) {
            ss << " " << Aparencia::cor(Cor::VERDE_CLARO) << "(+" << bonusBuff << ")" << Aparencia::cor(Cor::RESET);
        } else if (valorPerdidoPorDebuff > 0) {
            ss << " " << Aparencia::cor(Cor::VERMELHO) << "(-" << valorPerdidoPorDebuff << ")" << Aparencia::cor(Cor::RESET);
        }
        return ss.str();
    };

    std::vector<std::string> atrLinhas;
    atrLinhas.push_back(formatarAtr("Forca", jogadorAtual->obterForca(), forcaPerdida, Cor::VERMELHO_CLARO));
    atrLinhas.push_back(formatarAtr("Destreza", jogadorAtual->obterDestreza(), destrezaPerdida, Cor::VERDE_CLARO));
    atrLinhas.push_back(formatarAtr("Resistencia", jogadorAtual->obterResistencia(), resPerdida, Cor::AMARELO));
    atrLinhas.push_back(formatarAtr("Constituicao", jogadorAtual->obterConstituicao(), constPerdida, Cor::AMARELO));
    atrLinhas.push_back(formatarAtr("Inteligencia", jogadorAtual->obterInteligencia(), 0, Cor::AZUL));
    atrLinhas.push_back(formatarAtr("Sabedoria", jogadorAtual->obterSabedoria(), 0, Cor::CIANO));

    std::vector<std::string> caixaAtributos = Aparencia::criarCaixa(atrLinhas, "ATRIBUTOS", 35, Cor::MAGENTA);

    std::vector<std::string> habLinhas;
    habLinhas.push_back("[PASSIVA]: " + jogadorAtual->obterClasse()->obterNomePassivaClasse());
    habLinhas.push_back("[ATIVA]  : " + jogadorAtual->obterClasse()->obterNomeHabilidadeClasse());
    habLinhas.push_back("");
    habLinhas.push_back("EQUIPAMENTOS:");
    std::string arma = jogadorAtual->obterArma() ? jogadorAtual->obterArma()->obterNomeItem() + jogadorAtual->obterArma()->obterInfoStatus() : "Punhos";
    std::string escudo = jogadorAtual->obterEscudo() ? jogadorAtual->obterEscudo()->obterNomeItem() + jogadorAtual->obterEscudo()->obterInfoStatus() : "Nenhum";
    std::string armadura = jogadorAtual->obterArmadura() ? jogadorAtual->obterArmadura()->obterNomeItem() + jogadorAtual->obterArmadura()->obterInfoStatus() : "Trapos";
    habLinhas.push_back(" > Arma    : " + arma);
    habLinhas.push_back(" > Escudo  : " + escudo);
    habLinhas.push_back(" > Armadura: " + armadura);

    std::vector<std::string> caixaHabilidades = Aparencia::criarCaixa(habLinhas, "HABILIDADES & EQUIPAMENTOS", 35, Cor::MAGENTA);

    static const EfeitoInfo efeitosParaExibir[] = {
        {EfeitoID::BuffAtributos,     Cor::VERDE_CLARO,   "Buff Atributos",   true},
        {EfeitoID::Lentidao,           Cor::MAGENTA, "Lentidao",         true},
        {EfeitoID::Sangramento,        Cor::VERMELHO,"Sangramento",      true},
        {EfeitoID::Fraqueza,           Cor::VERMELHO,"Fraqueza",         true},
        {EfeitoID::QuebraResistencia, Cor::CIANO,   "Quebra de Resistencia", false},
    };

    std::vector<std::string> statusLinhas;
    bool temStatus = false;
    for (const auto& info : efeitosParaExibir) {
        if (jogadorAtual->possuiEfeito(info.efeitoId)) {
            std::string stStr = "Efeito: " + Aparencia::cor(info.corId) + info.exibirNome;
            if (info.mostrarTurnos) {
                stStr += " (" + std::to_string(jogadorAtual->obterTurnosEfeito(info.efeitoId)) + " turnos)";
            }
            stStr += Aparencia::cor(Cor::RESET);
            statusLinhas.push_back(stStr);
            temStatus = true;
        }
    }
    if (!temStatus) statusLinhas.push_back("Nenhum status ativo.");
    std::vector<std::string> caixaStatus = Aparencia::criarCaixa(statusLinhas, "STATUS ATUAIS", 72, Cor::MAGENTA);
    
    Aparencia::imprimirBlocoCentralizado(caixaGeral, "", atrasoMs);
    Aparencia::imprimirLadoALado(caixaAtributos, caixaHabilidades, 40, 2, Cor::RESET, Cor::RESET, atrasoMs);
    Aparencia::imprimirBlocoCentralizado(caixaStatus, "", atrasoMs);

    std::cout << "\n";
    Aparencia::imprimirLinhaDivisoria();
}

void TelaAtributos::gerenciarFichaDoJogador(SistemaPersonagem* jogadorAtual)
{
    if (jogadorAtual == nullptr) return;
    bool continuar = true;
    do 
    {
        TelaAtributos::exibir(jogadorAtual);

        std::vector<std::string> opcoes = { "LIGAR/DESLIGAR PARRY" };
        if (jogadorAtual->podeSubirDeNivel()) opcoes.push_back("SUBIR DE NIVEL");
        opcoes.push_back("DETALHES DE ATRIBUTOS");
        opcoes.push_back("SALVAR E SAIR");
        opcoes.push_back("VOLTAR");

        std::cout << "\n";
        int selecao = ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);
        std::string op = opcoes[selecao];

        if (op == "LIGAR/DESLIGAR PARRY") {
            jogadorAtual->definirParryAtivado(!jogadorAtual->obterParryAtivado());
        } else if (op == "SUBIR DE NIVEL") {
            std::vector<std::string> opcoesAtr = {
                "Vida", "Forca", "Destreza", "Resistencia", "Constituicao", "Inteligencia", "Sabedoria", "Cancelar"
            };
            std::cout << "\n";
            Aparencia::imprimirCentralizado("Escolha o atributo para melhorar:");
            std::cout << "\n";
            int escolhaAtr = ControleDeInput::lerSelecaoMenuComSetas(opcoesAtr, true);
            
            if (escolhaAtr >= 0 && escolhaAtr <= 6) {
                TipoAtributo atributo = static_cast<TipoAtributo>(escolhaAtr + 1);
                if (jogadorAtual->subirDeNivel(atributo)) {
                    Aparencia::exibirPrompt("[SISTEMA]: Nivel subiu! Atributo melhorado.");
                    Aparencia::aguardarEnter();
                }
            }
        } else if (op == "SALVAR E SAIR") {
            std::vector<std::string> opcoesSimNao = { "NAO", "SIM" };
            std::cout << "\n";
            Aparencia::imprimirCentralizado("[AVISO]: Deseja salvar jogo e voltar para o menu principal?");
            std::cout << "\n";
            if (ControleDeInput::lerSelecaoMenuComSetas(opcoesSimNao, true) == 1) {
                Aparencia::imprimirCentralizado("[AVISO]: Tem certeza?");
                std::cout << "\n";
                if (ControleDeInput::lerSelecaoMenuComSetas(opcoesSimNao, true) == 1) {
                    jogadorAtual->definirVoltarProMenu(true);
                    return;
                }
            }
        } else if (op == "DETALHES DE ATRIBUTOS") {
            exibirDetalhesAtributos(jogadorAtual);
        } else if (op == "VOLTAR") {
            continuar = false;
        }
    } while (continuar);
}