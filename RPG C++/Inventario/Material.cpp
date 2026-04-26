#include "Material.h"
#include <string_view>
#include <map>

Material::Material(std::string nome, int preco) : Item(preco), nome(nome)
{
}

std::string Material::obterNomeItem() const { return nome; }
TipoEquipamento Material::obterTipo() const { return TipoEquipamento::MATERIAL; }
