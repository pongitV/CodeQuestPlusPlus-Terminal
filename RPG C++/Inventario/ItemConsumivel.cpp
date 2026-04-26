#include "ItemConsumivel.h"

ItemConsumivel::ItemConsumivel(std::string nome) : nome(nome)
{
    if (nome.find("Pocao de Cura") != std::string::npos) {
        adicionarPropriedade(Propriedade::ConsumivelCura);
    }
}

bool ItemConsumivel::ehTalisma() const {
    return temPropriedade(Propriedade::TalismaForca) || temPropriedade(Propriedade::TalismaInteligencia) ||
           temPropriedade(Propriedade::TalismaDestreza) || temPropriedade(Propriedade::TalismaSabedoria);
}

std::string ItemConsumivel::obterNomeItem() const { return nome; }

Raridade ItemConsumivel::obterRaridade() const {
    return ehTalisma() ? Raridade::RARO : Raridade::COMUM;
}

TipoEquipamento ItemConsumivel::obterTipo() const { return TipoEquipamento::CONSUMIVEL; }

int ItemConsumivel::obterPrecoVenda() const {
    if (ehTalisma()) return 120;
    if (temPropriedade(Propriedade::ConsumivelCura)) return 6;
    return 3;
}
