#pragma once

#include <string>

class SistemaPersonagem;

class TelaCombate 
{
public:
    static void exibirLogoParaTelaDeCombate(const std::string& tituloDaTela = "");
    static void exibirBarraDeStatusDoJogador(SistemaPersonagem* jogadorAtual);
};
