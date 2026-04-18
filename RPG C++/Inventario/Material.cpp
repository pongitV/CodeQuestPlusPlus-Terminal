#include "Material.h"

Material::Material(std::string nome) : nome(nome)
{
}

std::string Material::obterNomeItem() const { return nome; }
Raridade Material::obterRaridade() const { return Raridade::COMUM; }
TipoEquipamento Material::obterTipo() const { return TipoEquipamento::MATERIAL; }

int Material::obterPrecoVenda() const {
    if (nome == "Gosma acida") return 5;
    if (nome == "Dente de goblin") return 1;
    if (nome == "Nucleo pegajoso") return 30;
    if (nome == "Po magico") return 15;
    return 3;
}