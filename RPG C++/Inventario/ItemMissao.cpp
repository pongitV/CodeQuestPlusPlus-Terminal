#include "ItemMissao.h"
#include <functional>
#include <unordered_map>

ItemMissao::ItemMissao(std::string nome, int preco) : Item(preco), nome(nome) {}

std::string ItemMissao::obterNomeItem() const { return nome; }
TipoEquipamento ItemMissao::obterTipo() const { return TipoEquipamento::MISSAO; }

std::unique_ptr<Item> fabricarItemMissao(const std::string& nome) {
    static const std::unordered_map<std::string, std::function<std::unique_ptr<Item>()>> construtores = {
        {"Dispositivo de teclas de linguagem desconhecida", []() { return std::make_unique<ItemMissao>("Dispositivo de teclas de linguagem desconhecida", 500); }}
    };
    auto it = construtores.find(nome);
    if (it != construtores.end()) return it->second();
    return nullptr;
}
