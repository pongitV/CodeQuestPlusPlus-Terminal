#pragma once

#include "Item.h"
#include <string>

class ItemConsumivel : public Item
{
private:
    std::string nome;
    bool ehTalisma() const;

public:
    ItemConsumivel(std::string nome, int preco = 3);

    std::string obterNomeItem() const override;
    TipoEquipamento obterTipo() const override;
};