#include "Armadura.h"

Armadura::Armadura(std::string nome, int reducaoFixa, int preco) 
    : Item(preco), nome(nome), reducaoFixa(reducaoFixa) 
{
}

std::string Armadura::obterNomeItem() const { return nome; }
TipoEquipamento Armadura::obterTipo() const { return TipoEquipamento::ARMADURA; }

int Armadura::obterReducaoFixa() const { return reducaoFixa; }

std::string Armadura::obterInfoStatus() const {
    std::string info = " (Def: " + std::to_string(reducaoFixa);
    if (int penalidadeDestreza = reducaoFixa / 3; penalidadeDestreza > 0) {
        info += " | -" + std::to_string(penalidadeDestreza) + " Dest";
    }
    return info + ")";
}

std::unique_ptr<Item> Armadura::gerarCopiaMelhorada() const {
    auto novaArmadura = std::make_unique<Armadura>(nome + "+", static_cast<int>(reducaoFixa * 1.5), precoVenda * 2);
    for (Propriedade prop : propriedades) novaArmadura->adicionarPropriedade(prop);
    novaArmadura->adicionarPropriedade(Propriedade::Melhorado);
    return novaArmadura;
}
