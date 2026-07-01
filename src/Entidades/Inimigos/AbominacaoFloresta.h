#pragma once

#include <string>
#include <vector>

#include "../Racas/RacaBase.h"

class AbominacaoFloresta : public RacaBase
{
private:
    bool curandoAtivamente = false;

public:
    std::string obterNomeRaca() const override;
    TipoRaca obterTipoRaca() const override { return TipoRaca::AbominacaoFloresta; }
    Atributos obterAtributosRaca() const override;
    std::string obterNomeHabilidadeRaca() const override;
    std::string obterDescricaoHabilidadeRaca() const override;
    const std::vector<std::string>& obterAparenciaRaca() const override;

    InfoBestiario obterInfoBestiario() const override;

    void aoCausarDano(Personagem* atacante, Personagem* alvo, int danoCausado) override;

    void realizarDrops(Personagem* inimigo, Personagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal) override;
};
