#include "Mapa3Reino.h"

#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <unordered_map>

#include "../Gerenciadores/GerenciadorMenu.h"
#include "../Telas/TelaMenu.h"
#include "../Utilidades/Aparencia.h"
#include "ControleMapa.h"
#include "../Utilidades/ControleDeInput.h"
#include "../Gerenciadores/GerenciadorInimigos.h"
#include "Mapa3ReinoLayouts.h"
#include "../NPCs/NPCCavaleiroGenerico.h"


Mapa3Reino::Mapa3Reino(SistemaPersonagem* personagemJogador) :
    posicaoXDoJogador(48), 
    posicaoYDoJogador(33),
    jogadorAtual(personagemJogador), 
    exploracaoEstaAtiva(true), 
    tituloDoMapaAtual("CAMINHO DO CASTELO")
{
    matrizDoMapaAtual = Mapa3ReinoLayouts::obterLayoutReino();
}

Mapa3Reino::~Mapa3Reino() = default;

namespace {
    std::vector<std::string> obterArteTransicaoReino() {
        return {
            "                  |>>>                        |>>>",
            "                    |                           |",
            "                _  _|_  _                   _  _|_  _",
            "               | |_| |_| |                 | |_| |_| |",
            "               \\  .      /                 \\ .    .  /",
            "                \\    ,  /                   \\    .  /",
            "                 | .   |_   _   _   _   _   _| ,   |",
            "                 |    .| |_| |_| |_| |_| |_| |  .  |",
            "                 | ,   | .    .     .      . |    .|",
            "                 |   . |  .     . .   .  ,   |.    |",
            "     ___----_____| .   |.   ,  _______   .   |   , |---~_____",
            "_---~            |     |  .   /+++++++\\    . | .   |         ~---_",
            "                 |.    | .    |+++++++| .    |   . |              ~-_",
            "              __ |   . |   ,  |+++++++|.  . _|__   |                 ~-_",
            "     ____--`~    '--~~__ .    |++++ __|----~    ~`---,              ___^~-__",
            "-~--~                   ~---__|,--~'                  ~~----_____-~'   `~----~"
        };
    }
}

void Mapa3Reino::iniciarLoopDeExploracaoDoMapa()
{
    bool trollDerrotado = false;
    bool conviteRecebido = false;

    ControleMapa::padronizarTamanhoDoMapa(matrizDoMapaAtual);

    Aparencia::ocultarCursor();

    auto formatador = [&](char celula, int x, int y) -> std::string {
        if (x == posicaoXDoJogador && y == posicaoYDoJogador) return Aparencia::cor(Cor::NEGRITO, jogadorAtual->obterCorJogador()) + std::string(1, jogadorAtual->obterIconeJogador()) + Aparencia::cor(Cor::RESET);
        if (celula == 'T') return Aparencia::cor(Cor::NEGRITO, Cor::VERMELHO) + "T" + Aparencia::cor(Cor::RESET);
        if (celula == 'G') return Aparencia::cor(Cor::NEGRITO, Cor::AMARELO) + "G" + Aparencia::cor(Cor::RESET);
        if (celula == '=' || celula == '|') return Aparencia::cor(Cor::CINZA) + std::string(1, celula) + Aparencia::cor(Cor::RESET);
        return std::string(1, celula);
    };

    int linhaInicialParaDesenharOMapa = 0;

    auto restaurarTela = [&]() {
        linhaInicialParaDesenharOMapa = ControleMapa::animarIntroducaoMapa(tituloDoMapaAtual, {}, 0, {}, 0, Cor::CIANO, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, formatador, false);
    };

    auto animarTela = [&]() {
        std::vector<std::string> arteTitulo;
        int larguraArte = 0;
        std::vector<std::string> arteTrans;
        int larguraTrans = 0;

        if (tituloDoMapaAtual == "CAMINHO DO CASTELO") {
            arteTitulo = Mapa3ReinoLayouts::obterLogoReino();
            larguraArte = 60;
            arteTrans = obterArteTransicaoReino();
            larguraTrans = 75;
        }

        linhaInicialParaDesenharOMapa = ControleMapa::animarIntroducaoMapa(tituloDoMapaAtual, arteTitulo, larguraArte, arteTrans, larguraTrans, Cor::CIANO, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, formatador, true);
    };

    animarTela();

    std::unordered_map<char, std::function<void(int, int, int)>> interacoes;

    interacoes['^'] = [&](int px, int py, int larg) {
        char nextCell = matrizDoMapaAtual[py][px+1];
        if (nextCell == 'C') {
            if (!conviteRecebido) {
                Aparencia::limparTela();
                Aparencia::exibirPainelTexto("ACESSO NEGADO", Cor::CIANO);
                int espacosM = (larg - 60) / 2;
                std::cout << "\n" << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[SISTEMA]: Os portoes estao trancados. Voce precisa de uma permissao real.\n";
                ControleDeInput::aguardarEnter();
                restaurarTela();
            } else {
                Aparencia::limparTela();
                Aparencia::exibirPainelTexto("FIM DA DEMO", Cor::CIANO);
                int espacosM = (larg - 60) / 2;
                std::cout << "\n" << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[SISTEMA]: Voce apresentou o Convite Real e os portoes se abriram!\n";
                std::cout << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[SISTEMA]: A historia continua em breve...\n";
                ControleDeInput::aguardarEnter();
                exploracaoEstaAtiva = false;
            }
        }
        else if (nextCell == 'F') {
            exploracaoEstaAtiva = false;
        }
    };

    interacoes['G'] = [&](int px, int py, int larg) {
        Aparencia::limparTela();
        Aparencia::exibirPainelTexto("GUARDA REAL", Cor::CIANO);
        int espacosM = (larg - 60) / 2;
        std::cout << "\n" << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[Guarda]: Alto la! Somente o Rei pode conceder passagem.\n";
        std::cout << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[Guarda]: (O castelo ainda esta em construcao pelos deuses/devs)\n";
        ControleDeInput::aguardarEnter();
        restaurarTela();
    };

    auto interagirCavaleiro = [&](int px, int py, int larg) {
        NPCCavaleiroGenerico::interagir(jogadorAtual, trollDerrotado, conviteRecebido, larg, matrizDoMapaAtual, exploracaoEstaAtiva, restaurarTela, matrizDoMapaAtual[py][px], px, py);
    };
    interacoes['T'] = interagirCavaleiro;
    interacoes['C'] = interagirCavaleiro;

    while (exploracaoEstaAtiva && jogadorAtual->obterVida() > 0)
    {
        int larguraDoTerminal = Aparencia::obterLarguraTerminal();
        int alturaDoTerminal = Aparencia::obterAlturaTerminal();


        ControleMapa::renderizarMapa(matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, larguraDoTerminal, alturaDoTerminal, linhaInicialParaDesenharOMapa, formatador);

        char teclaPressionadaPeloJogador = ControleDeInput::lerTecla();

        int proximaPosicaoX = posicaoXDoJogador;
        int proximaPosicaoY = posicaoYDoJogador;

        bool abriuMenu = ControleMapa::processarInputEComandos(teclaPressionadaPeloJogador, jogadorAtual, proximaPosicaoX, proximaPosicaoY, restaurarTela);
        
        if (jogadorAtual->obterVoltarProMenu()) break;
        if (abriuMenu) continue;

        ControleMapa::aplicarLimitesDeMapa(proximaPosicaoX, proximaPosicaoY, matrizDoMapaAtual);

        char celulaDestino = matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX];

        auto it = interacoes.find(celulaDestino);
        if (it != interacoes.end()) {
            it->second(proximaPosicaoX, proximaPosicaoY, larguraDoTerminal);
        } else if ((celulaDestino != '#' && celulaDestino != '=' && celulaDestino != '|' && celulaDestino != '[' && celulaDestino != ']' && celulaDestino != 'A' && celulaDestino != 'S' && celulaDestino != 'E' && celulaDestino != 'L' && celulaDestino != 'O' && celulaDestino != ' ') || jogadorAtual->isNoclip()) {
            posicaoXDoJogador = proximaPosicaoX;
            posicaoYDoJogador = proximaPosicaoY;
        }
    }
}
