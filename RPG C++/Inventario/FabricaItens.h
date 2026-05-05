#pragma once
#include <memory>
#include <string>
#include "Item.h"

class FabricaItens {
public:
    // Cria um item de forma type-safe baseada num Enum.
    static std::unique_ptr<Item> criarItem(ItemID id);

    // Mantido para retrocompatibilidade com sistema de Saves e Encantamentos (+).
    static std::unique_ptr<Item> criarItem(const std::string& nome);
    
    static std::string obterNomeDeID(ItemID id);
};