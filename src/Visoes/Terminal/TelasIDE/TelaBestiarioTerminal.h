#pragma once

#include <string>
#include <vector>

#include "../../../Entidades/Personagem.h"

class TelaBestiarioTerminal {
public:
    static void exibirLista(Personagem* jogadorAtual, const std::vector<std::string>& nomesInimigos, const std::vector<std::string>& descobertos);
    static void exibirFicha(const std::string& nomeInimigo, const std::vector<std::string>& estatisticas, const std::vector<std::string>& habilidades);
};
