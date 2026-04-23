#include <vector>
#include <string>

#include "../Sistema/Personagem.h"

#pragma once

class Mapa2Floresta 
{
private:
    std::vector<std::string> matrizDoMapaAtual;
    int posicaoXDoJogador;
    int posicaoYDoJogador;
    Personagem* jogadorAtual;
    
    std::vector<std::string> matrizDoMapaPrincipalSalva;
    int posicaoXSalvaAntesDeEntrarNoSubMapa;
    int posicaoYSalvaAntesDeEntrarNoSubMapa;
    bool jogadorEstaDentroDeUmSubMapa;

    std::vector<std::string> matrizDoMapaDoCoracaoDaArvoreSalva;

public:
    Mapa2Floresta(Personagem* personagemJogador);
    void iniciarLoopDeExploracaoDoMapa();
};