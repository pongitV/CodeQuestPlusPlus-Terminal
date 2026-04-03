#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

#include "Menu.h"
#include "../Inventario/Item.h"
#include "../Sistema/GeradorInimigos.h"

#include "../Raças/RacaBase.h"
#include "../Raças/RacaDwarf.h"
#include "../Raças/RacaElfo.h" 
#include "../Raças/RacaHumano.h"
#include "../Raças/RacaOrk.h"

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
    std::cin.clear(); 
    if (std::cin.peek() == '\n') std::cin.ignore();
    std::cin.ignore(std::cin.rdbuf()->in_avail(), '\n');
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

    RacaBase* racaFinal = nullptr;
    while (racaFinal == nullptr) 
    {
        limparTela();
        exibirLogo();
        std::cout << "Ola, " << nome << ". Escolha sua raca: \n";
        std::cout << "1. Dwarf | 2. Elfo | 3. Humano | 4. Ork\nEscolha: ";
        
        if (!(std::cin >> escolha)) 
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (escolha == 1) racaFinal = new RacaDwarf();
        else if (escolha == 2) racaFinal = new RacaElfo();
        else if (escolha == 3) racaFinal = new RacaHumano();
        else if (escolha == 4) racaFinal = new RacaOrk();
    }

    ClasseBase* classeFinal = nullptr;
    while (classeFinal == nullptr) 
    {
        limparTela();
        exibirLogo();
        std::cout << "Escolha sua classe: \n";
        std::cout << "1. Arqueiro | 2. Bardo | 3. Guerreiro | 4. Mago\nEscolha: ";
        
        if (!(std::cin >> escolha)) 
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        ClasseBase* temp = nullptr;
        if (escolha == 1) temp = new ClasseArqueiro();
        else if (escolha == 2) temp = new ClasseBardo();
        else if (escolha == 3) temp = new ClasseGuerreiro();
        else if (escolha == 4) temp = new ClasseMago();

        if (temp) 
        {
            limparTela();
            exibirLogo();
            std::cout << "--- VISUAL DA CLASSE: " << temp->obterNomeClasse() << " ---\n\n";
            
            // Usa o desenho épico para a seleção
            for (const std::string& linha : temp->obterAparenciaClasseMenu()) 
            {
                std::cout << linha << "\n";
            }

            std::cout << "\nConfirmar escolha? ( Sim(1) / Nao(2) ): ";
            int confirma;
            std::cin >> confirma;
            if (confirma == 1) classeFinal = temp;
            else delete temp;
        }
    }

    Personagem* p = new Personagem(nome, racaFinal, classeFinal);
    std::cout << "\n[SISTEMA]: Personagem criado com sucesso!\n";
    esperar();
    return p;
}

void Menu::exibirInventario(Personagem* p) 
{
    if (p == nullptr) return;
    limparTela();
    p->obterInventario()->listarItens(p->obterArma(), p->obterEscudo(), p->obterArmadura()); 
}

void Menu::exibirStatusJogador(Personagem* p) 
{
    if (p == nullptr) return;

    int largura = obterLarguraTerminal();
    std::string arma = (p->obterArma()) ? p->obterArma()->obterNomeItem() : "Punhos";
    std::string escu = (p->obterEscudo()) ? p->obterEscudo()->obterNomeItem() : "Nenhum";
    std::string dura = (p->obterArmadura()) ? p->obterArmadura()->obterNomeItem() : "Trapos";
    int qtdPocoes = p->obterInventario()->contarItensPorNome("Pocao de Cura (30%)");
    
    // CORREÇÃO: Usa obterAparenciaClasse (stick figure) para o combate
    std::vector<std::string> corpo = p->obterClasse()->obterAparenciaClasseMenu();

    std::cout << std::string(largura, '=') << "\n";
    
    // AJUSTE: Formato (Raça / Classe)
    std::cout << "| " << corpo[0] << " |  HEROI: " << p->obterNome() 
              << " (" << p->obterRaca()->obterNomeRaca() << " / " << p->obterNomeClasse() << ")\n";
    
    std::cout << "| " << corpo[1] << " |  HP: " << p->obterVida() << "/" << p->obterVidaMaxima() 
              << " | OURO: " << p->obterInventario()->obterOuro() << "G\n";
    
    std::cout << "| " << corpo[2] << " |  EQUIP: " << arma << " | " << escu << " | " << dura << "\n";
    std::cout << "| " << corpo[3] << " |  CURA DISPONIVEL: " << qtdPocoes << "x Pocao de Cura (30%)\n";
    
    std::cout << std::string(largura, '=') << "\n";
}

void Menu::exibirHorda(const std::vector<Personagem*>& inimigos) 
{
    if (inimigos.empty()) return;

    int larguraTerminal = obterLarguraTerminal();
    std::vector<std::string> arte = GeradorInimigos::obterGoblinASCII();
    
    int numInimigos = static_cast<int>(inimigos.size());
    int larguraColuna = larguraTerminal / numInimigos; 

    std::cout << std::string(larguraTerminal, '-') << "\n";

    for (size_t i = 0; i < inimigos.size(); i++) 
    {
        std::string identificador = inimigos[i]->obterNome() + " [" + std::to_string(i) + "]";
        int espacosEsq = (larguraColuna - static_cast<int>(identificador.length())) / 2;
        std::cout << std::string(espacosEsq > 0 ? espacosEsq : 0, ' ') 
                  << std::left << std::setw(larguraColuna - espacosEsq) << identificador;
    }
    std::cout << "\n";

    for (size_t i = 0; i < inimigos.size(); i++) 
    {
        std::string hp = "HP: " + std::to_string(inimigos[i]->obterVida()) + "/" + std::to_string(inimigos[i]->obterVidaMaxima());
        int espacosEsq = (larguraColuna - static_cast<int>(hp.length())) / 2;
        std::cout << std::string(espacosEsq > 0 ? espacosEsq : 0, ' ') 
                  << std::left << std::setw(larguraColuna - espacosEsq) << hp;
    }
    std::cout << "\n\n";

    for (size_t i = 0; i < arte.size(); i++) 
    {
        for (size_t j = 0; j < inimigos.size(); j++) 
        {
            int espacosEsq = (larguraColuna - static_cast<int>(arte[i].length())) / 2;
            std::cout << std::string(espacosEsq > 0 ? espacosEsq : 0, ' ') 
                      << std::left << std::setw(larguraColuna - espacosEsq) << arte[i];
        }
        std::cout << "\n";
    }
    std::cout << std::string(larguraTerminal, '-') << "\n";
}