#include "Personagem.h"

#pragma once

class Menu 
{
public:
    static void exibirLogo();
    static void limparTela();
    static void esperar();
    
    static Personagem* criarPersonagem();
    static void exibirStatusJogador(Personagem* p);
    static void exibirInventario(Personagem* p); 

    static void exibirHorda(const std::vector<Personagem*>& inimigos);

private:
    static int obterLarguraTerminal(); 
};