#include "Material.h"

Material::Material(std::string nome) : Item(nome)
{
    if (nome == "Gosma acida") precoVenda = 5;
    else if (nome == "Dente de goblin") precoVenda = 1;
    else if (nome == "Nucleo pegajoso") precoVenda = 30;
    else if (nome == "Po magico") precoVenda = 15;
    else precoVenda = 3;
}

Raridade Material::obterRaridade() const { return Raridade::COMUM; }
TipoEquipamento Material::obterTipo() const { return TipoEquipamento::MATERIAL; }
