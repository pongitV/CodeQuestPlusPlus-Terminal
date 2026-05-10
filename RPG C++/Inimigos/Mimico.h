#pragma once

#include "../Racas/RacaBase.h"
#include <string>
#include <vector>
#include <memory>

class Mimico : public RacaBase
{
private:
    int ouroRoubadoTotal = 0;

public:
    std::string obterNomeRaca() const override;
    TipoRaca obterTipoRaca() const override { return static_cast<TipoRaca>(10); } // Ajuste o Enum TipoRaca no RacaBase.h se necessario
    const std::vector<std::string>& obterAparenciaRaca() const override;
    Atributos obterAtributosRaca() const override;
    std::vector<std::unique_ptr<Item>> obterEquipamentoRaca() const override;

    std::string obterNomeHabilidadeRaca() const override;
    std::string obterDescricaoHabilidadeRaca() const override;
    InfoBestiario obterInfoBestiario() const override;
    void aoCausarDano(SistemaPersonagem* atacante, SistemaPersonagem* alvo, int danoCausado) override;
    void realizarDrops(SistemaPersonagem* inimigo, SistemaPersonagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal) override;
};
