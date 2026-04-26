#pragma once

#include "../Sistema/Personagem.h"
#include <functional>
#include <string>
#include <vector>
#include <memory>

class ControleDeMapa 
{
public:
    // Processa o input de movimento e de menus. 
    // Retorna 'true' se um menu foi aberto e o turno de interacao do mapa deve ser pulado.
    static bool processarInputEComandos(char tecla, Personagem* jogador, int& proximaPosicaoX, int& proximaPosicaoY, const std::function<void()>& restaurarTela);

    static void processarCombate(
        Personagem* jogadorAtual, std::vector<std::string>& matrizDoMapaAtual, 
        int& posicaoXDoJogador, int& posicaoYDoJogador, bool& exploracaoEstaAtiva,
        const std::string& titulo, const std::string& msg, std::vector<std::unique_ptr<Personagem>> inimigos, 
        int px, int py, int rootX, int celulas, int larguraDoTerminal, const std::function<void()>& restaurarTela);

    static void entrarSubMapa(
        std::vector<std::string>& matrizDoMapaAtual, std::vector<std::string>& matrizDoMapaPrincipalSalva,
        int& posicaoXSalvaAntesDeEntrarNoSubMapa, int& posicaoYSalvaAntesDeEntrarNoSubMapa,
        int& posicaoXDoJogador, int& posicaoYDoJogador, bool& jogadorEstaDentroDeUmSubMapa,
        std::string& tituloDoMapaAtual, std::vector<std::string>& mapaSalvo, bool& jaVisitado,
        const std::vector<std::string>& mapaGerado, int spawnX, int spawnY, const std::string& titulo, const std::function<void()>& restaurarTela);
};
