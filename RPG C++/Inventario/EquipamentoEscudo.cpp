#include "EquipamentoEscudo.h"
#include <string_view>
#include <map>
#include <memory>

EquipamentoEscudo::EquipamentoEscudo(std::string nome, int reducaoFixa, int durabilidade, int preco)
    : Item(preco), nome(nome), reducaoFixa(reducaoFixa), durabilidade(durabilidade)
{
}

std::string EquipamentoEscudo::obterNomeItem() const { return nome; }
TipoEquipamento EquipamentoEscudo::obterTipo() const { return TipoEquipamento::ESCUDO; }

int EquipamentoEscudo::obterDurabilidadeAtualEscudo() const { return durabilidade; }
int EquipamentoEscudo::obterReducaoDanoFixaEscudo() const { return reducaoFixa; }
void EquipamentoEscudo::reduzirDurabilidade(int qtd) { durabilidade -= qtd; }
void EquipamentoEscudo::aumentarDurabilidade(int qtd) { durabilidade += qtd; }

std::string EquipamentoEscudo::obterInfoStatus() const {
    return " (Def: " + std::to_string(reducaoFixa) + " | Dur: " + std::to_string(durabilidade) + ")";
}

std::unique_ptr<Item> EquipamentoEscudo::gerarCopiaMelhorada() const {
    auto novoEscudo = std::make_unique<EquipamentoEscudo>(nome + "+", static_cast<int>(reducaoFixa * 1.5), static_cast<int>(durabilidade * 1.5), precoVenda * 2);
    for (Propriedade prop : propriedades) novoEscudo->adicionarPropriedade(prop);
    novoEscudo->adicionarPropriedade(Propriedade::Melhorado);
    return novoEscudo;
}
