#pragma once

#include <vector>
#include <string>

struct EstatisticasCombate {
    int quantidadeDeOuroObtido = 0;
    int quantidadeDeXpObtido = 0;
    int totalDeDanoCausado = 0;
    int totalDeDanoRecebido = 0;
    int curaTotalRecebida = 0;
    int turnosCombate = 1;
    std::vector<std::string> itensObtidos;
    std::vector<std::string> inimigosDerrotados;
    int parriesTentados = 0;
    int parriesEfetivos = 0;
    int parriesPerfeitos = 0;
    int maiorDanoCausado = 0;
    int itensConsumidos = 0;
    std::vector<std::string> novasDescobertas;
};
