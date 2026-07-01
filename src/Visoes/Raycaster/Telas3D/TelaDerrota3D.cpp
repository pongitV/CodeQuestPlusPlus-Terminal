#include "TelaDerrota3D.h"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

#include "../../../Entidades/Racas/RacaBase.h"
#include "../../TelasBase/TelaBase.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../TelasBase/Derrota/TelaDerrotaLayout.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../TelasBase/Combate/TelaCombate.h"
#include "../../../Sistemas/Combate/Combate3DRenderer.h"
#include "../../TelasBase/TelaCena3D.h"
#include <sstream>

class CenaDerrota3D : public TelaCena3D {
private:
    Personagem* jogadorAtual;
    int turnosCombate;
    int totalDeDanoCausado;
    std::vector<std::string> tela3D;
    std::vector<std::string> lapide;
    int lapideX;

public:
    CenaDerrota3D(Personagem* jogador, int turnos, int dano) 
        : jogadorAtual(jogador), turnosCombate(turnos), totalDeDanoCausado(dano) {
        
        int larguraTerminal = Aparencia::obterLarguraTerminal();
        std::vector<Personagem*> vazio;
        tela3D = Combate3DRenderer::renderizarQuadro("Cemiterio", jogadorAtual, vazio);

        auto formatLapideLine = [](const std::string& text) {
            std::string bgCinza = "\033[48;2;100;100;100m";
            std::string reset = "\033[0m";
            std::string corBorda = bgCinza + "\033[38;2;0;0;0m";
            std::string fgText = "\033[38;2;0;0;0m";
            
            std::string cleanText = Aparencia::removerCoresANSI(text);
            int len = Aparencia::obterComprimentoVisual(cleanText);
            int padLeft = (50 - len) / 2;
            if (padLeft < 0) padLeft = 0;
            int padRight = 50 - len - padLeft;
            if (padRight < 0) padRight = 0;
            
            return std::string(5, ' ') + corBorda + "│" + bgCinza + std::string(padLeft, ' ') + fgText + cleanText + bgCinza + std::string(padRight, ' ') + corBorda + "│" + reset + std::string(5, ' ');
        };

        std::string bgCinza = "\033[48;2;100;100;100m";
        std::string reset = "\033[0m";
        std::string corBorda = bgCinza + "\033[38;2;0;0;0m";
        std::string fgText = "\033[38;2;0;0;0m";

        lapide.push_back(std::string(9, ' ') + corBorda + ".──────────────────────────────────────────." + reset + std::string(9, ' '));
        lapide.push_back(std::string(7, ' ') + corBorda + "/" + bgCinza + std::string(46, ' ') + corBorda + "\\" + reset + std::string(7, ' '));
        lapide.push_back(std::string(6, ' ') + corBorda + "/" + bgCinza + std::string(20, ' ') + fgText + "R. I. P." + bgCinza + std::string(20, ' ') + corBorda + "\\" + reset + std::string(6, ' '));
        lapide.push_back(formatLapideLine(""));
        lapide.push_back(formatLapideLine(jogadorAtual->obterNome()));
        lapide.push_back(formatLapideLine("pereceu em batalha,"));
        lapide.push_back(formatLapideLine("lutando ate seu ultimo suspiro..."));
        lapide.push_back(formatLapideLine(""));
        lapide.push_back(formatLapideLine("Sobreviveu por " + std::to_string(turnosCombate) + " turnos"));
        lapide.push_back(formatLapideLine("Dano causado: " + std::to_string(totalDeDanoCausado)));
        lapide.push_back(formatLapideLine(""));
        lapide.push_back(formatLapideLine("Mas nao foi o suficiente..."));
        lapide.push_back(formatLapideLine(""));
        lapide.push_back(std::string(3, ' ') + corBorda + "──┴──────────────────────────────────────────────────┴──" + reset + std::string(3, ' '));

        int lapideWidth = 62;
        lapideX = (larguraTerminal - lapideWidth) / 2;
        if (lapideX < 0) lapideX = 0;
    }

protected:
    std::vector<std::string> obterLogo() const override {
        return ArtesDerrota::logoDerrota;
    }

    std::string obterCorLogo() const override {
        return "\033[1;31m";
    }

    std::vector<std::string> obterFundoInicial() const override {
        return tela3D;
    }

    std::vector<std::string> comporFundoComElementos(float opacity) const override {
        std::vector<std::string> frameLinhas = tela3D;
        std::vector<std::string> lapideFaded;
        for (const auto& l : lapide) {
            lapideFaded.push_back(Aparencia::fadarLinhaAnsi(l, opacity));
        }
        
        sobreporPainel(frameLinhas, lapideFaded, 22, lapideX);
        return frameLinhas;
    }
};

void TelaDerrota3D::exibir(Personagem* jogadorAtual, int, int, int totalDeDanoCausado, int, int, int turnosCombate)
{
    Aparencia::limparTela();
    
    CenaDerrota3D cena(jogadorAtual, turnosCombate, totalDeDanoCausado);
    cena.executar();
    
    std::string enterMsg = "Pressione [ENTER] para continuar...";
    std::cout << "\n" << Aparencia::espacosParaCentralizar(Aparencia::obterComprimentoVisual(enterMsg)) << Aparencia::cor(Cor::CINZA) << enterMsg << "\033[0m\n\033[J" << std::flush;
    
    ControleDeInput::aguardarEnter();
}
