#include "ItemMaterial.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include <vector>
#include <functional>
#include <unordered_map>
#include "../FabricaItens.h"

ItemMaterial::ItemMaterial(std::string nome, int preco) : Item(preco), nome(nome)
{
}

std::string ItemMaterial::obterNomeItem() const { return nome; }
TipoEquipamento ItemMaterial::obterTipo() const { return TipoEquipamento::MATERIAL; }

std::vector<std::string> ItemMaterial::obterDetalhesInspecao(Personagem* personagem) const {
    std::vector<std::string> linhas;
    linhas.push_back(" > Tipo: Material");
    
    if (!descricaoInspecao.empty()) {
        for (const auto& desc : descricaoInspecao) linhas.push_back(" > Descricao: " + desc);
    } else {
        linhas.push_back(" > Descricao: Pode ser util para construcoes ou rituais.");
    }
    return linhas;
}

std::unique_ptr<Item> fabricarItemMaterial(ItemID id) {
    auto criarMaterial = [](ItemID id, int preco, const std::string& desc = "") {
        auto m = std::make_unique<ItemMaterial>(FabricaItens::obterNomeDeID(id), preco);
        if (!desc.empty()) m->definirDescricaoInspecao(desc);
        return m;
    };

    static const std::unordered_map<ItemID, std::function<std::unique_ptr<Item>()>> construtores = {
        {ItemID::GosmaAcida, [criarMaterial]() { return criarMaterial(ItemID::GosmaAcida, 5); }},
        {ItemID::DenteGoblin, [criarMaterial]() { return criarMaterial(ItemID::DenteGoblin, 1, "Pode ser usado na Cabana da Bruxa para encantar armas com Sangramento (Requer 40x)."); }},
        {ItemID::NucleoPegajoso, [criarMaterial]() { return criarMaterial(ItemID::NucleoPegajoso, 30, "Pode ser usado na Cabana da Bruxa para encantar armas com Lentidao (Requer 5x)."); }},
        {ItemID::PoMagico, [criarMaterial]() { return criarMaterial(ItemID::PoMagico, 15, "Pode ser usado na Cabana da Bruxa para encantar armas com Quebra de Resistencia Permanente (Requer 25x)."); }},
        {ItemID::MadeiraEnfeiticada, [criarMaterial]() { return criarMaterial(ItemID::MadeiraEnfeiticada, 3, "Pode ser usada na Cabana da Bruxa para encantar o Arco ou o Violao (Requer 1x)."); }},
        {ItemID::CoracaoFloresta, [criarMaterial]() { return criarMaterial(ItemID::CoracaoFloresta, 3, "Usado na Cabana da Bruxa para encantar o Cajado ou para desbloquear a passagem do labirinto (Requer 3x)."); }},
        {ItemID::PedraUpgrade, [criarMaterial]() { return criarMaterial(ItemID::PedraUpgrade, 3, "Uma pedra extremamente rara. Pode ser usada na Forja de Bjorn para conceder +3 de Defesa (Resistencia) a uma armadura."); }},
        {ItemID::ConviteReal, [criarMaterial]() { return criarMaterial(ItemID::ConviteReal, 1, "Permite o acesso livre aos portoes do Castelo Real."); }}
    };
    auto it = construtores.find(id);
    if (it != construtores.end()) return it->second();
    return nullptr;
}






