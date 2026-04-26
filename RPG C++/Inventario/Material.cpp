#include "Material.h"
#include <string_view>
#include <map>

static const std::map<std::string_view, int> precosMateriais = {
    {"Gosma acida", 5},
    {"Dente de goblin", 1},
    {"Nucleo pegajoso", 30},
    {"Po magico", 15},
};

Material::Material(std::string nome) : nome(nome)
{
}

std::string Material::obterNomeItem() const { return nome; }
Raridade Material::obterRaridade() const { return Raridade::COMUM; }
TipoEquipamento Material::obterTipo() const { return TipoEquipamento::MATERIAL; }

int Material::obterPrecoVenda() const {
    auto it = precosMateriais.find(nome);
    return (it != precosMateriais.end()) ? it->second : 3;
}
