#include "FabricaItens.h"
#include "EquipamentoArma.h"
#include "EquipamentoEscudo.h"
#include "EquipamentoArmadura.h"
#include "ItemConsumivel.h"
#include "ItemMaterial.h"
#include "ItemMissao.h"

#include "../Sistemas/SistemaPersonagem.h"
#include "../Utilidades/SimplificacoesAparencia.h"

#include <functional>
#include <unordered_map>

std::unique_ptr<Item> FabricaItens::criarItem(const std::string& nome) 
{
    if (!nome.empty() && nome.back() == '+') {
        std::string nomeBase = nome.substr(0, nome.length() - 1);
        auto itemBase = criarItem(nomeBase);
        if (itemBase) return itemBase->gerarCopiaMelhorada();
        return nullptr;
    }

    if (nome.find("Pocao de Cura") != std::string::npos) return std::make_unique<ItemConsumivel>(nome, 6);

    static const std::unordered_map<std::string, std::function<std::unique_ptr<Item>()>> registroItens = {
        // --- Armas ---
        // Ordem dos inteiros: (Dano Fisico, Dano Magico, Req. Forca, Req. Destreza, Req. Inteligencia, Req. Sabedoria, Preco Venda)
        {"Adaga artesanal de pedra", []() { return std::make_unique<EquipamentoArma>("Adaga artesanal de pedra", 5, 0, 0, 0, 0, 0, 3); }},
        {"Arco recurvo de madeira", []() { return std::make_unique<EquipamentoArma>("Arco recurvo de madeira", 10, 0, 0, 0, 0, 0, 3); }},
        {"Cajado de cristal magico", []() { return std::make_unique<EquipamentoArma>("Cajado de cristal magico", 0, 30, 0, 0, 0, 0, 3); }},
        {"Varinha corroida", []() { return std::make_unique<EquipamentoArma>("Varinha corroida", 0, 25, 0, 0, 0, 0, 3); }},
        {"Violao encantado", []() { return std::make_unique<EquipamentoArma>("Violao encantado", 0, 10, 0, 0, 0, 0, 3); }},
        {"Espada longa de ferro", []() { return std::make_unique<EquipamentoArma>("Espada longa de ferro", 10, 0, 0, 0, 0, 0, 3); }},
        {"Machado de guerra danificado", []() { return std::make_unique<EquipamentoArma>("Machado de guerra danificado", 15, 0, 10, 0, 0, 0, 3); }},
        {"Gosma acida (Arma)", []() { return std::make_unique<EquipamentoArma>("Gosma acida", 2, 7, 0, 0, 0, 0, 3); }},
        {"Tronco de arvore amarrotado", []() { return std::make_unique<EquipamentoArma>("Tronco de arvore amarrotado", 40, 0, 25, 0, 0, 0, 30); }},
        
        // --- Escudos ---
        // Ordem dos inteiros: (Defesa Fixa, Durabilidade, Req. Resistencia, Valor Req. Secundario, Tipo Atributo Secundario, Preco Venda)
        {"Escudo medio de metal", []() { return std::make_unique<EquipamentoEscudo>("Escudo medio de metal", 15, 5, 0, 0, TipoAtributo::Forca, 9); }},
        {"Barreira magica", []() { return std::make_unique<EquipamentoEscudo>("Barreira magica", 50, 2, 0, 0, TipoAtributo::Inteligencia, 3); }},
        {"Capa magica", []() { return std::make_unique<EquipamentoEscudo>("Capa magica", 6, 10, 0, 0, TipoAtributo::Sabedoria, 9); }},
        {"Bracedeiras de prata", []() { return std::make_unique<EquipamentoEscudo>("Bracedeiras de prata", 5, 3, 0, 0, TipoAtributo::Destreza, 3); }},
        
        // --- Armaduras ---
        // Ordem dos inteiros: (Defesa Fixa, Req. Resistencia, Req. Constituicao, Preco Venda)
        {"Armadura de malha e metal", []() { return std::make_unique<EquipamentoArmadura>("Armadura de malha e metal", 7, 0, 0, 3); }},
        {"Armadura leve de couro com malha", []() { return std::make_unique<EquipamentoArmadura>("Armadura leve de couro com malha", 5, 0, 0, 3); }},
        {"Tunica", []() { return std::make_unique<EquipamentoArmadura>("Tunica", 2, 0, 0, 3); }},
        {"Traje de Couro e tecido nobre", []() { return std::make_unique<EquipamentoArmadura>("Traje de Couro e tecido nobre", 4, 0, 0, 3); }},
        {"Armadura de trapos e sucata", []() { return std::make_unique<EquipamentoArmadura>("Armadura de trapos e sucata", 3, 0, 0, 3); }},
        
        // --- Consumiveis ---
        {"Pocao de Furia (Buff)", []() {
            auto buff = std::make_unique<ItemConsumivel>("Pocao de Furia (Buff)", 3);
            buff->adicionarPropriedade(Propriedade::ConsumivelBuff);
            return buff;
        }},
        {"Elixir Arcano (Buff)", []() {
            auto buff = std::make_unique<ItemConsumivel>("Elixir Arcano (Buff)", 3);
            buff->adicionarPropriedade(Propriedade::ConsumivelBuff);
            return buff;
        }},
        {"Frasco de Gosma (Debuff)", []() {
            auto debuff = std::make_unique<ItemConsumivel>("Frasco de Gosma (Debuff)");
            debuff->adicionarPropriedade(Propriedade::ConsumivelDebuffLentidao);
            debuff->definirAcaoUsar([](SistemaPersonagem* usuario, SistemaPersonagem* alvo) {
                if (!SistemaPersonagem::isValido(alvo) || alvo->obterVida() <= 0) return;
                alvo->adicionarEfeito(std::make_unique<EfeitoLentidao>(3));
                std::cout << "\n" << SimplificacoesAparencia::cor(Cor::MAGENTA) << ">> Voce jogou o frasco! " << alvo->obterNome() << " esta com lentidao por 3 turnos!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
            });
            return debuff;
        }},
        {"Frasco de Fraqueza (Debuff)", []() {
            auto debuff = std::make_unique<ItemConsumivel>("Frasco de Fraqueza (Debuff)");
            debuff->adicionarPropriedade(Propriedade::ConsumivelDebuffFraqueza);
            debuff->definirAcaoUsar([](SistemaPersonagem* usuario, SistemaPersonagem* alvo) {
                if (!SistemaPersonagem::isValido(alvo) || alvo->obterVida() <= 0) return;
                alvo->adicionarEfeito(std::make_unique<EfeitoFraqueza>(3));
                std::cout << "\n" << SimplificacoesAparencia::cor(Cor::VERMELHO) << ">> Voce jogou o frasco! " << alvo->obterNome() << " teve sua forca reduzida em 25% por 3 turnos!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
            });
            return debuff;
        }},
        {"Orgao regenerador", []() {
            auto buff = std::make_unique<ItemConsumivel>("Orgao regenerador", 500);
            buff->adicionarPropriedade(Propriedade::ConsumivelPoderTroll);
            return buff;
        }},
        
        // --- Talismas ---
        {"Talisma do Urso", []() { auto t = std::make_unique<ItemConsumivel>("Talisma do Urso", 120); t->adicionarPropriedade(Propriedade::TalismaForca); return t; }},
        {"Talisma do Corvo", []() { auto t = std::make_unique<ItemConsumivel>("Talisma do Corvo", 120); t->adicionarPropriedade(Propriedade::TalismaInteligencia); return t; }},
        {"Talisma do Leopardo", []() { auto t = std::make_unique<ItemConsumivel>("Talisma do Leopardo", 120); t->adicionarPropriedade(Propriedade::TalismaDestreza); return t; }},
        {"Talisma da Coruja", []() { auto t = std::make_unique<ItemConsumivel>("Talisma da Coruja", 120); t->adicionarPropriedade(Propriedade::TalismaSabedoria); return t; }},
        
        // --- Materiais ---
        {"Gosma acida", []() { return std::make_unique<ItemMaterial>("Gosma acida", 5); }},
        {"Dente de goblin", []() { return std::make_unique<ItemMaterial>("Dente de goblin", 1); }},
        {"Nucleo pegajoso", []() { return std::make_unique<ItemMaterial>("Nucleo pegajoso", 30); }},
        {"Po magico", []() { return std::make_unique<ItemMaterial>("Po magico", 15); }},
        {"Madeira enfeiticada", []() { return std::make_unique<ItemMaterial>("Madeira enfeiticada", 3); }},
        {"Coracao da floresta", []() { return std::make_unique<ItemMaterial>("Coracao da floresta", 3); }},
        {"Pedra magica de upgrade", []() { return std::make_unique<ItemMaterial>("Pedra magica de upgrade", 3); }},
        
        // --- Missoes ---
        {"Dispositivo de teclas de linguagem desconhecida", []() { return std::make_unique<ItemMissao>("Dispositivo de teclas de linguagem desconhecida", 500); }}
    };

    auto it = registroItens.find(nome);
    if (it != registroItens.end()) {
        return it->second();
    }

    return nullptr;
}