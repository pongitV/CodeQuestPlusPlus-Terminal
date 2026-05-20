#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <chrono>

#include "TelaAtributos.h"
#include "../TelaBase.h"
#include "../../../Entidades/Racas/RacaBase.h"
#include "../../../Entidades/Classes/ClasseBase.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "TelaAtributosLayout.h"

struct EfeitoInfo {
    EfeitoID efeitoId;
    Cor corId;
    const char* exibirNome;
    bool mostrarTurnos;
};

namespace {
    void exibirDetalhesAtributos(Personagem* jogadorAtual) {
        Aparencia::limparTela();
        Aparencia::exibirPainelTexto("DETALHES DOS ATRIBUTOS", Cor::MAGENTA);
        
        std::vector<std::string> painelRaca = {"[ RACA: " + jogadorAtual->obterRaca()->obterNomeRaca() + " ]", ""};
        for (const auto& linha : jogadorAtual->obterRaca()->obterAparenciaRaca()) painelRaca.push_back(linha);

        std::vector<std::string> painelClasse = {"[ CLASSE: " + jogadorAtual->obterNomeClasse() + " ]", ""};
        for (const auto& linha : jogadorAtual->obterClasse()->obterAparenciaClasseMenu()) painelClasse.push_back(linha);

        Aparencia::imprimirLadoALado(painelRaca, painelClasse, 0, 10, Cor::BRANCO, Cor::BRANCO, 10);
        std::cout << "\n";
        Aparencia::imprimirLinhaDivisoria();

        std::vector<std::string> linhas;
        std::string cB = Aparencia::cor(Cor::BRANCO);
        linhas.push_back("");
        linhas.push_back("═══ EFEITOS DE CADA ATRIBUTO ═══");
        linhas.push_back(" " + Aparencia::cor(Cor::VERDE)          + "Vida         " + cB + ": Pontos de vida (HP) maximos do personagem.");
        linhas.push_back(" " + Aparencia::cor(Cor::VERMELHO)       + "Forca        " + cB + ": Aumenta o dano base de ataques fisicos.");
        linhas.push_back(" " + Aparencia::cor(Cor::LARANJA)        + "Destreza     " + cB + ": Aumenta o dano fisico final em porcentagem e define a ordem de turno.");
        linhas.push_back(" " + Aparencia::cor(Cor::AZUL)           + "Resistencia  " + cB + ": Reduz o dano recebido de forma fixa.");
        linhas.push_back(" " + Aparencia::cor(Cor::CIANO)          + "Constituicao " + cB + ": Reduz o dano recebido em porcentagem.");
        linhas.push_back(" " + Aparencia::cor(Cor::ROXO)           + "Inteligencia " + cB + ": Aumenta o dano base de ataques magicos.");
        linhas.push_back(" " + Aparencia::cor(Cor::LILAS)          + "Sabedoria    " + cB + ": Aumenta o dano magico final em porcentagem e aumenta a potencia de curas.");
        linhas.push_back("");
        
        linhas.push_back("═══ ATRIBUTOS DE DANO RECOMENDADOS PARA A CLASSE " + jogadorAtual->obterNomeClasse() + " ═══");
        
        TipoClasse tipo = jogadorAtual->obterTipoClasse();
        if (tipo == TipoClasse::Guerreiro) {
            linhas.push_back(" 1. " + Aparencia::cor(Cor::VERMELHO)    + "Forca    " + cB + ": Aumenta o dano base, essencial para armas pesadas.");
            linhas.push_back(" 2. " + Aparencia::cor(Cor::LARANJA)     + "Destreza " + cB + ": Multiplica o dano final, util para qualquer build fisica.");
        } else if (tipo == TipoClasse::Arqueiro) {
            linhas.push_back(" 1. " + Aparencia::cor(Cor::LARANJA)     + "Destreza " + cB + ": Atributo principal, aumenta o dano e define a ordem de turno.");
            linhas.push_back(" 2. " + Aparencia::cor(Cor::VERMELHO)    + "Forca    " + cB + ": Aumenta o dano base, fortalecendo o dano fisico geral.");
        } else if (tipo == TipoClasse::Mago) {
            linhas.push_back(" 1. " + Aparencia::cor(Cor::ROXO)        + "Inteligencia " + cB + ": Essencial, aumenta drasticamente o dano base de magias.");
            linhas.push_back(" 2. " + Aparencia::cor(Cor::LILAS)       + "Sabedoria    " + cB + ": Multiplica o dano magico final e fortalece habilidades.");
        } else if (tipo == TipoClasse::Bardo) {
            linhas.push_back(" 1. " + Aparencia::cor(Cor::LILAS)       + "Sabedoria    " + cB + ": Fortalece intensamente os efeitos das curas e utilidade do Bardo.");
            linhas.push_back(" 2. " + Aparencia::cor(Cor::ROXO)        + "Inteligencia " + cB + ": Melhora o dano magico, permitindo que o Bardo lute efetivamente.");
        } else {
            linhas.push_back(" Nenhum atributo de dano especifico definido para esta classe.");
        }

        Aparencia::imprimirBlocoCentralizado(linhas, Aparencia::cor(Cor::BRANCO), 10);
        ControleDeInput::aguardarEnter();
    }
}

void TelaAtributos::exibir(Personagem* jogadorAtual)
{
    if (jogadorAtual == nullptr) return;
    Aparencia::limparTela();

    static auto ultimoAcessoFicha = std::chrono::steady_clock::now() - std::chrono::hours(1);
    bool animar = TelaBase::deveAnimarEntradaDaTela(ultimoAcessoFicha, 300);

    int largura = Aparencia::obterLarguraTerminal();

    Aparencia::exibirPainelArte(ArtesAtributos::logoFicha, 59, Cor::MAGENTA, "", animar);

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

    std::string difStr = "DIFICULDADE: ";
    switch (jogadorAtual->obterDificuldade()) {
        case DificuldadeJogo::Facil: difStr += Aparencia::cor(Cor::VERDE) + "Facil"; break;
        case DificuldadeJogo::Normal: difStr += Aparencia::cor(Cor::AMARELO) + "Normal"; break;
        case DificuldadeJogo::Dificil: difStr += Aparencia::cor(Cor::VERMELHO) + "Dificil"; break;
    }
    difStr += Aparencia::cor(Cor::RESET) + "   [PARRY]: ";
    difStr += jogadorAtual->obterParryAtivado() ? (Aparencia::cor(Cor::VERDE) + "Ligado" + Aparencia::cor(Cor::RESET)) : (Aparencia::cor(Cor::CINZA) + "Desligado" + Aparencia::cor(Cor::RESET));
    infoGeral.push_back(difStr);

    std::vector<std::string> caixaGeral = Aparencia::criarCaixa(infoGeral, "", 72, Cor::MAGENTA);

    auto formatarAtr = [temBuff, multiplicadorDeAtributosAtual](std::string nomeDoAtributo, int valorBaseDoAtributo, int valorPerdidoPorDebuff, Cor corBase, int atrRaca, int atrClasse) -> std::string
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
        
        ss << "  " << Aparencia::cor(Cor::CINZA) << "[R: " << atrRaca << " | C: " << atrClasse << "]" << Aparencia::cor(Cor::RESET);

        return ss.str();
    };

    Atributos atrRaca = jogadorAtual->obterRaca()->obterAtributosRaca();
    Atributos atrClasse = jogadorAtual->obterClasse()->obterAtributosClasse();

    std::vector<std::string> atrLinhas;
    atrLinhas.push_back(formatarAtr("Forca", jogadorAtual->obterForca(), forcaPerdida, Cor::VERMELHO, atrRaca.forca, atrClasse.forca));
    atrLinhas.push_back(formatarAtr("Destreza", jogadorAtual->obterDestreza(), destrezaPerdida, Cor::LARANJA, atrRaca.destreza, atrClasse.destreza));
    atrLinhas.push_back(formatarAtr("Resistencia", jogadorAtual->obterResistencia(), resPerdida, Cor::AZUL, atrRaca.resistencia, atrClasse.resistencia));
    atrLinhas.push_back(formatarAtr("Constituicao", jogadorAtual->obterConstituicao(), constPerdida, Cor::CIANO, atrRaca.constituicao, atrClasse.constituicao));
    atrLinhas.push_back(formatarAtr("Inteligencia", jogadorAtual->obterInteligencia(), 0, Cor::ROXO, atrRaca.inteligencia, atrClasse.inteligencia));
    atrLinhas.push_back(formatarAtr("Sabedoria", jogadorAtual->obterSabedoria(), 0, Cor::LILAS, atrRaca.sabedoria, atrClasse.sabedoria));

    std::vector<std::string> caixaAtributos = Aparencia::criarCaixa(atrLinhas, "ATRIBUTOS", 35, Cor::MAGENTA);

    std::vector<std::string> habLinhas;
    habLinhas.push_back("[HAB. RACA]: " + jogadorAtual->obterRaca()->obterNomeHabilidadeRaca());
    habLinhas.push_back(" - " + Aparencia::cor(Cor::CINZA) + jogadorAtual->obterRaca()->obterDescricaoHabilidadeRaca() + Aparencia::cor(Cor::RESET));
    habLinhas.push_back("[PASSIVA]  : " + jogadorAtual->obterClasse()->obterNomePassivaClasse());
    habLinhas.push_back(" - " + Aparencia::cor(Cor::CINZA) + jogadorAtual->obterClasse()->obterDescricaoPassivaClasse() + Aparencia::cor(Cor::RESET));
    habLinhas.push_back("[ATIVA]    : " + jogadorAtual->obterClasse()->obterNomeHabilidadeClasse());
    habLinhas.push_back(" - " + Aparencia::cor(Cor::CINZA) + jogadorAtual->obterClasse()->obterDescricaoHabilidadeClasse() + Aparencia::cor(Cor::RESET));
    habLinhas.push_back("");
    habLinhas.push_back("EQUIPAMENTOS:");
    std::string arma = jogadorAtual->obterArma() ? jogadorAtual->obterArma()->obterNomeItem() + jogadorAtual->obterArma()->obterInfoStatus() : "Punhos";
    std::string escudo = jogadorAtual->obterEscudo() ? jogadorAtual->obterEscudo()->obterNomeItem() + jogadorAtual->obterEscudo()->obterInfoStatus() : "Nenhum";
    std::string armadura = jogadorAtual->obterArmadura() ? jogadorAtual->obterArmadura()->obterNomeItem() + jogadorAtual->obterArmadura()->obterInfoStatus() : "Trapos";
    habLinhas.push_back(" > Arma    : " + arma);
    habLinhas.push_back(" > Escudo  : " + escudo);
    habLinhas.push_back(" > Armadura: " + armadura);

    std::vector<std::string> caixaHabilidades = Aparencia::criarCaixa(habLinhas, "HABILIDADES & EQUIPAMENTOS", 35, Cor::MAGENTA);

    // --- PODER DE COMBATE (Derivados) ---
    int danoFis = 1, danoMag = 0;
    if (jogadorAtual->obterArma()) {
        danoFis = jogadorAtual->obterArma()->obterDanoFisico();
        danoMag = jogadorAtual->obterArma()->obterDanoMagico();
    }
    int forca = jogadorAtual->obterForca();
    int destreza = jogadorAtual->obterDestreza();
    int inteli = jogadorAtual->obterInteligencia();
    int sabedoria = jogadorAtual->obterSabedoria();
    
    if (danoFis == 0 && danoMag > 0) { forca /= 10; destreza /= 10; }
    else if (danoFis > 0 && danoMag == 0) { inteli /= 10; sabedoria /= 10; }

    int danoFisEst = std::max(0, static_cast<int>((danoFis + forca) * (1.0 + (destreza / 100.0)) * multiplicadorDeAtributosAtual));
    int danoMagEst = std::max(0, static_cast<int>((danoMag + inteli) * (1.0 + (sabedoria / 100.0)) * multiplicadorDeAtributosAtual));
    int defFixa = jogadorAtual->obterResistencia();
    double mitigacao = std::min(50.0, jogadorAtual->obterConstituicao() / 2.0);

    std::string sFis = Aparencia::cor(Cor::VERMELHO) + std::to_string(danoFisEst) + Aparencia::cor(Cor::RESET);
    std::string sMag = Aparencia::cor(Cor::VERMELHO) + std::to_string(danoMagEst) + Aparencia::cor(Cor::RESET);
    std::string sDef = Aparencia::cor(Cor::AZUL) + std::to_string(defFixa) + Aparencia::cor(Cor::RESET);
    
    std::ostringstream ssMit;
    ssMit << std::fixed << std::setprecision(1) << mitigacao;
    std::string sMit = Aparencia::cor(Cor::CIANO) + ssMit.str() + "%" + Aparencia::cor(Cor::RESET);

    std::vector<std::string> poderLinhas = {
        " Dano Fisico  : " + sFis, " Dano Magico  : " + sMag, " Defesa Fixa  : " + sDef, " Mitigacao (%): " + sMit
    };
    std::vector<std::string> caixaPoder = Aparencia::criarCaixa(poderLinhas, "PODER DE COMBATE", 35, Cor::MAGENTA);

    static const EfeitoInfo efeitosParaExibir[] = {
        {EfeitoID::BuffAtributos,     Cor::VERDE_CLARO,   "Buff Atributos",   true},
        {EfeitoID::Lentidao,           Cor::MAGENTA, "Lentidao",         true},
        {EfeitoID::Sangramento,        Cor::VERMELHO,"Sangramento",      true},
        {EfeitoID::Fraqueza,           Cor::VERMELHO,"Fraqueza",         true},
        {EfeitoID::QuebraResistencia, Cor::CIANO,   "Quebra de Resistencia", false},
        {EfeitoID::RodaAdaptacao,     Cor::AMARELO, "Adaptacao Divina",  false},
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
    std::vector<std::string> caixaStatus = Aparencia::criarCaixa(statusLinhas, "STATUS ATUAIS", 35, Cor::MAGENTA);
    
    Aparencia::imprimirBlocoCentralizado(caixaGeral, "");
    Aparencia::imprimirLadoALado(caixaAtributos, caixaHabilidades, 40, 2, Cor::RESET, Cor::RESET, 0);
    Aparencia::imprimirLadoALado(caixaPoder, caixaStatus, 40, 2, Cor::RESET, Cor::RESET, 0);

    std::cout << "\n";
    Aparencia::imprimirLinhaDivisoria();
}

void TelaAtributos::gerenciarFichaDoJogador(Personagem* jogadorAtual)
{
    if (jogadorAtual == nullptr) return;
    bool continuar = true;
    do 
    {
        TelaAtributos::exibir(jogadorAtual);

        std::vector<std::string> opcoes = { "SUBIR DE NIVEL" };
        opcoes.push_back("DETALHES DE ATRIBUTOS");
        opcoes.push_back("VOLTAR");

        std::cout << "\n";
        int selecao = ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);
        std::string op = opcoes[selecao];

        if (op == "SUBIR DE NIVEL") {
            if (!jogadorAtual->podeSubirDeNivel()) {
                Aparencia::exibirPrompt(Aparencia::cor(Cor::AMARELO) + "[SISTEMA]: Voce nao tem XP suficiente para subir de nivel!" + Aparencia::cor(Cor::RESET));
                ControleDeInput::aguardarEnter();
            } else {
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
                        ControleDeInput::aguardarEnter();
                    }
                }
            }
        } else if (op == "DETALHES DE ATRIBUTOS") {
            exibirDetalhesAtributos(jogadorAtual);
        } else if (op == "VOLTAR") {
            continuar = false;
        }
    } while (continuar);
}






