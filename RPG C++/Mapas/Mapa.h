#pragma once
#include <vector>
#include <string>
#include "../Sistema/Personagem.h"

class Mapa 
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

    std::vector<std::string> matrizDoMapaDaCavernaSalva;
    bool cavernaJaFoiVisitada;

    std::vector<std::string> matrizDoMapaDaLojaSalva;
    bool lojaJaFoiVisitada;

public:
    Mapa(Personagem* personagemJogador);
    
    void iniciarLoopDeExploracaoDoMapa();
};