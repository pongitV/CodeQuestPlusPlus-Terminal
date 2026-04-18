#pragma once

#include "Item.h"
#include <string>
#include <set>

class Arma : public Item 
{
private:
    std::string nome;
    int danoFisico;
    int danoMagico;
    bool efeitoSangramento;
    bool efeitoLentidao;

public:
    Arma(std::string nome, int danoFisico, int danoMagico);
    
    std::string obterNomeItem() const override;
    void alterarNome(const std::string& n) override;
    Raridade obterRaridade() const override;
    TipoEquipamento obterTipo() const override;

    int obterDanoFisico() const override;
    int obterDanoMagico() const override;
    
    bool possuiEfeitoSangramento() const override;
    bool possuiEfeitoLentidao() const override;

    int obterPrecoVenda() const override;
    std::string obterInfoStatus() const override;

    void aplicarEfeitoSangramento() override;
    void aplicarEfeitoLentidao() override;
    
    void antesDeCausarDano(Personagem* atacante, Personagem* alvo) override;
    void aoCausarDano(Personagem* atacante, Personagem* alvo, int danoCausado) override;
    int garantirDanoMinimo(int danoFinal) override;
};
