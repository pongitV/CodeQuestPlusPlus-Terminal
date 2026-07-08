#include "TelaVitoriaIDE.h"
#include <iostream>
#include <map>
#include <sstream>
#include <thread>
#include <chrono>
#include "../../TemaIDE.h"
#include "../../../../Core/Utilidades/Aparencia.h"
#include "../../../../Core/Utilidades/ControleDeInput.h"
#include "../../../../Sistemas/Combate/Combate.h"
#include "../../../../Entidades/Racas/RacaBase.h"
#include "../../../TelasBase/TelaBase.h"
#include "../../../TelasBase/Vitoria/TelaVitoriaLayout.h"
#include "../../../TelasBase/TelaCena3D.h"
#include "../../../PerspectivaRaycaster/EngineRaycaster/RaycasterRenderizadorCombate.h"
#include "../../../GerenciadorPerspectiva.h"

class CenaVitoriaIntro3D : public TelaCena3D {
private:
    Personagem* jogadorAtual;
    int quantidadeDeOuroObtido;
    int totalDeDanoCausado;
    int totalDeDanoRecebido;
    int curaTotalRecebida;
    int turnosCombate;
    std::vector<std::string> tela3D;
    std::string tituloMapa;

public:
    CenaVitoriaIntro3D(Personagem* jogador, int ouro, int danoCausado, int danoRecebido, int cura, int turnos, const std::string& mapa)
        : jogadorAtual(jogador), quantidadeDeOuroObtido(ouro), totalDeDanoCausado(danoCausado),
          totalDeDanoRecebido(danoRecebido), curaTotalRecebida(cura), turnosCombate(turnos), tituloMapa(mapa) {
        std::vector<Personagem*> vazio;
        tela3D = RaycasterRenderizadorCombate::renderizarQuadro(tituloMapa, jogadorAtual, vazio);
    }

protected:
    std::vector<std::string> obterLogo() const override {
        return ArtesVitoria::logoVitoria;
    }

    std::string obterCorLogo() const override {
        return "\033[1;32m";
    }

    std::vector<std::string> obterFundoInicial() const override {
        return tela3D;
    }

    std::vector<std::string> comporFundoComElementos(float opacity) const override {
        std::vector<std::string> frameLinhas = tela3D;
        int larguraTerminal = Aparencia::obterLarguraTerminal();

        std::vector<std::string> estLinhas;
        estLinhas.push_back("");
        estLinhas.push_back(" Turnos         : " + std::to_string(turnosCombate));
        estLinhas.push_back(" Dano Causado   : " + std::to_string(totalDeDanoCausado));
        estLinhas.push_back(" Maior Hit Dano : " + Aparencia::cor(Cor::VERMELHO) + std::to_string(Combate::obterMaiorDanoCausado()) + Aparencia::cor(Cor::RESET));
        estLinhas.push_back(" Dano Recebido  : " + std::to_string(totalDeDanoRecebido));
        estLinhas.push_back(" Cura Realizada : " + std::to_string(curaTotalRecebida));
        estLinhas.push_back(" Itens Gastos   : " + std::to_string(Combate::obterItensConsumidos()));

        std::string strParry = std::to_string(Combate::obterParriesEfetivos()) + "/" + std::to_string(Combate::obterParriesTentados());
        if (Combate::obterParriesTentados() > 0 && Combate::obterParriesEfetivos() == Combate::obterParriesTentados()) {
            strParry = Aparencia::cor(Cor::AMARELO) + strParry + " (Perfeito!)" + Aparencia::cor(Cor::RESET);
        }
        estLinhas.push_back(" Parries        : " + strParry);
        estLinhas.push_back("");
        std::vector<std::string> caixaEst = TelaBase::criarCaixa(estLinhas, "ESTATISTICAS DE DESEMPENHO", 40, Cor::CIANO);

        std::vector<std::string> progressoLinhas;
        progressoLinhas.push_back(" Evolucao do Personagem:");
        progressoLinhas.push_back("");

        double xpPct = static_cast<double>(jogadorAtual->obterXpAtual()) / std::max(1, jogadorAtual->obterXpParaSubir());
        std::string barraXp = TelaBase::gerarBarraGradiente(xpPct, 12, Cor::CIANO);
        progressoLinhas.push_back(" XP: [" + barraXp + Aparencia::cor(Cor::RESET) + "] " + Aparencia::cor(Cor::CIANO) + "+0" + Aparencia::cor(Cor::RESET));

        int totalAnimadoOuro = jogadorAtual->obterInventario()->obterOuro() - quantidadeDeOuroObtido;
        progressoLinhas.push_back(" Ouro Total: " + Aparencia::cor(Cor::AMARELO) + std::to_string(totalAnimadoOuro) + "G " + Aparencia::cor(Cor::RESET) + "(+0)");
        progressoLinhas.push_back("");

        std::vector<std::string> caixaProgresso = TelaBase::criarCaixa(progressoLinhas, "PROGRESSAO DE BATALHA", 40, Cor::AMARELO);

        std::vector<std::string> dropsLinhas;
        dropsLinhas.push_back(Aparencia::cor(Cor::CINZA) + " ???" + Aparencia::cor(Cor::RESET));
        std::vector<std::string> caixaDrops = TelaBase::criarCaixa(dropsLinhas, "SAQUE & DESCOBERTAS", 84, Cor::MAGENTA);

        int estWidth = 40;
        int progWidth = 40;
        int gap = 4;
        int totalTopWidth = estWidth + gap + progWidth;

        int startEstX = (larguraTerminal - totalTopWidth) / 2;
        if (startEstX < 0) startEstX = 0;
        int startProgX = startEstX + estWidth + gap;
        int startDropsX = (larguraTerminal - 84) / 2;
        if (startDropsX < 0) startDropsX = 0;

        std::vector<std::string> fadeEst, fadeProg, fadeDrops;
        for (const auto& l : caixaEst) fadeEst.push_back(Aparencia::fadarLinhaAnsi(l, opacity));
        for (const auto& l : caixaProgresso) fadeProg.push_back(Aparencia::fadarLinhaAnsi(l, opacity));
        for (const auto& l : caixaDrops) fadeDrops.push_back(Aparencia::fadarLinhaAnsi(l, opacity));

        sobreporPainel(frameLinhas, fadeEst, 16, startEstX);
        sobreporPainel(frameLinhas, fadeProg, 16, startProgX);

        int yDrops = 16 + caixaProgresso.size() + 1;
        sobreporPainel(frameLinhas, fadeDrops, yDrops, startDropsX);

        return frameLinhas;
    }
};

void TelaVitoriaIDE::exibir(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido,
    int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate,
    const std::vector<std::string>& inimigosDerrotados, int parriesPerfeitos, int maiorDano,
    const std::vector<std::pair<std::string, int>>& dropsUnicos,
    bool podeSubirNivel, const std::vector<std::string>& novasDescobertas,
    const std::string& tituloMapa)
{
    bool isModo3D = GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva();

    if (!isModo3D) {
        Aparencia::limparTela();
    }

    int framesXP = 20;
    int framesTotal = framesXP + (dropsUnicos.empty() ? 0 : static_cast<int>(dropsUnicos.size())) + 1;

    if (isModo3D) {
        CenaVitoriaIntro3D intro(jogadorAtual, quantidadeDeOuroObtido, totalDeDanoCausado, totalDeDanoRecebido, curaTotalRecebida, turnosCombate, tituloMapa);
        intro.executar();
    }

    for (int frame = 0; frame <= framesTotal; ++frame) {
        int curOuro = (quantidadeDeOuroObtido * std::min(frame, framesXP)) / framesXP;
        int curXp = (quantidadeDeXpObtido * std::min(frame, framesXP)) / framesXP;

        std::ostringstream buffer;
        std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

        std::vector<Personagem*> vazio;
        std::vector<std::string> tela3D;

        if (isModo3D) {
            tela3D = RaycasterRenderizadorCombate::renderizarQuadro(tituloMapa, jogadorAtual, vazio);
        } else {
            Aparencia::exibirPainelArte(ArtesVitoria::logoVitoria, 85, Cor::VERDE, "", frame == 0);
        }

        std::vector<std::string> estLinhas;
        estLinhas.push_back("");
        estLinhas.push_back(" Turnos         : " + std::to_string(turnosCombate));
        estLinhas.push_back(" Dano Causado   : " + std::to_string(totalDeDanoCausado));
        estLinhas.push_back(" Maior Hit Dano : " + Aparencia::cor(Cor::VERMELHO) + std::to_string(Combate::obterMaiorDanoCausado()) + Aparencia::cor(Cor::RESET));
        estLinhas.push_back(" Dano Recebido  : " + std::to_string(totalDeDanoRecebido));
        estLinhas.push_back(" Cura Realizada : " + std::to_string(curaTotalRecebida));
        estLinhas.push_back(" Itens Gastos   : " + std::to_string(Combate::obterItensConsumidos()));

        std::string strParry = std::to_string(Combate::obterParriesEfetivos()) + "/" + std::to_string(Combate::obterParriesTentados());
        if (Combate::obterParriesTentados() > 0 && Combate::obterParriesEfetivos() == Combate::obterParriesTentados()) {
            strParry = Aparencia::cor(Cor::AMARELO) + strParry + " (Perfeito!)" + Aparencia::cor(Cor::RESET);
        }
        estLinhas.push_back(" Parries        : " + strParry);
        estLinhas.push_back("");
        std::vector<std::string> caixaEst = TelaBase::criarCaixa(estLinhas, "ESTATISTICAS DE DESEMPENHO", 40, Cor::CIANO);

        std::vector<std::string> progressoLinhas;
        progressoLinhas.push_back(" Evolucao do Personagem:");
        progressoLinhas.push_back("");

        double xpPct = static_cast<double>(jogadorAtual->obterXpAtual()) / std::max(1, jogadorAtual->obterXpParaSubir());
        std::string barraXp = TelaBase::gerarBarraGradiente(xpPct, 12, Cor::CIANO);
        progressoLinhas.push_back(" XP: [" + barraXp + Aparencia::cor(Cor::RESET) + "] " + Aparencia::cor(Cor::CIANO) + "+" + std::to_string(curXp) + Aparencia::cor(Cor::RESET));

        int totalAnimadoOuro = jogadorAtual->obterInventario()->obterOuro() - quantidadeDeOuroObtido + curOuro;
        progressoLinhas.push_back(" Ouro Total: " + Aparencia::cor(Cor::AMARELO) + std::to_string(totalAnimadoOuro) + "G " + Aparencia::cor(Cor::RESET) + "(+" + std::to_string(curOuro) + ")");
        progressoLinhas.push_back("");

        std::vector<std::string> caixaProgresso = TelaBase::criarCaixa(progressoLinhas, "PROGRESSAO DE BATALHA", 40, Cor::AMARELO);

        std::vector<std::string> dropsLinhas;
        if (dropsUnicos.empty()) {
            if (frame >= framesXP) {
                dropsLinhas.push_back(Aparencia::cor(Cor::CINZA) + " Nenhum item dropado." + Aparencia::cor(Cor::RESET));
            } else {
                dropsLinhas.push_back(Aparencia::cor(Cor::CINZA) + " ???" + Aparencia::cor(Cor::RESET));
            }
        } else {
            if (frame <= framesXP) {
                dropsLinhas.push_back(Aparencia::cor(Cor::CINZA) + " ???" + Aparencia::cor(Cor::RESET));
            } else {
                int itensMostrar = std::min((int)dropsUnicos.size(), frame - framesXP);
                for (int i = 0; i < itensMostrar; ++i) {
                    dropsLinhas.push_back(" " + std::to_string(dropsUnicos[i].second) + "x " + dropsUnicos[i].first);
                }
            }
        }

        if (frame == framesTotal) {
            if (!novasDescobertas.empty()) {
                dropsLinhas.push_back("");
                for (const auto& desc : novasDescobertas) {
                    dropsLinhas.push_back(Aparencia::cor(Cor::CIANO) + " [!] " + desc + Aparencia::cor(Cor::RESET));
                }
            }
            if (podeSubirNivel) {
                dropsLinhas.push_back("");
                dropsLinhas.push_back(Aparencia::cor(Cor::VERDE) + " *** VOCE PODE SUBIR DE NIVEL! ***" + Aparencia::cor(Cor::RESET));
            }

            dropsLinhas.push_back("");
            std::string enterMsg = "\033[5m[ PRESSIONE ENTER PARA CONTINUAR ]\033[0m";
            int paddingSize = (80 - 34) / 2;
            std::string padding(paddingSize > 0 ? paddingSize : 0, ' ');
            dropsLinhas.push_back(padding + Aparencia::cor(Cor::VERDE) + enterMsg + Aparencia::cor(Cor::RESET));
        }

        std::vector<std::string> caixaDrops = TelaBase::criarCaixa(dropsLinhas, "SAQUE & DESCOBERTAS", 84, Cor::MAGENTA);

        if (isModo3D) {
            auto overlayPanel = [](std::vector<std::string>& fundo, const std::vector<std::string>& arte, int startY, int startX) {
                for (size_t i = 0; i < arte.size(); ++i) {
                    int y = startY + i;
                    if (y >= 0 && y < static_cast<int>(fundo.size())) {
                        fundo[y] = Aparencia::sobreporPainelNaLinhaAnsi(fundo[y], arte[i], startX);
                    }
                }
            };

            std::vector<std::string> logoColorida = ArtesVitoria::logoVitoria;
            for (auto& l : logoColorida) l = Aparencia::cor(Cor::VERDE) + l + Aparencia::cor(Cor::RESET);

            int larguraTerminal = Aparencia::obterLarguraTerminal();

            int logoWidth = 0;
            for(const auto& l : logoColorida) {
                logoWidth = std::max(logoWidth, Aparencia::obterComprimentoVisual(l));
            }
            int logoX = (larguraTerminal - logoWidth) / 2;
            if (logoX < 0) logoX = 0;

            int estWidth = 40;
            int progWidth = 40;
            int gap = 4;
            int totalTopWidth = estWidth + gap + progWidth;

            int startEstX = (larguraTerminal - totalTopWidth) / 2;
            if (startEstX < 0) startEstX = 0;
            int startProgX = startEstX + estWidth + gap;

            int startDropsX = (larguraTerminal - 84) / 2;
            if (startDropsX < 0) startDropsX = 0;

            overlayPanel(tela3D, logoColorida, 2, logoX);
            overlayPanel(tela3D, caixaEst, 16, startEstX);
            overlayPanel(tela3D, caixaProgresso, 16, startProgX);

            int yDrops = 16 + caixaProgresso.size() + 1;
            overlayPanel(tela3D, caixaDrops, yDrops, startDropsX);
        } else {
            Aparencia::imprimirLadoALado(caixaEst, caixaProgresso, 43, 4);
            std::cout << "\n";
            Aparencia::imprimirCentralizadoMultilinha(caixaDrops);
        }

        if (!isModo3D) {
            std::cout << "\n";
        }

        if (isModo3D) {
            std::string renderStr = "";
            for (size_t i = 0; i < tela3D.size(); ++i) {
                renderStr += tela3D[i];
                if (i < tela3D.size() - 1) {
                    renderStr += "\n";
                }
            }
            std::cout << "\033[H" << renderStr;
        }

        std::cout.rdbuf(oldCout);
        std::cout << "\033[H" << buffer.str();
        if (!isModo3D) std::cout << "\033[J";
        std::cout << std::flush;

        if (frame < framesTotal) {
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
        }
    }

    if (isModo3D) {
        std::cout << "\033[33;1H";
    }
    ControleDeInput::aguardarEnter("");
}
