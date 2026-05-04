#include "ControleDeMapa.h"
#include "../Inventario/InventarioCombate.h"
#include "../Telas/TelaAtributos.h"
#include "../Telas/TelaBestiario.h"
#include "../Gerenciadores/GerenciadorMenu.h"
#include "../Utilidades/SimplificacoesAparencia.h"
#include "../Gerenciadores/GerenciadorCombate.h"
#include "../Utilidades/ControleDeInput.h"
#include <iostream>

bool ControleDeMapa::processarInputEComandos(char tecla, SistemaPersonagem* jogador, int& proximaPosicaoX, int& proximaPosicaoY, const std::function<void()>& restaurarTela)
{
    ComandoMapa comando = ControleDeInput::traduzirTeclaParaComando(tecla);

    // Movimentação (Não abre menus, portanto retorna falso)
    if (comando == ComandoMapa::Cima) { proximaPosicaoY--; return false; }
    if (comando == ComandoMapa::Baixo) { proximaPosicaoY++; return false; }
    if (comando == ComandoMapa::Esquerda) { proximaPosicaoX--; return false; }
    if (comando == ComandoMapa::Direita) { proximaPosicaoX++; return false; }

    // Menus (Retornam true informando o mapa que a tela precisa pular a interação de movimento)
    if (comando == ComandoMapa::Inventario)
    {
        InventarioCombate::gerenciarInventario(jogador);
        restaurarTela();
        return true;
    }
    if (comando == ComandoMapa::Ficha)
    {
        TelaAtributos::gerenciarFichaDoJogador(jogador);
        restaurarTela();
        return true;
    }
    if (comando == ComandoMapa::Bestiario)
    {
        TelaBestiario::exibirLista(jogador);
        restaurarTela();
        return true;
    }
    return false;
}

void ControleDeMapa::processarCombate(
    SistemaPersonagem* jogadorAtual, std::vector<std::string>& matrizDoMapaAtual, 
    int& posicaoXDoJogador, int& posicaoYDoJogador, bool& exploracaoEstaAtiva,
    const std::string& titulo, const std::string& msg, std::vector<std::unique_ptr<SistemaPersonagem>> inimigos, 
    int px, int py, int rootX, int celulas, int larguraDoTerminal, const std::function<void()>& restaurarTela)
{
    SimplificacoesAparencia::limparTela();
    GerenciadorMenu::exibirLogoDoJogo(titulo);
    int espacosM = std::max(0, (larguraDoTerminal - static_cast<int>(msg.length())) / 2);
    std::string mE(espacosM, ' ');
    std::cout << "\n" << mE << SimplificacoesAparencia::cor(Cor::AMARELO) << "[!] " << msg << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
    std::cout << mE << "[0] Nao, recuar | [1] Sim, batalha!\n" << mE << "Escolha: ";

    int opcao;
    if (std::cin >> opcao && opcao == 1) {
        GerenciadorCombate combate(jogadorAtual, std::move(inimigos));
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
