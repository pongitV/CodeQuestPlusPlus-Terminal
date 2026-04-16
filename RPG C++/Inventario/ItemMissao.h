#pragma once

#include "Item.h"
#include <string>

class ItemMissao : public Item {
public:
    ItemMissao(std::string nome);
    Raridade obterRaridade() const override;
    TipoEquipamento obterTipo() const override;
};
