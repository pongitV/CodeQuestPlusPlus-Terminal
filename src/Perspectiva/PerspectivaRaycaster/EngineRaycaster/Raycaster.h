#pragma once

#include <vector>
#include <string>
#include "RaycasterQuadro.h"
#include "../../../Entidades/Personagem.h"
#include "../../../Core/Utilidades/Aparencia.h"

class Raycaster : public RaycasterQuadro {
public:
    static float sensibilidadeX;
    static float sensibilidadeY;

    static char iniciarExploracao3D(const std::vector<std::string>& matrizDoMapa, float& jogadorX, float& jogadorY, float& anguloVisao, const std::string& tituloMapa, Personagem* jogador, int& outHitX, int& outHitY, int tipoAnimacaoEntrada = 0);
    static void piscarTelaCor(Cor cor, int duracaoMs);
    static std::vector<std::string> desenharQuadroEstatico3D(const std::vector<std::string>& matrizDoMapa, float jogadorX, float jogadorY, float anguloVisao, const std::string& tituloMapa, Personagem* jogador, int alturaOverride = -1);
};
