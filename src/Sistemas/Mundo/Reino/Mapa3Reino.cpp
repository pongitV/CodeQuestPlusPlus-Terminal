#include "Mapa3Reino.h"

#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <unordered_map>

#include "../../../Core/Controladores/MenuJogo.h"
#include "../../../Interface/Telas/Menu/TelaMenu.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../ControleMapa.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Core/Controladores/CriadorInimigos.h"
#include "Mapa3ReinoLayout.h"
#include "../../../Entidades/NPCs/CavaleiroGenerico/NPCCavaleiroGenerico.h"


Mapa3Reino::Mapa3Reino(Personagem* personagemJogador) :
    posicaoXDoJogador(47), 
    posicaoYDoJogador(32),
    jogadorAtual(personagemJogador), 
    exploracaoEstaAtiva(true), 
    tituloDoMapaAtual("CAMINHO DO CASTELO"),
    proximoMapa(ProximaTransicaoMapa::Nenhuma)
{
    matrizDoMapaAtual = Mapa3ReinoLayouts::obterLayoutReino();
}

Mapa3Reino::~Mapa3Reino() = default;

ProximaTransicaoMapa Mapa3Reino::iniciarLoopDeExploracao()
{
    bool trollDerrotado = false;
    bool conviteRecebido = false;

    ControleMapa::padronizarTamanhoDoMapa(matrizDoMapaAtual);

    Aparencia::ocultarCursor();

    auto formatador = [&](char celula, int x, int y) -> std::string {
        if (x == posicaoXDoJogador && y == posicaoYDoJogador) {
            char ic = jogadorAtual->obterIconeJogador();
            if (ic <= 32 || ic > 126) ic = '@'; // Garante que o icone seja um caractere visivel
            return Aparencia::cor(jogadorAtual->obterCorJogador()) + std::string(1, ic) + Aparencia::cor(Cor::RESET);
        }
        return ControleMapa::formatarCelula(celula, x, y, tituloDoMapaAtual, matrizDoMapaAtual, false);
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
            arteTrans = Mapa3ReinoLayouts::obterArteTransicaoReino();
            larguraTrans = 75;
        }

        linhaInicialParaDesenharOMapa = ControleMapa::animarIntroducaoMapa(tituloDoMapaAtual, arteTitulo, larguraArte, arteTrans, larguraTrans, Cor::CIANO, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, formatador, true);
    };

    animarTela();

    std::unordered_map<char, std::function<void(int, int, int)>> interacoes;

    interacoes['^'] = [&](int px, int py, int larg) {
        // 1. Acesso ao Castelo (X=47, Y=3)
        if (px == 47 && py == 3) {
            if (!conviteRecebido) {
                Aparencia::iniciarInteracaoPopup();
                std::vector<std::string> msg = { "Os portoes estao trancados.", "Voce precisa de uma permissao real." };
                Aparencia::exibirPopup("ACESSO NEGADO", msg, Cor::CIANO);
            } else {
                Aparencia::iniciarInteracaoPopup();
                std::vector<std::string> msg = {
                    "Voce apresentou o Convite Real e os portoes se abriram!",
                    "A historia continua em breve..."
                };
                Aparencia::exibirPopup("FIM DA DEMO", msg, Cor::AMARELO);
                exploracaoEstaAtiva = false;
                proximoMapa = ProximaTransicaoMapa::VoltarMenu;
            }
        }
        // 2. Retornar para a Floresta (X=47, Y=33)
        else if (px == 47 && py == 33) {
            exploracaoEstaAtiva = false;
            proximoMapa = ProximaTransicaoMapa::Floresta;
        }
    };

    interacoes['G'] = [&](int px, int py, int larg) {
        Aparencia::iniciarInteracaoPopup();
        std::vector<std::string> msg = {
            "Alto la! Somente o Rei pode conceder passagem.",
            "(O castelo ainda esta em construcao pelos devs)"
        };
        Aparencia::exibirPopup("GUARDA REAL", msg, Cor::CIANO);
        posicaoXDoJogador = px;
        posicaoYDoJogador = py;
        if (exploracaoEstaAtiva && !ControleMapa::isExploracao3DAtiva()) restaurarTela();
    };

    auto interagirCavaleiro = [&](int px, int py, int larg) {
        NPCCavaleiroGenerico::interagir(jogadorAtual, trollDerrotado, conviteRecebido, larg, matrizDoMapaAtual, exploracaoEstaAtiva, restaurarTela, matrizDoMapaAtual[py][px], px, py);
        posicaoXDoJogador = px;
        posicaoYDoJogador = py;
    };
    interacoes['T'] = interagirCavaleiro;
    interacoes['C'] = interagirCavaleiro;

    auto processarInteracao = [&](int px, int py, int larg) {
        char celulaDestino = matrizDoMapaAtual[py][px];
        auto it = interacoes.find(celulaDestino);
        if (it != interacoes.end()) {
            it->second(px, py, larg);
        } else if ((celulaDestino != '*' && celulaDestino != '#' && celulaDestino != '=' && celulaDestino != '|' && celulaDestino != '[' && celulaDestino != ']' && celulaDestino != 'A' && celulaDestino != 'S' && celulaDestino != 'E' && celulaDestino != 'L' && celulaDestino != 'O' && celulaDestino != ' ') || jogadorAtual->isNoclip()) {
            posicaoXDoJogador = px;
            posicaoYDoJogador = py;
        }
    };

    bool precisaRenderizar = true;
    ProximaTransicaoMapa destinoViagemRapida = ControleMapa::executarLoopDeExploracao(
        jogadorAtual, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador,
        exploracaoEstaAtiva, tituloDoMapaAtual, []() { return ""; },
        []() -> std::vector<std::string> { return {}; },
        processarInteracao, formatador, restaurarTela,
        linhaInicialParaDesenharOMapa, precisaRenderizar
    );

    if (destinoViagemRapida != ProximaTransicaoMapa::Nenhuma) {
        return destinoViagemRapida;
    }

    if (jogadorAtual->obterVida() <= 0 || jogadorAtual->obterVoltarProMenu()) {
        return ProximaTransicaoMapa::VoltarMenu;
    }
    return proximoMapa;
}
