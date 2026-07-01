#pragma once

#include "../Item.h"
#include <string>
#include <vector>

class ItemMaterial : public Item {
private:
    std::string nome;
public:
    ItemMaterial(const std::string& nome, int preco = 3);

    std::string obterNomeItem() const override;
    TipoEquipamento obterTipo() const override;
    std::vector<std::string> obterDetalhesInspecao(Personagem* personagem = nullptr) const override;
};

std::unique_ptr<Item> fabricarItemMaterial(ItemID id);
