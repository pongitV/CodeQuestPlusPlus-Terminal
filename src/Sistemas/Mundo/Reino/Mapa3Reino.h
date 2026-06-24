#pragma once

#include <string>
#include <vector>

#include "../../../Entidades/Personagem.h"
#include "../MapaInteracao.h"

class Mapa3Reino final : public IMapa 
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
    explicit Mapa3Reino(Personagem* personagemJogador);
    ~Mapa3Reino() override;

    std::string obterTitulo() const override { return tituloDoMapaAtual; }
    ProximaTransicaoMapa iniciarLoopDeExploracao() override;
};
