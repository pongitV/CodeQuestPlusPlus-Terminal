#include "AnimadorMapa.h"
#include "../../Core/Utilidades/Aparencia.h"
#include "../../Core/Utilidades/ControleDeInput.h"
#include "RenderizadorMapa.h"
#include "../ControleMapa.h"
#include "../../Perspectiva/PerspectivaRaycaster/EngineRaycaster/RaycasterMundo.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>

int AnimadorMapa::animarIntroducaoMapa(
    const std::string& tituloDoMapa,
    const std::vector<std::string>& arteDoMapa,
    int /*larguraArte*/,
    const std::vector<std::string>& arteTransicao,
    int /*larguraTransicao*/,
    Cor /*corTema*/,
    const std::vector<std::string>& matrizDoMapa,
    int posicaoXDoJogador,
    int posicaoYDoJogador,
    const std::function<std::string(char, int, int)>& formatadorCelula,
    bool animar,
    bool usarAnimacaoBanner,
    const std::function<void()>& acaoAposFadeInArte
) {
    if (ControleMapa::isExploracao3DAtiva()) {
        RaycasterMundo::atualizarMapHash(matrizDoMapa);
        return 0; 
    }

    RaycasterMundo::atualizarMapHash(matrizDoMapa);
    Aparencia::ocultarCursor();

    int larguraTerminal = Aparencia::obterLarguraTerminal();
    int alturaTerminal = Aparencia::obterAlturaTerminal();

    if (!animar) {
        Aparencia::exibirPainelTexto(tituloDoMapa, Cor::BRANCO);
        int linhaInicialMapa = Aparencia::obterPosicaoCursorY();
        RenderizadorMapa::renderizarMapa(matrizDoMapa, posicaoXDoJogador, posicaoYDoJogador, larguraTerminal, alturaTerminal, linhaInicialMapa, formatadorCelula);
        return linhaInicialMapa;
    }

    Aparencia::limparTela();

    if (acaoAposFadeInArte) {
        acaoAposFadeInArte();
        Aparencia::limparTela();
    }
    
    Aparencia::exibirPainelTexto(tituloDoMapa, Cor::BRANCO, true);
    int linhaInicialMapa = Aparencia::obterPosicaoCursorY();

    std::vector<std::string> bannerBase;
    if (usarAnimacaoBanner) {
        if (!arteDoMapa.empty()) {
            bannerBase = arteDoMapa;
        } else if (!arteTransicao.empty()) {
            bannerBase = arteTransicao;
        }
    }

    if (bannerBase.empty() || !usarAnimacaoBanner) {
        RenderizadorMapa::renderizarMapa(matrizDoMapa, posicaoXDoJogador, posicaoYDoJogador, larguraTerminal, alturaTerminal, linhaInicialMapa, formatadorCelula);
        return linhaInicialMapa;
    }

    std::vector<std::string> banner;
    for (const auto& l : bannerBase) {
        banner.push_back("\033[1;37m" + Aparencia::removerCoresANSI(l) + "\033[0m");
    }
    
    int maxW = 0;
    for (const auto& l : banner) {
        int w = Aparencia::obterComprimentoVisual(l);
        if (w > maxW) maxW = w;
    }
    
    int bannerHeight = banner.size();
    int startXBox = (larguraTerminal - maxW) / 2;
    if (startXBox < 0) startXBox = 0;

    int startX, endX;
    RenderizadorMapa::calcularCameraHorizontal(larguraTerminal, posicaoXDoJogador, matrizDoMapa.empty() ? 0 : static_cast<int>(matrizDoMapa[0].length()), startX, endX);
    std::string margemEsquerdaDoMapa = RenderizadorMapa::calcularMargemCentralizada(larguraTerminal, endX - startX);
    
    std::string textoDeControles = "W,A,S,D: Mover | V: Visao | I: Inventario | C: Ficha | B: Diario | M: Mapa";
    std::string margemEsquerdaControles = RenderizadorMapa::calcularMargemCentralizada(larguraTerminal, textoDeControles.length());
    
    int offsetMapaReal = 2;
    
    int startY, endY;
    RenderizadorMapa::calcularCameraVertical(alturaTerminal, linhaInicialMapa, posicaoYDoJogador, static_cast<int>(matrizDoMapa.size()), startY, endY);
    
    std::vector<std::string> linhasDoMapaCache;
    for (int y = startY; y < endY; y++) {
        std::string linhaStr = margemEsquerdaDoMapa;
        linhaStr.reserve(margemEsquerdaDoMapa.size() + (endX - startX) * 10);
        for (int x = startX; x < endX; x++) {
            char c = (x < static_cast<int>(matrizDoMapa[y].length())) ? matrizDoMapa[y][x] : ' ';
            linhaStr += formatadorCelula(c, x, y);
        }
        linhasDoMapaCache.push_back(linhaStr);
    }

    std::ostringstream initialMap;
    initialMap << "\033[" << (linhaInicialMapa + 1) << ";1H\033[K" << margemEsquerdaControles << Aparencia::cor(Cor::CINZA) << textoDeControles << Aparencia::cor(Cor::RESET) << "\n\033[K\n";
    for (int i = 0; i < (int)linhasDoMapaCache.size(); i++) {
        initialMap << "\033[" << (linhaInicialMapa + 1 + offsetMapaReal + i) << ";1H" << linhasDoMapaCache[i] << "\033[K";
    }
    std::cout << initialMap.str() << std::flush;

    int destinoY = 2;
    
    Aparencia::animarFadeIn(15, 40, [&](int frame, int /*intensidade*/) {
        float opacity = frame / 15.0f;
        int c = (int)(255 * opacity);
        std::string corFadedBanner = "\033[38;2;" + std::to_string(c) + ";" + std::to_string(c) + ";" + std::to_string(c) + "m";
        
        std::ostringstream telaFrame;
        
        for (int i = 0; i < bannerHeight; i++) {
            int drawY = linhaInicialMapa + destinoY + i;
            if (drawY >= linhaInicialMapa && drawY < alturaTerminal) {
                std::string linhaLimpa = Aparencia::removerCoresANSI(bannerBase[i]);
                telaFrame << "\033[" << (drawY + 1) << ";" << (startXBox + 1) << "H" << corFadedBanner << linhaLimpa << "\033[0m";
            }
        }
        std::cout << telaFrame.str() << std::flush;
    });

    if (!ControleDeInput::teclaPressionada()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    ControleDeInput::limparBuffer();
    
    RenderizadorMapa::renderizarMapa(matrizDoMapa, posicaoXDoJogador, posicaoYDoJogador, larguraTerminal, alturaTerminal, linhaInicialMapa, formatadorCelula);
    
    return linhaInicialMapa;
}

void AnimadorMapa::animarFlashbang(int r, int g, int b) {
    int LARGURA_TELA = Aparencia::obterLarguraTerminal();
    int ALTURA_TELA = Aparencia::obterAlturaTerminal();
    if (LARGURA_TELA <= 0) LARGURA_TELA = 120;
    if (ALTURA_TELA <= 0) ALTURA_TELA = 30;

    std::cout << "\033[?25l"; // Hide cursor
    std::string colorPrefix = "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
    
    std::vector<std::string> fadeChars = {"█", "▓", "▒", "░", " "};
    
    for (int passo = 0; passo < (int)fadeChars.size(); passo++) {
        std::string buffer = "\033[H";
        buffer.reserve(LARGURA_TELA * ALTURA_TELA * 20);
        
        for (int y = 0; y < ALTURA_TELA; y++) {
            for (int x = 0; x < LARGURA_TELA; x++) {
                if (y == ALTURA_TELA - 1 && x == LARGURA_TELA - 1) break; // prevent scroll
                if (fadeChars[passo] == " ") {
                    buffer += "\033[40m \033[0m"; // Preto final
                } else {
                    buffer += colorPrefix + "\033[38;2;255;255;255m" + fadeChars[passo] + "\033[0m";
                }
            }
            if (y < ALTURA_TELA - 1) buffer += "\n";
        }
        std::cout << buffer << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
    Aparencia::limparTela();
}
