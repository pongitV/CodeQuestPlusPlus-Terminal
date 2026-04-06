#pragma once
#include <vector>
#include <string>
#include "Personagem.h"

class Mapa 
{
private:
    std::vector<std::string> layout;
    int jogadorX;
    int jogadorY;
    Personagem* jogador;

public:
    Mapa(Personagem* p);
    
    // Inicia o loop do mapa e gerencia a movimentação e encontros
    void iniciarExploracao();
};