#pragma once

#include "../Entidades/Personagem.h"
#include <functional>
#include <string>
#include <vector>
#include <memory>
#include "../Core/Utilidades/Aparencia.h"
#include "MapaInteracao.h"

class ControleMapa 
{
public:
    // Processa o input de movimento e de menus. 
    // Retorna 'true' se um menu foi aberto e o turno de interacao do mapa deve ser pulado.
    static bool processarInputEComandos(char tecla, Personagem* jogador, int& proximaPosicaoX, int& proximaPosicaoY, const std::function<void()>& restaurarTela);

    static void aplicarLimitesDeMapa(int& posicaoX, int& posicaoY, const std::vector<std::string>& matrizDoMapa);

    static void processarCombate(
        Personagem* jogadorAtual, std::vector<std::string>& matrizDoMapaAtual, 
        int& posicaoXDoJogador, int& posicaoYDoJogador, bool& exploracaoEstaAtiva,
        const std::string& tituloDoCombate, const std::string& mensagemDeAviso, std::vector<std::unique_ptr<Personagem>> inimigosParaBatalha, 
        int posicaoXAposCombate, int posicaoYAposCombate, int posicaoXInicialDoInimigo, int quantidadeDeCelulasOcupadas, int larguraDoTerminal, const std::function<void()>& restaurarTela);

    static void entrarSubMapa(
        std::vector<std::string>& matrizDoMapaAtual, std::vector<std::string>& matrizDoMapaPrincipalSalva,
        int& posicaoXSalvaAntesDeEntrarNoSubMapa, int& posicaoYSalvaAntesDeEntrarNoSubMapa,
        int& posicaoXDoJogador, int& posicaoYDoJogador, bool& jogadorEstaDentroDeUmSubMapa,
        std::string& tituloDoMapaAtual, std::vector<std::string>& matrizDoSubMapaSalva, bool& subMapaJaFoiVisitado,
        const std::vector<std::string>& matrizDoSubMapaGerada, int posicaoXInicialNoSubMapa, int posicaoYInicialNoSubMapa, const std::string& tituloDoSubMapa, const std::function<void()>& restaurarTela);

    static void moverInimigosAleatoriamente(std::vector<std::string>& matrizDoMapaAtual, const std::vector<std::string>& matrizOriginal, const std::string& simbolosInimigos, int jogadorX, int jogadorY);

    static int animarIntroducaoMapa(
        const std::string& tituloDoMapa,
        const std::vector<std::string>& arteDoMapa,
        int larguraArte,
        const std::vector<std::string>& arteTransicao,
        int larguraTransicao,
        Cor corTema,
        const std::vector<std::string>& matrizDoMapa,
        int posicaoXDoJogador,
        int posicaoYDoJogador,
        const std::function<std::string(char, int, int)>& formatadorCelula,
        bool animar = true,
        bool usarAnimacaoBanner = true,
        const std::function<void()>& acaoAposFadeInArte = nullptr
    );

    static void animarFlashbang(int r, int g, int b);

    static void calcularCameraVertical(int alturaDoTerminal, int linhaInicial, int posicaoYDoJogador, int tamanhoDoMapa, int& startY, int& endY);
    static void calcularCameraHorizontal(int larguraDoTerminal, int posicaoXDoJogador, int larguraDoMapa, int& startX, int& endX);
    static std::string calcularMargemCentralizada(int larguraDoTerminal, int larguraDoTexto);
    static void padronizarTamanhoDoMapa(std::vector<std::string>& matrizDoMapa);

    static bool isExploracao3DAtiva();
    static void sinalizarTrocaDeMapa3D();
    static float obterPosCamera3DX();
    static float obterPosCamera3DY();
    static float obterAnguloCamera3D();
    static std::string obterTituloMapaAtual();
    static std::vector<std::string> obterMatrizDoMapaAtual();

    static void renderizarMapa(const std::vector<std::string>& matrizDoMapa, int posicaoXDoJogador, int posicaoYDoJogador, int larguraDoTerminal, int alturaDoTerminal, int linhaInicial, const std::function<std::string(char, int, int)>& formatadorCelula);

    static std::string formatarCelula(char celula, int x, int y, const std::string& tituloDoMapa, const std::vector<std::string>& matrizDoMapa, bool isMinimapa = false);

    static ProximaTransicaoMapa executarLoopDeExploracao(
        Personagem* jogadorAtual,
        std::vector<std::string>& matrizDoMapaAtual,
        int& posicaoXDoJogador,
        int& posicaoYDoJogador,
        bool& exploracaoEstaAtiva,
        const std::string& tituloDoMapaAtual,
        const std::function<std::string()>& obterSimbolosInimigos,
        const std::function<std::vector<std::string>()>& obterLayoutOriginal,
        const std::function<void(int, int, int)>& processarInteracao,
        const std::function<std::string(char, int, int)>& formatador,
        const std::function<void()>& restaurarTela,
        int& linhaInicialParaDesenharOMapa,
        bool& precisaRenderizar
    );
};
