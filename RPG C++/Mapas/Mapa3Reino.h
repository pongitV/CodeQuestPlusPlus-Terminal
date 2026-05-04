#pragma once

#include <string>
#include <vector>

#include "../Sistemas/SistemaPersonagem.h"

class Mapa3Reino 
{
public:
    std::vector<std::string> matrizDoMapaAtual;
    int posicaoXDoJogador;
    int posicaoYDoJogador;
    SistemaPersonagem* jogadorAtual;
    
    bool exploracaoEstaAtiva;
    std::string tituloDoMapaAtual;

public:
    Mapa3Reino(SistemaPersonagem* personagemJogador);
    ~Mapa3Reino();
    void iniciarLoopDeExploracaoDoMapa();
};
