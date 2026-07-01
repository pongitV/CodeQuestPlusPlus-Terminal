#pragma once

#include <string>
#include <vector>

#include "../Racas/RacaBase.h"

class Fada : public RacaBase
{
public:
    std::string obterNomeRaca() const override;
    TipoRaca obterTipoRaca() const override { return TipoRaca::Fada; }
    Atributos obterAtributosRaca() const override;
    std::string obterNomeHabilidadeRaca() const override;
    std::string obterDescricaoHabilidadeRaca() const override;
    const std::vector<std::string>& obterAparenciaRaca() const override;
    std::vector<std::unique_ptr<Item>> obterEquipamentoRaca() const override;

    InfoBestiario obterInfoBestiario() const override;

    void realizarDrops(Personagem* inimigo, Personagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal) override;
};
