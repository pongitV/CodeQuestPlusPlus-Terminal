#include "Mapa3PonteReino.h"

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
#include "Mapa3PonteReinoLayout.h"
#include "../../../Entidades/NPCs/CavaleiroGenerico/NPCCavaleiroGenerico.h"


Mapa3PonteReino::Mapa3PonteReino(Personagem* personagemJogador) :
    posicaoXDoJogador(47), 
    posicaoYDoJogador(32),
    jogadorAtual(personagemJogador), 
    exploracaoEstaAtiva(true), 
    tituloDoMapaAtual("PONTE DO REINO"),
    proximoMapa(ProximaTransicaoMapa::Nenhuma)
{
    matrizDoMapaAtual = Mapa3PonteReinoLayouts::obterLayoutPonteReino();
    ControleMapa::padronizarTamanhoDoMapa(matrizDoMapaAtual);
}

Mapa3PonteReino::~Mapa3PonteReino() = default;

ProximaTransicaoMapa Mapa3PonteReino::iniciarLoopDeExploracao()
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

        if (tituloDoMapaAtual == "PONTE DO REINO" || tituloDoMapaAtual == "CAMINHO DO Reino") {
            arteTitulo = Mapa3PonteReinoLayouts::obterLogoPonteReino();
            larguraArte = 150; // A nova arte ASCII tem cerca de 150 caracteres de largura
            arteTrans = Mapa3PonteReinoLayouts::obterArteTransicaoPonteReino();
            larguraTrans = 75;
        }

        linhaInicialParaDesenharOMapa = ControleMapa::animarIntroducaoMapa(tituloDoMapaAtual, arteTitulo, larguraArte, arteTrans, larguraTrans, Cor::CIANO, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, formatador, true);
    };

    auto animarTela_ = animarTela; // Para fins estáticos
    animarTela();

    std::unordered_map<char, std::function<void(int, int, int)>> interacoes;

    interacoes['^'] = [&](int px, int py, int larg) {
        // 1. Acesso ao Reino (X=47, Y=3)
        if (px == 47 && py == 3) {
            if (!conviteRecebido) {
                Aparencia::iniciarInteracaoPopup();
                std::vector<std::string> msg = { "Os portoes estao trancados.", "Voce precisa de uma permissao real." };
                Aparencia::exibirPopup("ACESSO NEGADO", msg, Cor::CIANO);
            } else {
                Aparencia::iniciarInteracaoPopup();
                std::vector<std::string> msg = {
                    "Voce apresentou o Convite Real e os portoes se abriram!",
                    "Entrando no Reino do Reino..."
                };
                Aparencia::exibirPopup("ACESSO CONCEDIDO", msg, Cor::AMARELO);
                exploracaoEstaAtiva = false;
                proximoMapa = ProximaTransicaoMapa::Reino;
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
            "(O Reino ainda esta em construcao pelos devs)"
        };
        Aparencia::exibirPopup("GUARDA REAL", msg, Cor::CIANO);
        if (exploracaoEstaAtiva && !ControleMapa::isExploracao3DAtiva()) restaurarTela();
    };

    auto interagirCavaleiro = [&](int px, int py, int larg) {
        NPCCavaleiroGenerico::interagir(jogadorAtual, trollDerrotado, conviteRecebido, larg, matrizDoMapaAtual, exploracaoEstaAtiva, restaurarTela, matrizDoMapaAtual[py][px], px, py);
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
