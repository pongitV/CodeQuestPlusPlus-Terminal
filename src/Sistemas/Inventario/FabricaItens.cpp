#include "FabricaItens.h"
#include "Equipamentos/EquipamentoArma.h"
#include "Equipamentos/EquipamentoEscudo.h"
#include "Equipamentos/EquipamentoArmadura.h"
#include "Itens/ItemConsumivel.h"
#include "Itens/ItemMaterial.h"
#include "Itens/ItemMissao.h"

#include "../../Entidades/Personagem.h"
#include "../../Core/Utilidades/Aparencia.h"

#include <functional>
#include <utility>
#include <unordered_map>

namespace {
    const std::pair<const char*, ItemID> mapaDeNomesGlobais[] = {
        {"Adaga artesanal de pedra", ItemID::AdagaPedra},
        {"Arco recurvo de madeira", ItemID::ArcoMadeira},
        {"Cajado de cristal magico", ItemID::CajadoCristal},
        {"Varinha corroida", ItemID::VarinhaCorroida},
        {"Violao encantado", ItemID::ViolaoEncantado},
        {"Cajado de osso", ItemID::CajadoOsso},
        {"Espada longa de ferro", ItemID::EspadaFerro},
        {"Machado de guerra danificado", ItemID::MachadoGuerra},
        {"Gosma acida corrosiva", ItemID::GosmaAcidaArma},
        {"Tronco de arvore amarrotado", ItemID::TroncoAmarrotado},
        {"Espada do Cavaleiro", ItemID::EspadaCavaleiro},
        
        {"Escudo medio de metal", ItemID::EscudoMetal},
        {"Barreira magica", ItemID::BarreiraMagica},
        {"Capa magica", ItemID::CapaMagica},
        {"Bracedeiras de prata", ItemID::BracedeirasPrata},
        
        {"Armadura de malha e metal", ItemID::ArmaduraMalha},
        {"Armadura leve de couro com malha", ItemID::ArmaduraCouro},
        {"Tunica", ItemID::Tunica},
        {"Traje de Couro e tecido nobre", ItemID::TrajeNobre},
        {"Armadura de trapos e sucata", ItemID::ArmaduraTrapos},
        {"Armadura de Cavaleiro", ItemID::ArmaduraCavaleiro},
        {"Roupas de Ritualista", ItemID::RoupasRitualista},
        {"Armadura de bau", ItemID::ArmaduraBau},
        {"Roda da Adaptacao", ItemID::RodaAdaptacao},
        
        {"Pocao de Cura (30%VM)", ItemID::PocaoCura30},
        {"Pocao de Furia (Buff)", ItemID::PocaoFuria},
        {"Elixir Arcano (Buff)", ItemID::ElixirArcano},
        {"Frasco de Gosma (Debuff)", ItemID::FrascoGosma},
        {"Frasco de Fraqueza (Debuff)", ItemID::FrascoFraqueza},
        {"Orgao regenerador", ItemID::OrgaoRegenerador},
        {"Talisma do Urso", ItemID::TalismaUrso},
        {"Talisma do Corvo", ItemID::TalismaCorvo},
        {"Talisma do Leopardo", ItemID::TalismaLeopardo},
        {"Talisma da Coruja", ItemID::TalismaCoruja},
        {"Maca", ItemID::Maca},
        {"Pao", ItemID::Pao},
        {"Queijo", ItemID::Queijo},
        {"Carne Seca", ItemID::CarneSeca},
        {"Pocao de Cura Grande (50%VM)", ItemID::PocaoCuraGrande},
        {"Pocao de Forca Alquimica", ItemID::PocaoForcaAlquimica},
        {"Pocao de Veneno Alquimica (Debuff)", ItemID::PocaoVenenoAlquimica},
        {"Pocao de Lentidao Alquimica (Debuff)", ItemID::PocaoLentidaoAlquimica},
        
        {"Gosma acida", ItemID::GosmaAcida},
        {"Dente de goblin", ItemID::DenteGoblin},
        {"Nucleo pegajoso", ItemID::NucleoPegajoso},
        {"Po magico", ItemID::PoMagico},
        {"Madeira enfeiticada", ItemID::MadeiraEnfeiticada},
        {"Coracao da floresta", ItemID::CoracaoFloresta},
        {"Pedra magica de upgrade", ItemID::PedraUpgrade},
        {"Convite Real", ItemID::ConviteReal},
        
        {"Dispositivo de teclas de linguagem desconhecida", ItemID::DispositivoLinguagem}
    };

    struct MapasItens {
        std::unordered_map<ItemID, std::string> idParaNome;
        std::unordered_map<std::string, ItemID> nomeParaId;
        MapasItens() {
            for (const auto& par : mapaDeNomesGlobais) {
                idParaNome[par.second] = par.first;
                nomeParaId[par.first] = par.second;
            }
        }
    };

    const MapasItens& obterMapasItens() {
        static MapasItens mapas;
        return mapas;
    }
}

std::string FabricaItens::obterNomeDeID(ItemID id) {
    const auto& mapa = obterMapasItens().idParaNome;
    auto it = mapa.find(id);
    return it != mapa.end() ? it->second : "";
}

ItemID FabricaItens::obterIDDeNome(const std::string& nome) {
    const auto& mapa = obterMapasItens().nomeParaId;
    auto it = mapa.find(nome);
    return it != mapa.end() ? it->second : ItemID::Nenhum;
}

std::vector<std::unique_ptr<Item>> FabricaItens::criarVariosItens(ItemID id, int quantidade) {
    std::vector<std::unique_ptr<Item>> itens;
    itens.reserve(quantidade);
    for (int i = 0; i < quantidade; ++i) {
        itens.push_back(criarItem(id));
    }
    return itens;
}

std::vector<std::unique_ptr<Item>> FabricaItens::criarKitPocoes(int quantidade) {
    return criarVariosItens(ItemID::PocaoCura30, quantidade);
}


std::unique_ptr<Item> FabricaItens::criarItem(const std::string& nome) 
{
    if (!nome.empty() && nome.back() == '+') {
        std::string nomeBase = nome.substr(0, nome.length() - 1);
        auto itemBase = criarItem(nomeBase);
        if (itemBase) return itemBase->gerarCopiaMelhorada();
        return nullptr;
    }

    ItemID id = obterIDDeNome(nome);
    if (id != ItemID::Nenhum) return criarItem(id);

    return nullptr;
}

std::unique_ptr<Item> FabricaItens::criarItem(ItemID id) {
    static const std::vector<std::function<std::unique_ptr<Item>(ItemID)>> cadeiaDeFabricantes = {
        fabricarEquipamentoArma, fabricarEquipamentoEscudo, fabricarEquipamentoArmadura,
        fabricarItemConsumivel, fabricarItemMaterial, fabricarItemMissao
    };
    for (const auto& fabricante : cadeiaDeFabricantes) {
        if (auto item = fabricante(id)) return item;
    }
    return nullptr;
}
