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
    std::vector<std::string> obterAparenciaRaca() const override;

    int processarDanoOfensivo(int danoBase, SistemaPersonagem* atacante) override;

    void realizarDrops(SistemaPersonagem* inimigo, SistemaPersonagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal) override;
    static std::vector<std::string> obterMapaCoracaoDaArvore();
};
