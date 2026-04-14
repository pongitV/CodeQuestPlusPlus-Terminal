#include "Talisma.h"

Talisma::Talisma(std::string nome) : nome(nome) {}

std::string Talisma::obterNomeItem() const { return nome; }
Raridade Talisma::obterRaridade() const { return Raridade::RARO; }
TipoEquipamento Talisma::obterTipo() const { return TipoEquipamento::CONSUMIVEL; }
int Talisma::obterPrecoVenda() const { return 100; }
