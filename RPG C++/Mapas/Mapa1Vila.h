#pragma once

#include <string>
#include <vector>

#include "../Sistemas/SistemaPersonagem.h"

class Mapa1Vila 
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

    std::vector<std::string> matrizDoMapaDaCavernaSalva;

    std::vector<std::string> matrizDoMapaDaLojaSalva;

public:
    Mapa1Vila(SistemaPersonagem* personagemJogador);
    
    void iniciarLoopDeExploracaoDoMapa1Vila();
};