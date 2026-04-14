#pragma once

#include "Item.h"
#include <string>

class PocaoCura : public Item {
public:
    PocaoCura();

    std::string obterNomeItem() const override;
    Raridade obterRaridade() const override;
    TipoEquipamento obterTipo() const override;
    int obterPrecoVenda() const override;
};
