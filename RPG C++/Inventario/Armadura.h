#pragma once

#include <string>

#include "Item.h"

class Armadura : public Item 
{
private:
    int reducaoFixa;

public:
    Armadura(std::string nome, int reducaoFixa, int precoVenda = 3);
    
    Raridade obterRaridade() const override;
    TipoEquipamento obterTipo() const override;

    int obterReducaoFixa() const { return reducaoFixa; }

    std::string obterInfoStatus() const override;
};
