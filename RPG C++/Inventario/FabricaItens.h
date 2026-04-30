#pragma once
#include <memory>
#include <string>

class Item;

class FabricaItens {
public:
    // Centraliza a criacao e atributos hardcoded de todos os itens do jogo
    static std::unique_ptr<Item> criarItem(const std::string& nome);
};