#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <iomanip>

// Ajustar o logo ao tamanho do terminal
#ifdef _WIN32
    #include <windows.h> // Para Windows
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

// Menu e Inventário
#include "Menu.h"
#include "../Inventario/Item.h"
#include "../Sistema/GeradorInimigos.h"

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

int Menu::obterLarguraTerminal() 
{
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
#else
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
#endif
}

void Menu::exibirLogo() 
{
    int largura = obterLarguraTerminal();
    
    // Titulo ASCII
    std::vector<std::string> titulo = 
    {
        "  ____ ___  ____  _____    ___  _   _ _____ ____ _____ ",
        " / ___/ _ \\|  _ \\| ____|  / _ \\| | | | ____/ ___|_   _|",
        "| |  | | | | | | |  _|   | | | | | | |  _| \\___ \\ | |  ",
        "| |__| |_| | |_| | |___  | |_| | |_| | |___ ___) || |  ",
        " \\____\\___/|____/|_____|  \\__\\_\\\\__,_|_____|____/ |_|  "
    };

    std::cout << std::string(largura, '=') << "\n";

    for (const std::string& linha : titulo)
    {
        int espacos = (largura - static_cast<int>(linha.length())) / 2;
        if (espacos > 0) std::cout << std::string(espacos, ' ');
        std::cout << linha << "\n";
    }

    std::cout << "\n" << std::string(largura, '=') << "\n";
}

void Menu::limparTela() 
{
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
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

    // Cria o Personagem
    Personagem* p = new Personagem(nome, racaFinal, classeFinal);

    std::cout << "\n[SISTEMA]: Personagem criado e equipamentos de classe atribuidos!\n";
    esperar();

    return p;
}

void Menu::exibirInventario(Personagem* p) 
{
    if (p == nullptr) return;
    limparTela();
    p->obterInventario()->listarItens(p->obterArma(), p->obterEscudo(), p->obterArmadura()); 
    esperar();
}

void Menu::exibirHorda(const std::vector<Personagem*>& inimigos) 
{
    if (inimigos.empty()) return;

    // Obtemos a arte do Gerador de Inimigos
    std::vector<std::string> arte = GeradorInimigos::obterGoblinASCII();
    
    // Definimos a largura da coluna baseada na arte (aprox. 15 caracteres)
    int larguraColuna = 18; 

    std::cout << "\n" << std::string(larguraColuna * inimigos.size(), '-') << "\n";

    // 1. Linha de Nomes e Números (ex: Goblin [0])
    for (size_t i = 0; i < inimigos.size(); i++) 
    {
        std::string identificador = inimigos[i]->obterNome() + " [" + std::to_string(i) + "]";
        std::cout << std::left << std::setw(larguraColuna) << identificador;
    }
    std::cout << "\n";

    // 2. Linha de Vida (ex: HP: 50/50)
    for (size_t i = 0; i < inimigos.size(); i++) 
    {
        std::string hp = "HP: " + std::to_string(inimigos[i]->obterVida()) + "/" + std::to_string(inimigos[i]->obterVidaMaxima());
        std::cout << std::left << std::setw(larguraColuna) << hp;
    }
    std::cout << "\n\n";

    // 3. Impressão da Arte ASCII linha por linha para todos os inimigos
    for (size_t i = 0; i < arte.size(); i++) 
    {
        for (size_t j = 0; j < inimigos.size(); j++) 
        {
            std::cout << std::left << std::setw(larguraColuna) << arte[i];
        }
        std::cout << "\n";
    }
    
    std::cout << std::string(larguraColuna * inimigos.size(), '-') << "\n" << std::endl;
}