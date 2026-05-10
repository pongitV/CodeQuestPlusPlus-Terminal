#include "FabricaItens.h"
#include "EquipamentoArma.h"
#include "EquipamentoEscudo.h"
#include "EquipamentoArmadura.h"
#include "ItemConsumivel.h"
#include "ItemMaterial.h"
#include "ItemMissao.h"

#include "../Sistemas/SistemaPersonagem.h"
#include "../Utilidades/Aparencia.h"

#include <functional>
#include <unordered_map>

std::string FabricaItens::obterNomeDeID(ItemID id) {
    static const std::unordered_map<ItemID, std::string> mapaDeNomes = {
        {ItemID::AdagaPedra, "Adaga artesanal de pedra"},
        {ItemID::ArcoMadeira, "Arco recurvo de madeira"},
        {ItemID::CajadoCristal, "Cajado de cristal magico"},
        {ItemID::VarinhaCorroida, "Varinha corroida"},
        {ItemID::ViolaoEncantado, "Violao encantado"},
        {ItemID::EspadaFerro, "Espada longa de ferro"},
        {ItemID::MachadoGuerra, "Machado de guerra danificado"},
        {ItemID::GosmaAcidaArma, "Gosma acida corrosiva"},
        {ItemID::TroncoAmarrotado, "Tronco de arvore amarrotado"},
        {ItemID::EspadaCavaleiro, "Espada do Cavaleiro"},
        {ItemID::EscudoMetal, "Escudo medio de metal"},
        {ItemID::BarreiraMagica, "Barreira magica"},
        {ItemID::CapaMagica, "Capa magica"},
        {ItemID::BracedeirasPrata, "Bracedeiras de prata"},
        {ItemID::ArmaduraMalha, "Armadura de malha e metal"},
        {ItemID::ArmaduraCouro, "Armadura leve de couro com malha"},
        {ItemID::Tunica, "Tunica"},
        {ItemID::TrajeNobre, "Traje de Couro e tecido nobre"},
        {ItemID::ArmaduraTrapos, "Armadura de trapos e sucata"},
        {ItemID::ArmaduraCavaleiro, "Armadura de Cavaleiro"},
        {ItemID::ArmaduraBau, "Armadura de bau"},
        {ItemID::PocaoCura30, "Pocao de Cura (30%VM)"},
        {ItemID::PocaoFuria, "Pocao de Furia (Buff)"},
        {ItemID::ElixirArcano, "Elixir Arcano (Buff)"},
        {ItemID::FrascoGosma, "Frasco de Gosma (Debuff)"},
        {ItemID::FrascoFraqueza, "Frasco de Fraqueza (Debuff)"},
        {ItemID::OrgaoRegenerador, "Orgao regenerador"},
        {ItemID::TalismaUrso, "Talisma do Urso"},
        {ItemID::TalismaCorvo, "Talisma do Corvo"},
        {ItemID::TalismaLeopardo, "Talisma do Leopardo"},
        {ItemID::TalismaCoruja, "Talisma da Coruja"},
        {ItemID::GosmaAcida, "Gosma acida"},
        {ItemID::DenteGoblin, "Dente de goblin"},
        {ItemID::NucleoPegajoso, "Nucleo pegajoso"},
        {ItemID::PoMagico, "Po magico"},
        {ItemID::MadeiraEnfeiticada, "Madeira enfeiticada"},
        {ItemID::CoracaoFloresta, "Coracao da floresta"},
        {ItemID::PedraUpgrade, "Pedra magica de upgrade"},
        {ItemID::ConviteReal, "Convite Real"},
        {ItemID::DispositivoLinguagem, "Dispositivo de teclas de linguagem desconhecida"}
    };
    auto it = mapaDeNomes.find(id);
    return it != mapaDeNomes.end() ? it->second : "";
}

std::unique_ptr<Item> FabricaItens::criarItem(ItemID id) {
    return criarItem(obterNomeDeID(id));
}

std::vector<std::unique_ptr<Item>> FabricaItens::criarVariosItens(ItemID id, int quantidade) {
    std::vector<std::unique_ptr<Item>> itens;
    for (int i = 0; i < quantidade; ++i) {
        itens.push_back(criarItem(id));
    }
    return itens;
}

std::vector<std::unique_ptr<Item>> FabricaItens::criarKitPocoes(int quantidade) {
    return criarVariosItens(ItemID::PocaoCura30, quantidade);
}

std::unique_ptr<Item> fabricarEquipamentoArma(const std::string& nome);
std::unique_ptr<Item> fabricarEquipamentoEscudo(const std::string& nome);
std::unique_ptr<Item> fabricarEquipamentoArmadura(const std::string& nome);
std::unique_ptr<Item> fabricarItemConsumivel(const std::string& nome);
std::unique_ptr<Item> fabricarItemMaterial(const std::string& nome);
std::unique_ptr<Item> fabricarItemMissao(const std::string& nome);

std::unique_ptr<Item> FabricaItens::criarItem(const std::string& nome) 
{
    if (!nome.empty() && nome.back() == '+') {
        std::string nomeBase = nome.substr(0, nome.length() - 1);
        auto itemBase = criarItem(nomeBase);
        if (itemBase) return itemBase->gerarCopiaMelhorada();
        return nullptr;
    }

    static const std::vector<std::function<std::unique_ptr<Item>(const std::string&)>> cadeiaDeFabricantes = {
        fabricarEquipamentoArma, fabricarEquipamentoEscudo, fabricarEquipamentoArmadura,
        fabricarItemConsumivel, fabricarItemMaterial, fabricarItemMissao
    };

    for (const auto& fabricante : cadeiaDeFabricantes) {
        if (auto item = fabricante(nome)) return item;
    }

    return nullptr;
}