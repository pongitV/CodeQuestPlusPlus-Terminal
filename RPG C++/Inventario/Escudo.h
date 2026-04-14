#pragma once

#include "Item.h"
#include <string>


class Escudo : public Item 
{
private:
    std::string nome;
    int reducaoFixa;
    int durabilidade;

public:
    Escudo(std::string nome, int reducaoFixa, int durabilidade);
    
    std::string obterNomeItem() const override;
    Raridade obterRaridade() const override;
    TipoEquipamento obterTipo() const override;

    int obterDurabilidadeAtualEscudo() const override;
    int obterReducaoDanoFixaEscudo() const override;
    void reduzirDurabilidade(int qtd) override;
    void aumentarDurabilidade(int qtd) override;

    int obterPrecoVenda() const override;
    std::string obterInfoStatus() const override;
};