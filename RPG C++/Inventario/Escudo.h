#pragma once

#include "Item.h"
#include <string>


class Escudo : public Item 
{
private:
    int reducaoFixa;
    int durabilidade;

public:
    Escudo(std::string nome, int reducaoFixa, int durabilidade, int precoVenda = 3);
    
    Raridade obterRaridade() const override;
    TipoEquipamento obterTipo() const override;

    int obterDurabilidadeAtualEscudo() const { return durabilidade; }
    int obterReducaoDanoFixaEscudo() const { return reducaoFixa; }
    void reduzirDurabilidade(int qtd) { durabilidade -= qtd; }
    void aumentarDurabilidade(int qtd) { durabilidade += qtd; }

    std::string obterInfoStatus() const override;
};