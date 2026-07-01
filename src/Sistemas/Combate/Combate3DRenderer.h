#pragma once

#include <vector>
#include <string>
#include <tuple>
#include "../../Entidades/Personagem.h"

class Combate3DRenderer {
public:
    // Renderiza o fundo 3D estatico e sobrepoe a arte do inimigo
    // Retorna o quadro renderizado na resolucao da tela para ser exibido acima do HUD do combate.
    static std::vector<std::string> renderizarQuadro(
        const std::string& tituloMapa, 
        Personagem* jogador, 
        const std::vector<Personagem*>& inimigos,
        Personagem* alvoAnimacao = nullptr,
        int frame = 0,
        int framesDeDanoJogador = 0,
        int danoAmount = -1,
        bool isCura = false,
        int tempoMs = 0,
        bool isMorte = false,
        const std::vector<std::string>& dropsAnimacao = {},
        float spriteOpacity = 1.0f
    );

    // Gera uma mini-arena baseada no titulo do mapa (bioma)
    static std::vector<std::string> obterArenaPorTitulo(const std::string& titulo);

    // Retorna a cor RGB base para texturizar o sprite do inimigo no 3D
    static std::tuple<int,int,int> obterCorSpriteInimigo(Personagem* inimigo);

    // Pinta uma string de texto sobre o buffer 1D da tela 3D (overlay)
    static void pintarTextoNoBuffer(std::vector<std::string>& tela, int larguraTela, int alturaMax, int posX, int posY, const std::string& texto, const std::string& corFg, const std::string& corBgOverride = "");

private:
    // Pega as linhas de arte ASCII do monstro e pinta por cima das strings renderizadas do motor 3D
    static void sobreporSprite(
        std::vector<std::string>& tela, 
        Personagem* inimigo, 
        int inimigoIdx,
        int totalInimigos,
        int larguraTela, 
        int alturaTela, 
        int flashDanoInimigo, 
        int danoAmount, 
        bool isCura, 
        int tempoMs, 
        bool isMorte = false, 
        int frameMorte = 0, 
        const std::vector<std::string>& dropsAnimacao = {}, 
        bool isSelecionado = false,
        float spriteOpacity = 1.0f
    );
};
