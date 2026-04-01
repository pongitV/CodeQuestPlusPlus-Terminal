#include <iostream>

#include "RacaBase.h" 

#pragma once

class RacaHumano : public RacaBase 
{
public:
    std::string obterNomeRaca() const override;
    Atributos obterAtributosRaca() const override;

    void usarHabilidadeRaca(Personagem* usuario, Personagem* alvo) override;
};