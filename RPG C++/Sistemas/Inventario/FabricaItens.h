#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Item.h"

class FabricaItens {
public:

    // Cria um item de forma type-safe baseada num Enum.
    static std::unique_ptr<Item> criarItem(ItemID id);

    // Mantido para retrocompatibilidade com sistema de Saves e Encantamentos (+).
    static std::unique_ptr<Item> criarItem(const std::string& nome);
    
    static std::vector<std::unique_ptr<Item>> criarVariosItens(ItemID id, int quantidade);
    static std::vector<std::unique_ptr<Item>> criarKitPocoes(int quantidade = 3);

    static std::string obterNomeDeID(ItemID id);
    static ItemID obterIDDeNome(const std::string& nome);
};
