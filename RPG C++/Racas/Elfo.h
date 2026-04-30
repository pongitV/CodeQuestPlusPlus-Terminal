#pragma once

#include "RacaBase.h"

class Elfo : public RacaBase 
{
public:
    std::string obterNomeRaca() const override;
    TipoRaca obterTipoRaca() const override { return TipoRaca::Elfo; }
    std::vector<std::string> obterAparenciaRaca() const override;
    Atributos obterAtributosRaca() const override;

    std::string obterNomeHabilidadeRaca() const override;
    std::string obterDescricaoHabilidadeRaca() const override;
    int processarDanoOfensivo(int danoBase, SistemaPersonagem* atacante) override;
};