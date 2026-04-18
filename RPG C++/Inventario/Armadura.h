#pragma once

#include <string>

#include "Item.h"

class Armadura : public Item 
{
private:
    std::string nome;
    int reducaoFixa;

public:
    Armadura(std::string nome, int reducaoFixa);
    
    std::string obterNomeItem() const override;
    Raridade obterRaridade() const override;
    TipoEquipamento obterTipo() const override;

    int obterReducaoFixa() const override;

    std::string obterInfoStatus() const override;
};
