#pragma once

#include "Item.h"
#include <string>

class Personagem; // Declaracao antecipada para evitar erros de tipo incompleto

class ItemConsumivel : public Item 
{
public:
    ItemConsumivel(std::string nome, int precoVenda = 3);
    
    Raridade obterRaridade() const override;
    TipoEquipamento obterTipo() const override;
    bool aoUsar(Personagem& jogador) override;
};