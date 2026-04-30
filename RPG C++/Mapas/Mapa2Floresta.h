#include <vector>
#include <string>

#include "../Sistemas/SistemaPersonagem.h"

#pragma once

class Mapa2Floresta 
{
private:
    std::vector<std::string> matrizDoMapaAtual;
    int posicaoXDoJogador;
    int posicaoYDoJogador;
    SistemaPersonagem* jogadorAtual;
    
    std::vector<std::string> matrizDoMapaPrincipalSalva;
    int posicaoXSalvaAntesDeEntrarNoSubMapa;
    int posicaoYSalvaAntesDeEntrarNoSubMapa;
    bool jogadorEstaDentroDeUmSubMapa;

    std::vector<std::string> matrizDoMapaDoCoracaoDaArvoreSalva;

public:
    Mapa2Floresta(SistemaPersonagem* personagemJogador);
    void iniciarLoopDeExploracaoDoMapa();
};