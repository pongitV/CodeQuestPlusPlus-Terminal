#pragma once

#include "Item.h"
#include <string>

class Talisma : public Item 
{
private:
    std::string nome;
public:
    Talisma(std::string nome);
    std::string obterNomeItem() const override;
    Raridade obterRaridade() const override;
    TipoEquipamento obterTipo() const override;
    int obterPrecoVenda() const override;
};
