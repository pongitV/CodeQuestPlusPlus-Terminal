#pragma once

#include <string>
#include <vector>

#include "../../../Entidades/Personagem.h"

class TelaMapaTerminal {
public:
    static void exibirTopDown(const std::vector<std::string>& matriz, int jogadorX, int jogadorY, const std::string& tituloMapa);
    static void exibirCoordenadas(int x, int y);
    static void exibirInfoMapa(const std::string& titulo, const std::vector<std::string>& conteudo);
};
