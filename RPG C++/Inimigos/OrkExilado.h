#pragma once

#include <string>
#include <vector>

#include "../Racas/Ork.h"

class OrkExilado : public Ork
{
public:
    std::string obterNomeRaca() const override;
    TipoRaca obterTipoRaca() const override { return TipoRaca::OrkExilado; }
    Atributos obterAtributosRaca() const override;
    std::vector<std::string> obterAparenciaRaca() const override;
    std::vector<std::unique_ptr<Item>> obterEquipamentoRaca() const override;

    void realizarDrops(SistemaPersonagem* inimigo, SistemaPersonagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal) override;
    static std::vector<std::string> obterMapaCaverna(bool bjornResgatado);
};