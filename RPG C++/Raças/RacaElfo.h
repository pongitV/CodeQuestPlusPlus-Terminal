#include <iostream>

#include "RacaBase.h" 

#pragma once

class RacaElfo : public RacaBase 
{
public:
    std::string obterNomeRaca() const override;
    std::vector<std::string> obterAparenciaRaca() const override;
    Atributos obterAtributosRaca() const override;

    void usarHabilidadeRaca(Personagem* usuario, Personagem* alvo) override;
    std::string obterNomeHabilidade() const override;
    std::string obterDescricaoHabilidade() const override;
};