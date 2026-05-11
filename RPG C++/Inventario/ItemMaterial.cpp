#include "ItemMaterial.h"
#include "../Utilidades/Aparencia.h"
#include <vector>
#include <functional>
#include <unordered_map>
#include "FabricaItens.h"

ItemMaterial::ItemMaterial(std::string nome, int preco) : Item(preco), nome(nome)
{
}

std::string ItemMaterial::obterNomeItem() const { return nome; }
TipoEquipamento ItemMaterial::obterTipo() const { return TipoEquipamento::MATERIAL; }

std::vector<std::string> ItemMaterial::obterDetalhesInspecao() const {
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
    static const std::unordered_map<ItemID, std::function<std::unique_ptr<Item>()>> construtores = {
        {ItemID::GosmaAcida, []() { return std::make_unique<ItemMaterial>(FabricaItens::obterNomeDeID(ItemID::GosmaAcida), 5); }},
        {ItemID::DenteGoblin, []() { auto i = std::make_unique<ItemMaterial>(FabricaItens::obterNomeDeID(ItemID::DenteGoblin), 1); i->definirDescricaoInspecao("Pode ser usado na Cabana da Bruxa para encantar armas com Sangramento (Requer 40x)."); return i; }},
        {ItemID::NucleoPegajoso, []() { auto i = std::make_unique<ItemMaterial>(FabricaItens::obterNomeDeID(ItemID::NucleoPegajoso), 30); i->definirDescricaoInspecao("Pode ser usado na Cabana da Bruxa para encantar armas com Lentidao (Requer 5x)."); return i; }},
        {ItemID::PoMagico, []() { auto i = std::make_unique<ItemMaterial>(FabricaItens::obterNomeDeID(ItemID::PoMagico), 15); i->definirDescricaoInspecao("Pode ser usado na Cabana da Bruxa para encantar armas com Quebra de Resistencia Permanente (Requer 25x)."); return i; }},
        {ItemID::MadeiraEnfeiticada, []() { auto i = std::make_unique<ItemMaterial>(FabricaItens::obterNomeDeID(ItemID::MadeiraEnfeiticada), 3); i->definirDescricaoInspecao("Pode ser usada na Cabana da Bruxa para encantar o Arco ou o Violao (Requer 1x)."); return i; }},
        {ItemID::CoracaoFloresta, []() { auto i = std::make_unique<ItemMaterial>(FabricaItens::obterNomeDeID(ItemID::CoracaoFloresta), 3); i->definirDescricaoInspecao("Usado na Cabana da Bruxa para encantar o Cajado ou para desbloquear a passagem do labirinto (Requer 3x)."); return i; }},
        {ItemID::PedraUpgrade, []() { auto i = std::make_unique<ItemMaterial>(FabricaItens::obterNomeDeID(ItemID::PedraUpgrade), 3); i->definirDescricaoInspecao("Uma pedra extremamente rara. Pode ser usada na Forja de Bjorn para conceder +3 de Defesa (Resistencia) a uma armadura."); return i; }},
        {ItemID::ConviteReal, []() { auto i = std::make_unique<ItemMaterial>(FabricaItens::obterNomeDeID(ItemID::ConviteReal), 1); i->definirDescricaoInspecao("Permite o acesso livre aos portoes do Castelo Real."); return i; }}
    };
    auto it = construtores.find(id);
    if (it != construtores.end()) return it->second();
    return nullptr;
}
