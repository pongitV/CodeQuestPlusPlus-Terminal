#pragma once

#include "Item.h"
#include <string>

class ItemMissao : public Item {
private:
    std::string nome;
public:
    ItemMissao(std::string nome, int preco = 500);
    std::string obterNomeItem() const override;
    TipoEquipamento obterTipo() const override;
};
