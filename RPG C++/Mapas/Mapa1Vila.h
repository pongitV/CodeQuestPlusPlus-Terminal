#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

#include "../Sistemas/SistemaPersonagem.h"
#include "MapaInteracao.h"

class Mapa1Vila 
{
public:
    std::vector<std::string> matrizDoMapaAtual;
    int posicaoXDoJogador;
    int posicaoYDoJogador;
    SistemaPersonagem* jogadorAtual;
    bool exploracaoEstaAtiva;
    std::string tituloDoMapaAtual;

    std::unordered_map<char, std::unique_ptr<InteracaoVila>> interacoes;

    std::vector<std::string> matrizDoMapaPrincipalSalva;
    int posicaoXSalvaAntesDeEntrarNoSubMapa;
    int posicaoYSalvaAntesDeEntrarNoSubMapa;
    bool jogadorEstaDentroDeUmSubMapa;

    std::vector<std::string> matrizDoMapaDaCavernaSalva;
    std::vector<std::string> matrizDoMapaDaLojaSalva;
    std::vector<std::string> matrizDoMapaDaForjaSalva;
    std::vector<std::string> mapaBaseDaVila;

    // State moved from loop function
    bool bjornResgatado;
    bool forjaJaFoiVisitada;
    bool lojaJaFoiVisitada;
    bool cavernaJaFoiVisitada;

public:
    Mapa1Vila(SistemaPersonagem* personagemJogador);
    ~Mapa1Vila();

    void iniciarLoopDeExploracaoDoMapa1Vila();
private:
    void inicializarInteracoes();
};