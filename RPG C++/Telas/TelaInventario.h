#pragma once

#include "../Sistemas/SistemaPersonagem.h"

class TelaInventario 
{
public:
    static void exibir(SistemaPersonagem* jogadorAtual, bool mostrarPrecos = false);

    static void exibirMenuInteracaoItem(Item* itemEncontrado);
    static void exibirInspecaoItem(Item* item, SistemaPersonagem* jogadorAtual = nullptr);
    static void exibirPrompt(const std::string& mensagem);
    static Item* lerSelecaoDeItem(SistemaPersonagem* jogadorAtual, std::string& outCodigoDigitado);
};