#include "ItemMissao.h"

ItemMissao::ItemMissao(std::string nome, int preco) : Item(preco), nome(nome) {}

std::string ItemMissao::obterNomeItem() const { return nome; }
TipoEquipamento ItemMissao::obterTipo() const { return TipoEquipamento::MISSAO; }
