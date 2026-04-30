#pragma once

#include "RacaBase.h"

class Humano : public RacaBase 
{
public:
    std::string obterNomeRaca() const override;
    TipoRaca obterTipoRaca() const override { return TipoRaca::Humano; }
    std::vector<std::string> obterAparenciaRaca() const override;
    Atributos obterAtributosRaca() const override;

    std::string obterNomeHabilidadeRaca() const override;
    std::string obterDescricaoHabilidadeRaca() const override;
    int processarDanoDefensivo(int danoFinal, SistemaPersonagem* defensor) override;
};