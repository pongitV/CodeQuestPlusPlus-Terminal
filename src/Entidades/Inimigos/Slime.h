#pragma once

#include <string>
#include <vector>

#include "../Racas/RacaBase.h"

class Slime : public RacaBase
{
public:
    std::string obterNomeRaca() const override;
    TipoRaca obterTipoRaca() const override { return TipoRaca::Slime; }
    Atributos obterAtributosRaca() const override;
    std::string obterNomeHabilidadeRaca() const override;
    std::string obterDescricaoHabilidadeRaca() const override;
    const std::vector<std::string>& obterAparenciaRaca() const override;
    const std::vector<std::string>& obterAparenciaCombate() const override;

    InfoBestiario obterInfoBestiario() const override;

    std::vector<std::unique_ptr<Item>> obterEquipamentoRaca() const override;
    void realizarDrops(Personagem* inimigo, Personagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal) override;

    void aoCausarDano(Personagem* atacante, Personagem* alvo, int danoCausado) override;
};
