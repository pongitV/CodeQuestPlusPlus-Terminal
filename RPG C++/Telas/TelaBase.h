#pragma once

#include <string>
#include <vector>
#include <chrono>

class SistemaPersonagem;

class TelaBase 
{
public:
    virtual ~TelaBase() = default;

    static std::vector<std::string> comporEstatisticasBatalha(SistemaPersonagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate);
    static bool deveAnimarEntradaDaTela(std::chrono::steady_clock::time_point& ultimoAcesso, int delayMilissegundos = 300);
};
