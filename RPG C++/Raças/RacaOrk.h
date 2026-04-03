#include <string>

#include "RacaBase.h"

#pragma once

class RacaOrk : public RacaBase
{
public:
    std::string obterNomeRaca() const override;
    std::vector<std::string> obterAparenciaRaca() const override;
    Atributos obterAtributosRaca() const override;

    void usarHabilidadeRaca(Personagem* usuario, Personagem* alvo) override;
};