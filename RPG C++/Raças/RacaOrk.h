#include <string>

#include "RacaBase.h"

#pragma once

class RacaOrk : public RacaBase
{
public:
    std::string obterNomeRaca() const override;
    std::vector<std::string> obterAparenciaRaca() const override;
    Atributos obterAtributosRaca() const override;

    std::string obterNomeHabilidadeRaca() const override;
    std::string obterDescricaoHabilidadeRaca() const override;
    int processarDanoOfensivo(int danoBase, Personagem* atacante) override;
};