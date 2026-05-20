#pragma once

#include <string>
#include <vector>

#include "../../../Entidades/Personagem.h"

class Mapa3Reino 
{
public:
    std::vector<std::string> matrizDoMapaAtual;
    int posicaoXDoJogador;
    int posicaoYDoJogador;
    Personagem* jogadorAtual;
    
    bool exploracaoEstaAtiva;
    std::string tituloDoMapaAtual;

public:
    Mapa3Reino(Personagem* personagemJogador);
    ~Mapa3Reino();
    void iniciarLoopDeExploracaoDoMapa();
};
