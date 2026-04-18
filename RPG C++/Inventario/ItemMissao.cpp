#include "ItemMissao.h"

ItemMissao::ItemMissao(std::string nome) : nome(nome) {}

std::string ItemMissao::obterNomeItem() const { return nome; }
Raridade ItemMissao::obterRaridade() const { return Raridade::MITICO; }
TipoEquipamento ItemMissao::obterTipo() const { return TipoEquipamento::MISSAO; }
int ItemMissao::obterPrecoVenda() const { return 500; }
