#include "Escudo.h"

Escudo::Escudo(std::string nome, int reducaoFixa, int durabilidade) 
    : nome(nome), reducaoFixa(reducaoFixa), durabilidade(durabilidade) 
{
}

std::string Escudo::obterNomeItem() const { return nome; }
Raridade Escudo::obterRaridade() const { return Raridade::COMUM; }
TipoEquipamento Escudo::obterTipo() const { return TipoEquipamento::ESCUDO; }

int Escudo::obterDurabilidadeAtualEscudo() const { return durabilidade; }
int Escudo::obterReducaoDanoFixaEscudo() const { return reducaoFixa; }
void Escudo::reduzirDurabilidade(int qtd) { durabilidade -= qtd; }
void Escudo::aumentarDurabilidade(int qtd) { durabilidade += qtd; }

int Escudo::obterPrecoVenda() const {
    if (nome == "Manto encantado" || nome == "Escudo medio de metal" || nome == "Capa magica" || nome == "Escudo leve de madeira") return 9;
    return 3;
}

std::string Escudo::obterInfoStatus() const {
    return " (Def: " + std::to_string(reducaoFixa) + " | Dur: " + std::to_string(durabilidade) + ")";
}
