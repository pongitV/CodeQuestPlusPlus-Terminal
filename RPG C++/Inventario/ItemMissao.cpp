#include "ItemMissao.h"

ItemMissao::ItemMissao(std::string nome) : Item(nome, 500) {}

Raridade ItemMissao::obterRaridade() const { return Raridade::MITICO; }
TipoEquipamento ItemMissao::obterTipo() const { return TipoEquipamento::MISSAO; }
