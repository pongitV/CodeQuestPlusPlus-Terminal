#include "ItemMissao.h"
#include <functional>
#include <unordered_map>
#include "../FabricaItens.h"

ItemMissao::ItemMissao(const std::string& nome, int preco) : Item(preco), nome(nome) {}

std::string ItemMissao::obterNomeItem() const { return nome; }
TipoEquipamento ItemMissao::obterTipo() const { return TipoEquipamento::MISSAO; }

std::vector<std::string> ItemMissao::obterDetalhesInspecao(Personagem* /*personagem*/) const {
    std::vector<std::string> detalhes;
    detalhes.push_back(" > Tipo: Item de Missao");
    if (!descricaoInspecao.empty()) {
        for (const auto& desc : descricaoInspecao) detalhes.push_back(desc);
    } else {
        detalhes.push_back(" > Lore: Um item misterioso e importante para sua jornada.");
    }
    return detalhes;
}

std::unique_ptr<Item> fabricarItemMissao(ItemID id) {
    static const std::unordered_map<ItemID, std::function<std::unique_ptr<Item>()>> construtores = {
        {ItemID::DispositivoLinguagem, []() { 
            auto i = std::make_unique<ItemMissao>(FabricaItens::obterNomeDeID(ItemID::DispositivoLinguagem), 500); 
            i->definirDescricaoInspecao({" > Lore: Um estranho artefato de plastico com teclas.", "   Nao parece pertencer a este mundo, mas emana", "   uma energia peculiar..."});
            return i;
        }}
    };
    auto it = construtores.find(id);
    if (it != construtores.end()) return it->second();
    return nullptr;
}
