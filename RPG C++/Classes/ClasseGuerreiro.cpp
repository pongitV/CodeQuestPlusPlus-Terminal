#include <iostream>
#include <vector>

#include "ClasseGuerreiro.h"
#include "../Inventario/Item.h"

std::string ClasseGuerreiro::obterNomeClasse() const 
{ 
    return "Guerreiro"; 
}

std::vector<std::string> ClasseGuerreiro::obterAparenciaClasseMenu() const 
{
    return 
    {
        "      /\\",
        "      ||",
        "      ||",
        "      ||",
        "      ||           {}",
        "      ||          .--.",
        "      ||         /.--.\\",
        "      ||         |====|",
        "      ||         |`::`|",
        "     _||_    .-;`\\..../`;_.-^-._",
        "      /\\\\   /  |...::..|`   :   `|",
        "      |:'\\ |   /'''::''|   .:.   |",
        "       \\ /\\;-,/\\   ::  |..:::::..|",
        "        \\ <` >  >._::_.| ':::::' |",
        "         `\"\"`  /   ^^  |   ':'   |",
        "               |       \\    :    /",
        "               |        \\   :   /",
        "               |___/\\___|`-.:.-`",
        "                \\_ || _/    `",
        "                <_ >< _>",
        "                |  ||  |",
        "                |  ||  |",
        "               _\\.:||:./_",
        "              /____/\\____\\"
    };
}

Atributos ClasseGuerreiro::obterAtributosClasse() const
{
    return { 20, 1000000, 5, 5, 5, 0, 0 };
}

std::vector<Item*> ClasseGuerreiro::gerarKitInicial() const 
{
    return 
    {
        new PocaoCura(), new PocaoCura(), new PocaoCura(),
        new Arma("Espada longa de metal", 10),
        new Escudo("Escudo medio de metal", 0.15),
        new Armadura("Armadura media de malha", 8)
    };
}

void ClasseGuerreiro::usarHabilidadeClasseAtiva(Personagem* u, std::vector<Personagem*>& inimigos) 
{
    if (u->obterTurnosBuff() <= 0) 
    {
        u->definirTurnosBuff(3); // Primeira vez: 3 turnos
        u->definirMultiplicador(1.5);
        std::cout << "[HABILIDADE]: Determinacao no combate! Atributos x1.5\n";
    } 
    else 
    {
        // Se ja tiver buff, acumula o multiplicador mas NAO aumenta a duracao
        double novoMult = u->obterMultiplicador() + 1.5;
        u->definirMultiplicador(novoMult);
        std::cout << "[HABILIDADE]: Furia acumulada! Multiplicador agora e x" << novoMult << ".\n";
        std::cout << "(" << u->obterTurnosBuff() - 1 << " turnos restantes).\n";
    }
}

std::string ClasseGuerreiro::obterNomeHabilidadeClasseAtiva() const { return "Determinacao no combate"; }
std::string ClasseGuerreiro::obterDescricaoHabilidadeClasseAtiva() const { 
    return "Gasta seu turno para aumentar seus atributos em 1.5x por 3 turnos."; 
}