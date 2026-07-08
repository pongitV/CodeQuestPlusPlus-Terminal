#pragma once

#include <string>
#include <vector>
#include <utility>

class Personagem;

class TelaVitoriaRaycaster {
public:
    static void exibir(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido,
        int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate,
        const std::vector<std::string>& inimigosDerrotados, int parriesPerfeitos, int maiorDano,
        const std::vector<std::pair<std::string, int>>& dropsUnicos,
        bool podeSubirNivel, const std::vector<std::string>& novasDescobertas,
        const std::string& tituloMapa);
};
