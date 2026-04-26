#include "ControleDeMapa.h"
#include "../Inventario/InventarioCombate.h"
#include "../Interfaces/TelaAtributos.h"
#include "../Interfaces/TelaBestiario.h"
#include "../Sistema/FuncionalidadeMenu.h"
#include "../Sistema/SimplificacoesAparencia.h"
#include "../Sistema/GerenciadorCombate.h"
#include <iostream>

bool ControleDeMapa::processarInputEComandos(char tecla, Personagem* jogador, int& proximaPosicaoX, int& proximaPosicaoY, const std::function<void()>& restaurarTela)
{
    // Movimentação (Não abre menus, portanto retorna falso)
    if (tecla == 'w' || tecla == 'W' || tecla == 72) { proximaPosicaoY--; return false; }
    if (tecla == 's' || tecla == 'S' || tecla == 80) { proximaPosicaoY++; return false; }
    if (tecla == 'a' || tecla == 'A' || tecla == 75) { proximaPosicaoX--; return false; }
    if (tecla == 'd' || tecla == 'D' || tecla == 77) { proximaPosicaoX++; return false; }

    // Menus (Retornam true informando o mapa que a tela precisa pular a interação de movimento)
    if (tecla == 'i' || tecla == 'I')
    {
        InventarioCombate::gerenciarInventario(jogador);
        restaurarTela();
        return true;
    }
    if (tecla == 'c' || tecla == 'C')
    {
        TelaAtributos::gerenciarFichaDoJogador(jogador);
        restaurarTela();
        return true;
    }
    if (tecla == 'b' || tecla == 'B')
    {
        TelaBestiario::exibirLista(jogador);
        restaurarTela();
        return true;
    }
    return false;
}

void ControleDeMapa::processarCombate(
    Personagem* jogadorAtual, std::vector<std::string>& matrizDoMapaAtual, 
    int& posicaoXDoJogador, int& posicaoYDoJogador, bool& exploracaoEstaAtiva,
    const std::string& titulo, const std::string& msg, std::vector<std::unique_ptr<Personagem>> inimigos, 
    int px, int py, int rootX, int celulas, int larguraDoTerminal, const std::function<void()>& restaurarTela)
{
    SimplificacoesAparencia::limparTela();
    Menu::exibirLogoDoJogo(titulo);
    int espacosM = std::max(0, (larguraDoTerminal - static_cast<int>(msg.length())) / 2);
    std::string mE(espacosM, ' ');
    std::cout << "\n" << mE << "[!] " << msg << "\n";
    std::cout << mE << "[0] Nao, recuar | [1] Sim, batalha!\n" << mE << "Escolha: ";

    int opcao;
    if (std::cin >> opcao && opcao == 1) {
        SistemaRPG combate(jogadorAtual, std::move(inimigos));
        combate.iniciarCombate();

        if (jogadorAtual->obterVida() > 0) {
            for (int i = 0; i < celulas; ++i) matrizDoMapaAtual[py][rootX + i] = '.';
            posicaoXDoJogador = px;
            posicaoYDoJogador = py;
        }
    } else { std::cin.clear(); std::cin.ignore(1000, '\n'); }

    if (exploracaoEstaAtiva) restaurarTela();
}

void ControleDeMapa::entrarSubMapa(
    std::vector<std::string>& matrizDoMapaAtual, std::vector<std::string>& matrizDoMapaPrincipalSalva,
    int& posicaoXSalvaAntesDeEntrarNoSubMapa, int& posicaoYSalvaAntesDeEntrarNoSubMapa,
    int& posicaoXDoJogador, int& posicaoYDoJogador, bool& jogadorEstaDentroDeUmSubMapa,
    std::string& tituloDoMapaAtual, std::vector<std::string>& mapaSalvo, bool& jaVisitado,
    const std::vector<std::string>& mapaGerado, int spawnX, int spawnY, const std::string& titulo, const std::function<void()>& restaurarTela)
{
    matrizDoMapaPrincipalSalva = matrizDoMapaAtual;
    posicaoXSalvaAntesDeEntrarNoSubMapa = posicaoXDoJogador;
    posicaoYSalvaAntesDeEntrarNoSubMapa = posicaoYDoJogador;

    if (!jaVisitado) { matrizDoMapaAtual = mapaGerado; jaVisitado = true; } 
    else { matrizDoMapaAtual = mapaSalvo; }

    posicaoXDoJogador = spawnX;
    posicaoYDoJogador = spawnY;
    jogadorEstaDentroDeUmSubMapa = true;
    tituloDoMapaAtual = titulo;
    restaurarTela();
}
