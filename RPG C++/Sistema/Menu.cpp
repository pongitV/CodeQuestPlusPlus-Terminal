#include <iostream>
#include <limits>
#include "Menu.h"
#include "../Inventario/Item.h"

// Racas
#include "../Raças/RacaBase.h"
#include "../Raças/RacaDwarf.h"
#include "../Raças/RacaElfo.h" 
#include "../Raças/RacaHumano.h"
#include "../Raças/RacaOrk.h"

// Classes
#include "../Classes/ClasseBase.h"
#include "../Classes/ClasseArqueiro.h"
#include "../Classes/ClasseBardo.h"
#include "../Classes/ClasseGuerreiro.h"
#include "../Classes/ClasseMago.h"


void Menu::limparTela() 
{
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void Menu::exibirLogo() 
{
    std::cout << "================================================\n";
    std::cout << "                RPG: xxxxxxxxxxx                \n";
    std::cout << "================================================\n";
}

// Certifique-se de que esta função está no final do arquivo!
void Menu::exibirInventario(Personagem* p) 
{
    if (p == nullptr) return;
    limparTela();
    p->obterInventario()->listarItens(p->obterArma(), p->obterEscudo(), p->obterArmadura()); 
    esperar();
}

void Menu::esperar() 
{
    std::cout << "\nPressione Enter para continuar...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

Personagem* Menu::criarPersonagem() 
{
    std::string nome;
    int escolha = 0;

    limparTela();
    exibirLogo();
    std::cout << "Digite o nome do seu heroi: ";
    std::getline(std::cin, nome);

    // Seleção de Raça
    RacaBase* racaFinal = nullptr;
    while (racaFinal == nullptr) 
    {
        limparTela();
        exibirLogo();
        std::cout << "Ola, " << nome << ". Escolha sua raca: \n";
        std::cout << "1. Dwarf \n2. Elfo\n3. Humano\n4. Ork\nEscolha: ";
        
        if (!(std::cin >> escolha)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (escolha == 1) racaFinal = new RacaDwarf();
        else if (escolha == 2) racaFinal = new RacaElfo();
        else if (escolha == 3) racaFinal = new RacaHumano();
        else if (escolha == 4) racaFinal = new RacaOrk();
        else std::cout << "Opcao invalida\n";
    }

    // Seleção de Classe
    ClasseBase* classeFinal = nullptr;
    while (classeFinal == nullptr) 
    {
        limparTela();
        exibirLogo();
        std::cout << "Escolha sua classe: \n";
        std::cout << "1. Arqueiro\n2. Bardo\n3. Guerreiro\n4. Mago\nEscolha: ";
        
        if (!(std::cin >> escolha)) 
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (escolha == 1) classeFinal = new ClasseArqueiro();
        else if (escolha == 2) classeFinal = new ClasseBardo();
        else if (escolha == 3) classeFinal = new ClasseGuerreiro();
        else if (escolha == 4) classeFinal = new ClasseMago();
        else std::cout << "Opcao invalida\n";
    }

    // Instancia o Personagem
    Personagem* p = new Personagem(nome, racaFinal, classeFinal);

    std::cout << "\n[SISTEMA]: Personagem criado e equipamentos de classe atribuidos!\n";
    esperar();

    return p;
}
