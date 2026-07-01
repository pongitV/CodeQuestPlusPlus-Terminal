#pragma once

#include <string>
#include <vector>

#include "../../Entidades/Personagem.h"
#include "../MapaInteracao.h"

class Mapa3PonteReino final : public IMapa 
{
public:
    std::vector<std::string> matrizDoMapaAtual;
    int posicaoXDoJogador;
    int posicaoYDoJogador;
    Personagem* jogadorAtual;
    
    bool exploracaoEstaAtiva;
    std::string tituloDoMapaAtual;
    ProximaTransicaoMapa proximoMapa;

public:
    explicit Mapa3PonteReino(Personagem* personagemJogador);
    ~Mapa3PonteReino() override;

    std::string obterTitulo() const override { return tituloDoMapaAtual; }
    ProximaTransicaoMapa iniciarLoopDeExploracao() override;
};
