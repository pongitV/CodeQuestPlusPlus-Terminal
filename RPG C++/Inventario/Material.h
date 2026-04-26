#pragma once

#include "Item.h"
#include <string>

class Material : public Item {
private:
    std::string nome;
public:
    Material(std::string nome, int preco = 3);

    std::string obterNomeItem() const override;
    TipoEquipamento obterTipo() const override;
};