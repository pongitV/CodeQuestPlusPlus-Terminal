#pragma once

#include <string>
#include <vector>

#include "../../../Entidades/Personagem.h"

class TelaDiarioTerminal {
public:
    static void exibir(Personagem* jogadorAtual);
    static void exibirEntradaDiario(const std::string& titulo, const std::vector<std::string>& conteudo);
    static void exibirFichaRaca(const std::string& nomeRaca, const std::vector<std::string>& descricao);
    static void exibirFichaClasse(const std::string& nomeClasse, const std::vector<std::string>& descricao);
    static void exibirFichaNPC(const std::string& nomeNPC, const std::vector<std::string>& descricao);
    static void exibirFichaItem(const std::string& nomeItem, const std::vector<std::string>& descricao);
    static void exibirFichaMissao(const std::string& nomeMissao, const std::vector<std::string>& descricao);
};
