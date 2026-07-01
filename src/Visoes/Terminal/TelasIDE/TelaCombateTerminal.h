#pragma once

#include <string>
#include <vector>

#include "../../../Entidades/Personagem.h"

class TelaCombateTerminal {
public:
    static void exibir(Personagem* jogadorAtual, const std::vector<Personagem*>& inimigos, const std::string& tituloCombate = "Combate");
    static void exibirLogCombate(const std::vector<std::string>& mensagens);
    static void exibirStatsCombate(int turnos, int danoCausado, int danoRecebido, int cura);
};
