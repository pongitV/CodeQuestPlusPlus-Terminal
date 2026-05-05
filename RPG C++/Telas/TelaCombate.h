#pragma once

#include <string>
#include <vector>

class SistemaPersonagem;

class TelaCombate 
{
public:
    static void exibirLogoParaTelaDeCombate(const std::string& tituloDaTela = "");
    static void exibirBarraDeStatusDoJogador(SistemaPersonagem* jogadorAtual);
    static void exibirHordaDeInimigosLadoALado(const std::vector<SistemaPersonagem*>& listaDeInimigos);
};
