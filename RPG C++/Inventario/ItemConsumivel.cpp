#include "ItemConsumivel.h"
#include "../Gerenciadores/GerenciadorEfeitosStatus.h"
#include "../Sistemas/SistemaPersonagem.h"
#include "../Utilidades/SimplificacoesAparencia.h"
#include <functional>
#include <unordered_map>

ItemConsumivel::ItemConsumivel(std::string nome, int preco) : Item(preco), nome(nome)
{
    if (nome.find("Pocao de Cura") != std::string::npos) {
        adicionarPropriedade(Propriedade::ConsumivelCura);
    }
}

bool ItemConsumivel::ehTalisma() const {
    return temPropriedade(Propriedade::TalismaForca) || temPropriedade(Propriedade::TalismaInteligencia) ||
           temPropriedade(Propriedade::TalismaDestreza) || temPropriedade(Propriedade::TalismaSabedoria);
}

std::string ItemConsumivel::obterNomeItem() const { return nome; }

TipoEquipamento ItemConsumivel::obterTipo() const { return TipoEquipamento::CONSUMIVEL; }

std::unique_ptr<Item> fabricarItemConsumivel(const std::string& nome) {
    if (nome.find("Pocao de Cura") != std::string::npos) return std::make_unique<ItemConsumivel>(nome, 6);

    static const std::unordered_map<std::string, std::function<std::unique_ptr<Item>()>> construtores = {
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
        {"Orgao regenerador", []() { auto buff = std::make_unique<ItemConsumivel>("Orgao regenerador", 500); buff->adicionarPropriedade(Propriedade::ConsumivelPoderTroll); return buff; }},
        {"Talisma do Urso", []() { auto t = std::make_unique<ItemConsumivel>("Talisma do Urso", 120); t->adicionarPropriedade(Propriedade::TalismaForca); return t; }},
        {"Talisma do Corvo", []() { auto t = std::make_unique<ItemConsumivel>("Talisma do Corvo", 120); t->adicionarPropriedade(Propriedade::TalismaInteligencia); return t; }},
        {"Talisma do Leopardo", []() { auto t = std::make_unique<ItemConsumivel>("Talisma do Leopardo", 120); t->adicionarPropriedade(Propriedade::TalismaDestreza); return t; }},
        {"Talisma da Coruja", []() { auto t = std::make_unique<ItemConsumivel>("Talisma da Coruja", 120); t->adicionarPropriedade(Propriedade::TalismaSabedoria); return t; }}
    };
    auto it = construtores.find(nome);
    if (it != construtores.end()) return it->second();
    return nullptr;
}
