#pragma once

#include <string>

class Personagem;

class TelaCombate 
{
public:
    static void exibirLogoParaTelaDeCombate(const std::string& tituloDaTela = "");
    static void exibirBarraDeStatusDoJogador(Personagem* jogadorAtual);
};
