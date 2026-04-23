#pragma once

#include <string>
#include <vector>

#include "../Raças/RacaBase.h"

class RacaAbominacaoFloresta : public RacaBase
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

    int processarDanoOfensivo(int danoBase, Personagem* atacante) override;

    void realizarDrops(Personagem* inimigo, Personagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal) override;
    static std::vector<std::string> obterMapaCoracaoDaArvore();
};
