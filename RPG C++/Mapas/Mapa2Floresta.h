#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "../Sistemas/SistemaPersonagem.h"
#include "MapaInteracao.h"

class Mapa2Floresta 
{
public:
    std::vector<std::string> matrizDoMapaAtual;
    int posicaoXDoJogador;
    int posicaoYDoJogador;
    SistemaPersonagem* jogadorAtual;
    
    std::vector<std::string> matrizDoMapaPrincipalSalva;
    int posicaoXSalvaAntesDeEntrarNoSubMapa;
    int posicaoYSalvaAntesDeEntrarNoSubMapa;
    bool jogadorEstaDentroDeUmSubMapa;

    std::vector<std::string> matrizDoMapaDoCoracaoDaArvoreSalva;
    std::vector<std::string> matrizDoMapaDaCabanaSalva;
    std::vector<std::string> matrizDoMapaDoLabirintoSalva;
    bool cabanaJaFoiVisitada;
    bool coracaoDaArvoreJaFoiVisitado;
    bool labirintoJaFoiVisitado;
    bool exploracaoEstaAtiva;
    std::string tituloDoMapaAtual;

    std::unordered_map<char, std::unique_ptr<InteracaoFloresta>> interacoes;

public:
    Mapa2Floresta(SistemaPersonagem* personagemJogador);
    ~Mapa2Floresta();
    void iniciarLoopDeExploracaoDoMapa();
private:
    void inicializarInteracoes();
};