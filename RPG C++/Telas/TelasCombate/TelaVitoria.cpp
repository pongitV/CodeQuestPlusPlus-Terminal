#include <iostream>
#include <map>

#include "TelaVitoria.h"
#include "../../Racas/RacaBase.h"
#include "../TelaBase.h"
#include "../../Utilidades/Aparencia.h"
#include <sstream>
#include <thread>
#include <chrono>
#include "TelaVitoriaLayouts.h"
#include "../../Utilidades/ControleDeInput.h"
#include "../../Gerenciadores/GerenciadoresCombate/GerenciadorCombate.h"

void TelaVitoria::exibir(SistemaPersonagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate, const std::vector<std::string>& itensObtidos)
{
    Aparencia::limparTela();
    int frames = 20;

    // --- Calculo do Rank de Desempenho ---
    int ptsTurno = (turnosCombate <= 3) ? 40 : (turnosCombate <= 6) ? 30 : (turnosCombate <= 10) ? 20 : 10;
    int pctDano = (jogadorAtual->obterVidaMaxima() > 0) ? (totalDeDanoRecebido * 100 / jogadorAtual->obterVidaMaxima()) : 0;
    int ptsDano = (totalDeDanoRecebido == 0) ? 40 : (pctDano <= 20) ? 30 : (pctDano <= 50) ? 20 : 10;
    
    int ptsParry = 0;
    if (GerenciadorCombate::obterParriesTentados() > 0) {
        int pctParry = (GerenciadorCombate::obterParriesEfetivos() * 100) / GerenciadorCombate::obterParriesTentados();
        ptsParry = (pctParry >= 80) ? 20 : (pctParry >= 50) ? 10 : 0;
    } else if (totalDeDanoRecebido == 0) {
        ptsParry = 20; // Recompensa extra por esquiva/invulnerabilidade total se nenhum parry foi disparado
    }

    int scoreTotal = ptsTurno + ptsDano + ptsParry;
    std::string rankLetter;
    Cor corRank;
    if (scoreTotal >= 90) { rankLetter = "S"; corRank = Cor::AMARELO; }
    else if (scoreTotal >= 75) { rankLetter = "A"; corRank = Cor::VERDE; }
    else if (scoreTotal >= 60) { rankLetter = "B"; corRank = Cor::CIANO; }
    else if (scoreTotal >= 40) { rankLetter = "C"; corRank = Cor::BRANCO; }
    else { rankLetter = "D"; corRank = Cor::VERMELHO; }

    for (int frame = 0; frame <= frames; ++frame) {
        int curOuro = (quantidadeDeOuroObtido * frame) / frames;
        int curXp = (quantidadeDeXpObtido * frame) / frames;

        std::ostringstream buffer;
        std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

        Aparencia::exibirLogoAscii(ArtesVitoria::logoVitoria, 85, Cor::VERDE, "", frame == 0);

        std::vector<std::string> estLinhas;
        estLinhas.push_back("");
        estLinhas.push_back(" Turnos         : " + std::to_string(turnosCombate));
        estLinhas.push_back(" Dano Causado   : " + std::to_string(totalDeDanoCausado));
        estLinhas.push_back(" Maior Hit Dano : " + Aparencia::cor(Cor::VERMELHO) + std::to_string(GerenciadorCombate::obterMaiorDanoCausado()) + Aparencia::cor(Cor::RESET));
        estLinhas.push_back(" Dano Recebido  : " + std::to_string(totalDeDanoRecebido));
        estLinhas.push_back(" Cura Realizada : " + std::to_string(curaTotalRecebida));
        estLinhas.push_back(" Itens Gastos   : " + std::to_string(GerenciadorCombate::obterItensConsumidos()));
        
        std::string strParry = std::to_string(GerenciadorCombate::obterParriesEfetivos()) + "/" + std::to_string(GerenciadorCombate::obterParriesTentados());
        if (GerenciadorCombate::obterParriesTentados() > 0 && GerenciadorCombate::obterParriesEfetivos() == GerenciadorCombate::obterParriesTentados()) {
            strParry = Aparencia::cor(Cor::AMARELO) + strParry + " (Perfeito!)" + Aparencia::cor(Cor::RESET);
        }
        estLinhas.push_back(" Parries        : " + strParry);
        estLinhas.push_back("");
        std::vector<std::string> caixaEst = Aparencia::criarCaixa(estLinhas, "ESTATISTICAS DE DESEMPENHO", 40, Cor::CIANO);

        std::vector<std::string> lootLinhas;
        lootLinhas.push_back(frame == frames ? " Recompensas da Batalha:" : " Calculando recompensas...");
        lootLinhas.push_back("");
        
        double xpPct = static_cast<double>(jogadorAtual->obterXpAtual()) / std::max(1, jogadorAtual->obterXpParaSubir());
        std::string barraXp = Aparencia::gerarBarraSuave(xpPct, 12, Aparencia::cor(Cor::CIANO), Aparencia::cor(Cor::CINZA));
        lootLinhas.push_back(" XP: [" + barraXp + Aparencia::cor(Cor::RESET) + "] " + Aparencia::cor(Cor::CIANO) + "+" + std::to_string(curXp) + Aparencia::cor(Cor::RESET));
        
        int totalAnimadoOuro = jogadorAtual->obterInventario()->obterOuro() - quantidadeDeOuroObtido + curOuro;
        lootLinhas.push_back(" Ouro Total: " + Aparencia::cor(Cor::AMARELO) + std::to_string(totalAnimadoOuro) + "G " + Aparencia::cor(Cor::RESET) + "(+" + std::to_string(curOuro) + ")");
        lootLinhas.push_back("");
        
        lootLinhas.push_back(" Itens Dropados:");
        if (itensObtidos.empty()) {
            lootLinhas.push_back(Aparencia::cor(Cor::CINZA) + " > Nenhum item dropado." + Aparencia::cor(Cor::RESET));
        } else {
            if (frame == frames) {
                std::map<std::string, int> contagem;
                for (const std::string& item : itensObtidos) contagem[item]++;
                for (auto const& [nome, qtd] : contagem) lootLinhas.push_back(" > " + std::to_string(qtd) + "x " + nome);
            } else {
                lootLinhas.push_back(Aparencia::cor(Cor::CINZA) + " > ???" + Aparencia::cor(Cor::RESET));
            }
        }
        std::vector<std::string> caixaLoot = Aparencia::criarCaixa(lootLinhas, "PROGRESSAO & SAQUE", 40, Cor::AMARELO);

        Aparencia::imprimirLadoALado(caixaEst, caixaLoot, 43, 4);

        std::cout << "\n";
        std::vector<std::string> linhasDeRodape;
        
        std::string rankStr = Aparencia::cor(corRank);
        if (frame == frames && corRank == Cor::AMARELO) rankStr += "\033[5m"; // Efeito de piscar para Rank S
        rankStr += "RANK DE AVALIACAO: " + rankLetter + Aparencia::cor(Cor::RESET);
        linhasDeRodape.push_back(rankStr);
        
        if (frame == frames) {
            if (!GerenciadorCombate::obterNovasDescobertas().empty()) {
                linhasDeRodape.push_back("");
                for (const auto& desc : GerenciadorCombate::obterNovasDescobertas()) {
                    linhasDeRodape.push_back("\033[5m" + Aparencia::cor(Cor::CIANO) + "[!] " + desc + Aparencia::cor(Cor::RESET));
                }
            }
            
            if (jogadorAtual->podeSubirDeNivel()) {
                linhasDeRodape.push_back("");
                linhasDeRodape.push_back(Aparencia::cor(Cor::VERDE) + "*** VOCE PODE SUBIR DE NIVEL! ***" + Aparencia::cor(Cor::RESET));
            }
        }
        
        Aparencia::imprimirCentralizadoMultilinha(linhasDeRodape);

        std::cout.rdbuf(oldCout);
        std::cout << "\033[H" << buffer.str() << "\033[J" << std::flush;
        
        if (frame < frames) {
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
        }
    }

    ControleDeInput::aguardarEnter();
}