#pragma once

#include "Item.h"
#include <string>

class Material : public Item {
public:
    Material(std::string nome);

    Raridade obterRaridade() const override;
    TipoEquipamento obterTipo() const override;
};