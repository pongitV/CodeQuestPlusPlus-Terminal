#pragma once

#include "Item.h"
#include <string>

class ItemMaterial : public Item {
private:
    std::string nome;
public:
    ItemMaterial(std::string nome, int preco = 3);

    std::string obterNomeItem() const override;
    TipoEquipamento obterTipo() const override;
};