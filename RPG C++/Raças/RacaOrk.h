#include <string>

#include "RacaBase.h"

#pragma once

class RacaOrk : public RacaBase
{
public:
    std::string obterNomeRaca() const override;
    Atributos obterAtributosRaca() const override;

    void usarHabilidadeRaca(Personagem* usuario, Personagem* alvo) override;
    std::vector<std::string> obterAparencia() const override;
};