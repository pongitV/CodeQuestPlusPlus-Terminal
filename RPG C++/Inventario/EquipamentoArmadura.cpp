#include "EquipamentoArmadura.h"

EquipamentoArmadura::EquipamentoArmadura(std::string nome, int reducaoFixa, int preco) 
    : Item(preco), nome(nome), reducaoFixa(reducaoFixa) 
{
}

std::string EquipamentoArmadura::obterNomeItem() const { return nome; }
TipoEquipamento EquipamentoArmadura::obterTipo() const { return TipoEquipamento::ARMADURA; }

int EquipamentoArmadura::obterReducaoFixa() const { return reducaoFixa; }

std::string EquipamentoArmadura::obterInfoStatus() const {
    std::string info = " (Def: " + std::to_string(reducaoFixa);
    if (int penalidadeDestreza = reducaoFixa / 3; penalidadeDestreza > 0) {
        info += " | -" + std::to_string(penalidadeDestreza) + " Dest";
    }
    return info + ")";
}

std::unique_ptr<Item> EquipamentoArmadura::gerarCopiaMelhorada() const {
    auto novaArmadura = std::make_unique<EquipamentoArmadura>(nome + "+", static_cast<int>(reducaoFixa * 1.5), precoVenda * 2);
    for (Propriedade prop : propriedades) novaArmadura->adicionarPropriedade(prop);
    novaArmadura->adicionarPropriedade(Propriedade::Melhorado);
    return novaArmadura;
}
