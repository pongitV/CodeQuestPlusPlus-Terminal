#include "Armadura.h"

Armadura::Armadura(std::string nome, int reducaoFixa, int precoVenda) 
    : Item(nome, precoVenda), reducaoFixa(reducaoFixa) 
{
}

Raridade Armadura::obterRaridade() const { return Raridade::COMUM; }
TipoEquipamento Armadura::obterTipo() const { return TipoEquipamento::ARMADURA; }

std::string Armadura::obterInfoStatus() const {
    return " (Def: " + std::to_string(reducaoFixa) + ")";
}
