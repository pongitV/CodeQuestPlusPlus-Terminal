#pragma once

#include <string>
#include <vector>

#include "../Raças/RacaBase.h"

class RacaGoblin : public RacaBase
{
public:
    std::string obterNomeRaca() const override;
    Atributos obterAtributosRaca() const override;
    std::string obterNomeHabilidadeRaca() const override;
    std::string obterDescricaoHabilidadeRaca() const override;
    std::vector<std::string> obterAparenciaRaca() const override;

    std::vector<std::unique_ptr<Item>> obterEquipamentoRaca() const override;
    void realizarDrops(Personagem* inimigo, Personagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal) override;
};