#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "../Sistemas/SistemaPersonagem.h"

class NPCCavaleiroGenerico {
public:
    // CRIACAO DO NPC
    static std::unique_ptr<SistemaPersonagem> criarCavaleiro(const std::string& nome);

    // INTERACAO
    static void interagir(SistemaPersonagem* jogadorAtual, bool& trollDerrotado, bool& conviteRecebido, int larguraDoTerminal, std::vector<std::string>& matrizDoMapaAtual, bool exploracaoEstaAtiva, const std::function<void()>& restaurarTela, char celulaDestino, int proximaPosicaoX, int proximaPosicaoY);
};
