#pragma once
#include <string>

struct ResultadoDano {
    int danoFinal = 0;
    int danoBloqueado = 0;
    bool escudoQuebrou = false;
    std::string nomeEscudoQuebrado = "";
};
