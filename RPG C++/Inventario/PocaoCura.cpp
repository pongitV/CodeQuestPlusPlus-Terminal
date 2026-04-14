#include "PocaoCura.h"

PocaoCura::PocaoCura() 
{
}

std::string PocaoCura::obterNomeItem() const { return "Pocao de Cura (30%)"; }
Raridade PocaoCura::obterRaridade() const { return Raridade::COMUM; }
TipoEquipamento PocaoCura::obterTipo() const { return TipoEquipamento::CONSUMIVEL; }
int PocaoCura::obterPrecoVenda() const { return 6; }