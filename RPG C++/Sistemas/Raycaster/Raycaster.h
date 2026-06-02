#pragma once

#include <vector>
#include <string>
#include "../../Entidades/Personagem.h"

class Raycaster {
public:
    // Inicia um sub-loop de renderizacao e movimentacao em 1ª Pessoa.
    // Toma controle da thread atual para gerar frames em tempo real.
    // Retorna (encerra) quando o jogador pressiona a tecla ESC.
    // Utiliza referencias para atualizar a posicao X e Y reais do jogador no mapa top-down.
    static char iniciarExploracao3D(const std::vector<std::string>& matrizDoMapa, float& jogadorX, float& jogadorY, float& anguloVisao, const std::string& tituloMapa, Personagem* jogador);
};
