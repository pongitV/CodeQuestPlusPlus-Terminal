#pragma once

#include "../../../Entidades/Personagem.h"

class TelaInventario 
{
public:
    static void exibir(Personagem* jogadorAtual, bool mostrarPrecos = false);

    static void exibirMenuInteracaoItem(Item* itemEncontrado);
    static void exibirInspecaoItem(Item* item, Personagem* jogadorAtual = nullptr);
    static void exibirPrompt(const std::string& mensagem);
    static Item* lerSelecaoDeItem(Personagem* jogadorAtual, std::string& outCodigoDigitado);
};
