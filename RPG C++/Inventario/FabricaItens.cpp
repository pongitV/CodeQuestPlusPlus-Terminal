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
    switch(id) {
        case ItemID::AdagaPedra: return "Adaga artesanal de pedra";
        case ItemID::ArcoMadeira: return "Arco recurvo de madeira";
        case ItemID::CajadoCristal: return "Cajado de cristal magico";
        case ItemID::VarinhaCorroida: return "Varinha corroida";
        case ItemID::ViolaoEncantado: return "Violao encantado";
        case ItemID::EspadaFerro: return "Espada longa de ferro";
        case ItemID::MachadoGuerra: return "Machado de guerra danificado";
        case ItemID::GosmaAcidaArma: return "Gosma acida (Arma)";
        case ItemID::TroncoAmarrotado: return "Tronco de arvore amarrotado";
        case ItemID::EspadaCavaleiro: return "Espada do Cavaleiro";
        
        case ItemID::EscudoMetal: return "Escudo medio de metal";
        case ItemID::BarreiraMagica: return "Barreira magica";
        case ItemID::CapaMagica: return "Capa magica";
        case ItemID::BracedeirasPrata: return "Bracedeiras de prata";

        case ItemID::ArmaduraMalha: return "Armadura de malha e metal";
        case ItemID::ArmaduraCouro: return "Armadura leve de couro com malha";
        case ItemID::Tunica: return "Tunica";
        case ItemID::TrajeNobre: return "Traje de Couro e tecido nobre";
        case ItemID::ArmaduraTrapos: return "Armadura de trapos e sucata";
        case ItemID::ArmaduraCavaleiro: return "Armadura de Cavaleiro";

        case ItemID::PocaoCura30: return "Pocao de Cura (30%VM)";
        case ItemID::PocaoFuria: return "Pocao de Furia (Buff)";
        case ItemID::ElixirArcano: return "Elixir Arcano (Buff)";
        case ItemID::FrascoGosma: return "Frasco de Gosma (Debuff)";
        case ItemID::FrascoFraqueza: return "Frasco de Fraqueza (Debuff)";
        case ItemID::OrgaoRegenerador: return "Orgao regenerador";
        case ItemID::TalismaUrso: return "Talisma do Urso";
        case ItemID::TalismaCorvo: return "Talisma do Corvo";
        case ItemID::TalismaLeopardo: return "Talisma do Leopardo";
        case ItemID::TalismaCoruja: return "Talisma da Coruja";

        case ItemID::GosmaAcida: return "Gosma acida";
        case ItemID::DenteGoblin: return "Dente de goblin";
        case ItemID::NucleoPegajoso: return "Nucleo pegajoso";
        case ItemID::PoMagico: return "Po magico";
        case ItemID::MadeiraEnfeiticada: return "Madeira enfeiticada";
        case ItemID::CoracaoFloresta: return "Coracao da floresta";
        case ItemID::PedraUpgrade: return "Pedra magica de upgrade";
        case ItemID::ConviteReal: return "Convite Real";

        case ItemID::DispositivoLinguagem: return "Dispositivo de teclas de linguagem desconhecida";
        default: return "";
    }
}

std::unique_ptr<Item> FabricaItens::criarItem(ItemID id) {
    return criarItem(obterNomeDeID(id));
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

    std::unique_ptr<Item> item = nullptr;

    item = fabricarEquipamentoArma(nome);
    if (item) return item;

    item = fabricarEquipamentoEscudo(nome);
    if (item) return item;

    item = fabricarEquipamentoArmadura(nome);
    if (item) return item;

    item = fabricarItemConsumivel(nome);
    if (item) return item;

    item = fabricarItemMaterial(nome);
    if (item) return item;

    item = fabricarItemMissao(nome);
    if (item) return item;

    return nullptr;
}