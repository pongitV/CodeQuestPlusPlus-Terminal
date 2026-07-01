#pragma once

#include "RacaBase.h"

class Ork : public RacaBase
{
public:
    std::string obterNomeRaca() const override;
    TipoRaca obterTipoRaca() const override { return TipoRaca::Ork; }
    const std::vector<std::string>& obterAparenciaRaca() const override;
    Atributos obterAtributosRaca() const override;

    std::string obterNomeHabilidadeRaca() const override;
    std::string obterDescricaoHabilidadeRaca() const override;
    int processarDanoOfensivo(int danoBase, Personagem* atacante) override;
};
