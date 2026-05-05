#pragma once

#include "../Racas/RacaBase.h"

class Troll : public RacaBase
{
public:
    std::string obterNomeRaca() const override;
    TipoRaca obterTipoRaca() const override { return TipoRaca::Nenhum; }
    Atributos obterAtributosRaca() const override;
    std::string obterNomeHabilidadeRaca() const override;
    std::string obterDescricaoHabilidadeRaca() const override;
    const std::vector<std::string>& obterAparenciaRaca() const override;
    std::vector<std::unique_ptr<Item>> obterEquipamentoRaca() const override;
    void realizarDrops(SistemaPersonagem* inimigo, SistemaPersonagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal) override;
};
