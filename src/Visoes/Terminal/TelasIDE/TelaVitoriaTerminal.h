#pragma once

#include <string>
#include <vector>

#include "../../../Entidades/Personagem.h"

class TelaVitoriaTerminal {
public:
    static void exibir(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate, const std::vector<std::string>& itensObtidos);
    static void exibirStatsVitoria(const std::string& titulo, const std::vector<std::string>& conteudo);
};
