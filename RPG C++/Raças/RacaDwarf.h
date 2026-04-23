#include <iostream>

#include "RacaBase.h"

#pragma once

class RacaDwarf : public RacaBase
{
public:
    std::string obterNomeRaca() const override;
    TipoRaca obterTipoRaca() const override { return TipoRaca::Dwarf; }
    std::vector<std::string> obterAparenciaRaca() const override;
    Atributos obterAtributosRaca() const override;

    std::string obterNomeHabilidadeRaca() const override;
    std::string obterDescricaoHabilidadeRaca() const override;
    int processarDanoDefensivo(int danoFinal, Personagem* defensor) override;
};
