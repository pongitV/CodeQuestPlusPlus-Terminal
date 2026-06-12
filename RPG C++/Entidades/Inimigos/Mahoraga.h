#pragma once

#include "../Racas/RacaBase.h"

class Mahoraga : public RacaBase {
private:
    int parrysSofridos = 0;
    int defesasComEscudoSofridas = 0;
public:
    std::string obterNomeRaca() const override;
    TipoRaca obterTipoRaca() const override { return TipoRaca::Mahoraga; }
    Atributos obterAtributosRaca() const override;
    std::vector<std::unique_ptr<Item>> obterEquipamentoRaca() const override;
    const std::vector<std::string>& obterAparenciaRaca() const override;
    const std::vector<std::string>& obterAparenciaCombate() const override;

    std::string obterNomeHabilidadeRaca() const override;
    std::string obterDescricaoHabilidadeRaca() const override;
    
    void aoCausarDano(Personagem* atacante, Personagem* alvo, int danoCausado) override;
    void aoSofrerParryPerfeito() override;
    void aoTerAtaqueBloqueadoPorEscudo();
    bool ignoraParry() const override;
    bool ignoraEscudo() const;

    InfoBestiario obterInfoBestiario() const override;
    void realizarDrops(Personagem* inimigo, Personagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal) override;
};
