#include "ItemConsumivel.h"

ItemConsumivel::ItemConsumivel(std::string nome) : nome(nome) 
{
}

std::string ItemConsumivel::obterNomeItem() const { return nome; }

Raridade ItemConsumivel::obterRaridade() const {
    if (nome.find("Talisma") != std::string::npos) return Raridade::RARO;
    return Raridade::COMUM;
}

TipoEquipamento ItemConsumivel::obterTipo() const { return TipoEquipamento::CONSUMIVEL; }

int ItemConsumivel::obterPrecoVenda() const {
    if (nome.find("Talisma") != std::string::npos) return 120;
    if (nome.find("Pocao de Cura") != std::string::npos) return 6;
    return 3;
}