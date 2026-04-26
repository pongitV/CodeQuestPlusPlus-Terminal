#include "Escudo.h"
#include <string_view>
#include <map>

static const std::map<std::string_view, int> precosEscudos = {
    {"Manto encantado", 9},
    {"Escudo medio de metal", 9},
    {"Capa magica", 9},
    {"Escudo leve de madeira", 9},
};

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
    auto it = precosEscudos.find(nome);
    return (it != precosEscudos.end()) ? it->second : 3;
}

std::string Escudo::obterInfoStatus() const {
    return " (Def: " + std::to_string(reducaoFixa) + " | Dur: " + std::to_string(durabilidade) + ")";
}
