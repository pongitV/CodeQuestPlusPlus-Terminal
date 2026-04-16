#pragma once

#include "Item.h"
#include <string>
#include <set>

class Arma : public Item 
{
private:
    int danoFisico;
    int danoMagico;
    bool efeitoSangramento;
    bool efeitoLentidao;

public:
    Arma(std::string nome, int danoFisico, int danoMagico, int precoVenda = 3);
    
    Raridade obterRaridade() const override;
    TipoEquipamento obterTipo() const override;

    int obterDanoFisico() const { return danoFisico; }
    int obterDanoMagico() const { return danoMagico; }
    
    bool possuiEfeitoSangramento() const { return efeitoSangramento; }
    bool possuiEfeitoLentidao() const { return efeitoLentidao; }

    std::string obterInfoStatus() const override;

    void aplicarEfeitoSangramento() { efeitoSangramento = true; }
    void aplicarEfeitoLentidao() { efeitoLentidao = true; }
    
    void antesDeCausarDano(Personagem& atacante, Personagem& alvo) override;
    void aoCausarDano(Personagem& atacante, Personagem& alvo, int danoCausado) override;
    int garantirDanoMinimo(int danoFinal) override;
};
