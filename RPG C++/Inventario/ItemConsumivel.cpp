#include "ItemConsumivel.h"

ItemConsumivel::ItemConsumivel(std::string nome, int preco) : Item(preco), nome(nome)
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

TipoEquipamento ItemConsumivel::obterTipo() const { return TipoEquipamento::CONSUMIVEL; }
