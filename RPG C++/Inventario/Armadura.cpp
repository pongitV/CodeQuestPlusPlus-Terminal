#include "Armadura.h"

Armadura::Armadura(std::string nome, int reducaoFixa) 
    : nome(nome), reducaoFixa(reducaoFixa) 
{
}

std::string Armadura::obterNomeItem() const { return nome; }
Raridade Armadura::obterRaridade() const { return Raridade::COMUM; }
TipoEquipamento Armadura::obterTipo() const { return TipoEquipamento::ARMADURA; }

int Armadura::obterReducaoFixa() const { return reducaoFixa; }

std::string Armadura::obterInfoStatus() const {
    int penalidadeDestreza = reducaoFixa / 3;
    if (penalidadeDestreza > 0) {
        return " (Def: " + std::to_string(reducaoFixa) + " | -" + std::to_string(penalidadeDestreza) + " Dest)";
    }
    return " (Def: " + std::to_string(reducaoFixa) + ")";
}
