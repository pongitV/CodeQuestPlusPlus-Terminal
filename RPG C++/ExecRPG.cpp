#include <iostream>
#include <string>
#include <vector>

#include "Sistema/Menu.h"
#include "Sistema/Personagem.h"
#include "Sistema/GeradorInimigos.h"
#include "Sistema/SistemaRPG.h"

int main()
{
    // 1. Criacao do Jogador via Menu
    Personagem* heroi = Menu::criarPersonagem();

    // 2. Criacao dos Inimigos via Gerador
    std::vector<Personagem*> inimigos = GeradorInimigos::gerarHordaGoblins(3);

    // 3. Inicio do Combate
    SistemaRPG* motorJogo = new SistemaRPG(heroi, inimigos);
    motorJogo->iniciarCombate();

    // 4. Limpeza de Memoria
    delete motorJogo; 
    delete heroi;

    inimigos.clear(); 

    return 0;
}