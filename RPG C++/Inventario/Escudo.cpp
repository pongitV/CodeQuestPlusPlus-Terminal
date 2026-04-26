#include "Escudo.h"
#include <string_view>
#include <map>
#include <memory>

Escudo::Escudo(std::string nome, int reducaoFixa, int durabilidade, int preco)
    : Item(preco), nome(nome), reducaoFixa(reducaoFixa), durabilidade(durabilidade)
{
}

std::string Escudo::obterNomeItem() const { return nome; }
TipoEquipamento Escudo::obterTipo() const { return TipoEquipamento::ESCUDO; }

int Escudo::obterDurabilidadeAtualEscudo() const { return durabilidade; }
int Escudo::obterReducaoDanoFixaEscudo() const { return reducaoFixa; }
void Escudo::reduzirDurabilidade(int qtd) { durabilidade -= qtd; }
void Escudo::aumentarDurabilidade(int qtd) { durabilidade += qtd; }

std::string Escudo::obterInfoStatus() const {
    return " (Def: " + std::to_string(reducaoFixa) + " | Dur: " + std::to_string(durabilidade) + ")";
}

std::unique_ptr<Item> Escudo::gerarCopiaMelhorada() const {
    auto novoEscudo = std::make_unique<Escudo>(nome + "+", static_cast<int>(reducaoFixa * 1.5), static_cast<int>(durabilidade * 1.5), precoVenda * 2);
    for (Propriedade prop : propriedades) novoEscudo->adicionarPropriedade(prop);
    novoEscudo->adicionarPropriedade(Propriedade::Melhorado);
    return novoEscudo;
}
