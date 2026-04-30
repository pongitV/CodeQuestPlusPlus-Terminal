#include "FabricaItens.h"
#include "EquipamentoArma.h"
#include "EquipamentoEscudo.h"
#include "EquipamentoArmadura.h"
#include "ItemConsumivel.h"
#include "ItemMaterial.h"
#include "ItemMissao.h"

std::unique_ptr<Item> FabricaItens::criarItem(const std::string& nome) 
{
    // Trata automaticamente a instanciacao de itens melhorados pela Forja (+)
    if (!nome.empty() && nome.back() == '+') {
        std::string nomeBase = nome.substr(0, nome.length() - 1);
        auto itemBase = criarItem(nomeBase);
        if (itemBase) return itemBase->gerarCopiaMelhorada();
        return nullptr;
    }

    // --- Armas ---
    if (nome == "Adaga artesanal de pedra") return std::make_unique<EquipamentoArma>(nome, 5, 0, 5);
    if (nome == "Arco recurvo de madeira") return std::make_unique<EquipamentoArma>(nome, 10, 0, 3);
    if (nome == "Cajado") return std::make_unique<EquipamentoArma>(nome, 0, 30, 3);
    if (nome == "Violao encantado") return std::make_unique<EquipamentoArma>(nome, 0, 10, 3);
    if (nome == "Espada longa de ferro") return std::make_unique<EquipamentoArma>(nome, 10, 0, 3);
    if (nome == "Machado de guerra danificado") return std::make_unique<EquipamentoArma>(nome, 15, 0, 3);
    if (nome == "Gosma acida (Arma)") return std::make_unique<EquipamentoArma>("Gosma acida", 2, 7, 3); // Arma exclusiva do Slime
    
    // --- Escudos ---
    if (nome == "Escudo medio de metal") return std::make_unique<EquipamentoEscudo>(nome, 15, 5, 9);
    if (nome == "Barreira magica") return std::make_unique<EquipamentoEscudo>(nome, 50, 2, 3);
    if (nome == "Capa magica") return std::make_unique<EquipamentoEscudo>(nome, 6, 10, 9);
    if (nome == "Bracedeiras de prata") return std::make_unique<EquipamentoEscudo>(nome, 5, 3, 3);
    
    // --- Armaduras ---
    if (nome == "Armadura de malha e metal") return std::make_unique<EquipamentoArmadura>(nome, 7, 3);
    if (nome == "Armadura leve de couro com malha") return std::make_unique<EquipamentoArmadura>(nome, 5, 3);
    if (nome == "Tunica") return std::make_unique<EquipamentoArmadura>(nome, 2, 3);
    if (nome == "Traje de Couro e tecido nobre") return std::make_unique<EquipamentoArmadura>(nome, 4, 3);
    if (nome == "Armadura de trapos e sucata") return std::make_unique<EquipamentoArmadura>(nome, 3, 3);
    
    // --- Consumiveis ---
    if (nome.find("Pocao de Cura") != std::string::npos) return std::make_unique<ItemConsumivel>(nome, 6);
    
    if (nome == "Pocao de Furia (Buff)" || nome == "Elixir Arcano (Buff)") {
        auto buff = std::make_unique<ItemConsumivel>(nome, 3);
        buff->adicionarPropriedade(Propriedade::ConsumivelBuff);
        return buff;
    }
    if (nome == "Frasco de Gosma (Debuff)") {
        auto debuff = std::make_unique<ItemConsumivel>(nome);
        debuff->adicionarPropriedade(Propriedade::ConsumivelDebuffLentidao);
        return debuff;
    }
    if (nome == "Frasco de Fraqueza (Debuff)") {
        auto debuff = std::make_unique<ItemConsumivel>(nome);
        debuff->adicionarPropriedade(Propriedade::ConsumivelDebuffFraqueza);
        return debuff;
    }
    
    // --- Talismas ---
    if (nome == "Talisma do Urso") { auto t = std::make_unique<ItemConsumivel>(nome, 120); t->adicionarPropriedade(Propriedade::TalismaForca); return t; }
    if (nome == "Talisma do Corvo") { auto t = std::make_unique<ItemConsumivel>(nome, 120); t->adicionarPropriedade(Propriedade::TalismaInteligencia); return t; }
    if (nome == "Talisma do Leopardo") { auto t = std::make_unique<ItemConsumivel>(nome, 120); t->adicionarPropriedade(Propriedade::TalismaDestreza); return t; }
    if (nome == "Talisma da Coruja") { auto t = std::make_unique<ItemConsumivel>(nome, 120); t->adicionarPropriedade(Propriedade::TalismaSabedoria); return t; }
    
    // --- Materiais ---
    if (nome == "Gosma acida") return std::make_unique<ItemMaterial>(nome, 5);
    if (nome == "Dente de goblin") return std::make_unique<ItemMaterial>(nome, 1);
    if (nome == "Nucleo pegajoso") return std::make_unique<ItemMaterial>(nome, 30);
    if (nome == "Po magico") return std::make_unique<ItemMaterial>(nome, 15);
    if (nome == "Madeira enfeiticada" || nome == "Coracao da floresta" || nome == "Pedra magica de upgrade") return std::make_unique<ItemMaterial>(nome, 3);
    
    // --- Missoes ---
    if (nome == "Dispositivo de teclas de linguagem desconhecida") return std::make_unique<ItemMissao>(nome, 500);
    return nullptr;
}