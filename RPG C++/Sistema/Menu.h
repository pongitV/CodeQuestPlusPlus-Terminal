#include "Personagem.h"

#pragma once

class Menu 
{
public:
    static void exibirLogo();
    static Personagem* criarPersonagem();
    static void esperar();
    static void exibirInventario(Personagem* p); 

private:
    static void limparTela();
    static int obterLarguraTerminal(); 
};