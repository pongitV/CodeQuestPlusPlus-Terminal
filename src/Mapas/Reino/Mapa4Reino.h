#pragma once

#include <string>
#include <vector>

#include "../../Entidades/Personagem.h"
#include "../MapaInteracao.h"

class Mapa4Reino final : public IMapa 
{
public:
    std::vector<std::string> matrizDoMapaAtual;
    int posicaoXDoJogador;
    int posicaoYDoJogador;
    Personagem* jogadorAtual;
    
    bool exploracaoEstaAtiva;
    std::string tituloDoMapaAtual;
    ProximaTransicaoMapa proximoMapa;

    // Controle de Submapa (Igreja)
    bool jogadorEstaDentroDeUmSubMapa;
    std::vector<std::string> matrizDoMapaPrincipalSalva;
    int posicaoXSalvaAntesDeEntrarNoSubMapa;
    int posicaoYSalvaAntesDeEntrarNoSubMapa;
    std::vector<std::string> matrizDoMapaDaIgrejaSalva;
    bool igrejaJaFoiVisitada;

public:
    explicit Mapa4Reino(Personagem* personagemJogador);
    ~Mapa4Reino() override;

    std::string obterTitulo() const override { return tituloDoMapaAtual; }
    ProximaTransicaoMapa iniciarLoopDeExploracao() override;
};
