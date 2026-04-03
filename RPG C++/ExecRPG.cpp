#include <iostream>
#include <string>
#include <vector>

#include "Sistema/Menu.h"
#include "Sistema/Personagem.h"
#include "Sistema/GeradorInimigos.h"
#include "Sistema/SistemaRPG.h"
#include "Sistema/Menu.h"

int main()
{
    Menu::configurarTelaCheia();
    Personagem* heroi = Menu::criarPersonagem();
    std::vector<Personagem*> inimigos = GeradorInimigos::gerarHordaGoblins(3);
    SistemaRPG* motor = new SistemaRPG(heroi, inimigos);
    motor->iniciarCombate();
    delete motor; delete heroi;
    return 0;
}