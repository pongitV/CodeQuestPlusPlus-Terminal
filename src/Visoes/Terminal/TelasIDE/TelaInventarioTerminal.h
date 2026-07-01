#pragma once

#include <string>
#include <vector>
#include <utility>

#include "../../../Entidades/Personagem.h"

class TelaInventarioTerminal {
public:
    static void exibir(Personagem* jogadorAtual);
    static void exibirCategoria(Personagem* jogadorAtual, int categoria, bool mostrarPrecos = false);
    static void exibirInspecaoItem(const std::string& nomeItem, const std::string& descricao, int valor, const std::string& tipo);
};
