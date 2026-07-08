#pragma once

#include <vector>
#include <string>
#include "../../../Entidades/Personagem.h"

class RaycasterHUD {
public:
    static void desenhar(std::vector<std::string>& tela, int larguraTela, int alturaTela, float jogadorX, float jogadorY, float anguloVisao, const std::vector<std::string>& matrizDoMapa, const std::string& tituloMapa, bool temaFloresta, Personagem* jogador);
    static void desenharBarraStatus(std::vector<std::string>& tela, int LARGURA_TELA, int ALTURA_TELA, Personagem* jogador, float anguloVisao, const std::string& tituloBorda = "", int framesDanoJogador = 0, int danoAmount = -1, bool isCura = false);

private:
    static void desenharMinimapa(std::vector<std::string>& tela, int LARGURA_TELA, int ALTURA_TELA, float jogadorX, float jogadorY, float anguloVisao, const std::vector<std::string>& matrizDoMapa, const std::string& tituloMapa, bool temaFloresta, char iconeDoJogador, const std::string& corJogadorAnsi);
    static void desenharControles(std::vector<std::string>& tela, int LARGURA_TELA, int ALTURA_TELA);
};