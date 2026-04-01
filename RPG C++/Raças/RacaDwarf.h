#include <iostream>

#include "RacaBase.h"

#pragma once

class RacaDwarf : public RacaBase
{
public:
    std::string obterNomeRaca() const override;
    Atributos obterAtributosRaca() const override;

    void usarHabilidadeRaca(Personagem* usuario, Personagem* alvo) override;
};
