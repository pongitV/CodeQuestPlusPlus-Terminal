#include "Escudo.h"

Escudo::Escudo(std::string nome, int reducaoFixa, int durabilidade, int precoVenda) 
    : Item(nome, precoVenda), reducaoFixa(reducaoFixa), durabilidade(durabilidade) 
{
}

Raridade Escudo::obterRaridade() const { return Raridade::COMUM; }
TipoEquipamento Escudo::obterTipo() const { return TipoEquipamento::ESCUDO; }

std::string Escudo::obterInfoStatus() const {
    return " (Def: " + std::to_string(reducaoFixa) + " | Dur: " + std::to_string(durabilidade) + ")";
}
