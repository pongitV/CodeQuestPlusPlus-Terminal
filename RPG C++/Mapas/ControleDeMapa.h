#pragma once

#include "../Sistemas/SistemaPersonagem.h"
#include <functional>
#include <string>
#include <vector>
#include <memory>

class ControleDeMapa 
{
public:
    // Processa o input de movimento e de menus. 
    // Retorna 'true' se um menu foi aberto e o turno de interacao do mapa deve ser pulado.
    static bool processarInputEComandos(char tecla, SistemaPersonagem* jogador, int& proximaPosicaoX, int& proximaPosicaoY, const std::function<void()>& restaurarTela);

    static void aplicarLimitesDeMapa(int& posicaoX, int& posicaoY, const std::vector<std::string>& matrizDoMapa);

    static void processarCombate(
        SistemaPersonagem* jogadorAtual, std::vector<std::string>& matrizDoMapaAtual, 
        int& posicaoXDoJogador, int& posicaoYDoJogador, bool& exploracaoEstaAtiva,
        const std::string& tituloDoCombate, const std::string& mensagemDeAviso, std::vector<std::unique_ptr<SistemaPersonagem>> inimigosParaBatalha, 
        int posicaoXAposCombate, int posicaoYAposCombate, int posicaoXInicialDoInimigo, int quantidadeDeCelulasOcupadas, int larguraDoTerminal, const std::function<void()>& restaurarTela);

    static void entrarSubMapa(
        std::vector<std::string>& matrizDoMapaAtual, std::vector<std::string>& matrizDoMapaPrincipalSalva,
        int& posicaoXSalvaAntesDeEntrarNoSubMapa, int& posicaoYSalvaAntesDeEntrarNoSubMapa,
        int& posicaoXDoJogador, int& posicaoYDoJogador, bool& jogadorEstaDentroDeUmSubMapa,
        std::string& tituloDoMapaAtual, std::vector<std::string>& matrizDoSubMapaSalva, bool& subMapaJaFoiVisitado,
        const std::vector<std::string>& matrizDoSubMapaGerada, int posicaoXInicialNoSubMapa, int posicaoYInicialNoSubMapa, const std::string& tituloDoSubMapa, const std::function<void()>& restaurarTela);
};
