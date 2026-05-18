#pragma once

#include "Item.h"
#include <string>

class ItemConsumivel : public Item
{
private:
    std::string nome;

public:
    ItemConsumivel(std::string nome, int preco = 3);

    std::string obterNomeItem() const override;
    TipoEquipamento obterTipo() const override;
    std::vector<std::string> obterDetalhesInspecao(SistemaPersonagem* personagem = nullptr) const override;
};

std::unique_ptr<Item> fabricarItemConsumivel(ItemID id);