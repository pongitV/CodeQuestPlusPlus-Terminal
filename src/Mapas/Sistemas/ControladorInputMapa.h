#pragma once
#include <functional>

class Personagem;

class ControladorInputMapa {
public:
    static bool processarInputEComandos(char tecla, Personagem* jogador, int& proximaPosicaoX, int& proximaPosicaoY, const std::function<void()>& restaurarTela);
};
