#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <map> 
#include <chrono>
#include <thread>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

#include "Menu.h"
#include "../Inventario/Item.h"
#include "../Sistema/GeradorInimigos.h"

// Inclusao das Racas
#include "../Raças/RacaBase.h"
#include "../Raças/RacaDwarf.h"
#include "../Raças/RacaElfo.h" 
#include "../Raças/RacaHumano.h"
#include "../Raças/RacaOrk.h"

// Inclusao das Classes
#include "../Classes/ClasseBase.h"
#include "../Classes/ClasseArqueiro.h"
#include "../Classes/ClasseBardo.h"
#include "../Classes/ClasseGuerreiro.h"
#include "../Classes/ClasseMago.h"

void Menu::configurarTelaCheia() 
{
#ifdef _WIN32
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_MAXIMIZE);
#endif
}

int Menu::obterLarguraTerminal() 
{
    int largura = 120; // Tamanho padrão de fallback

#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        largura = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
#endif
    return largura;
}

void Menu::exibirLogo() 
{
#ifdef _WIN32
    // Configura o console para aceitar blocos UTF-8
    SetConsoleOutputCP(65001); 
#endif

    // Chama a nossa nova função para pegar o tamanho exato da tela do notebook/PC atual
    int larguraConsole = obterLarguraTerminal();
    
    // A arte tem exatamente 144 caracteres visuais de largura
    const int larguraArte = 144; 
    
    // Calcula o padding dinâmico
    int espacosPad = (larguraConsole > larguraArte) ? (larguraConsole - larguraArte) / 2 : 0;
    std::string padding(espacosPad, ' ');

    std::vector<std::string> logo = 
    {
        "   █████████     ███████    ██████████   ██████████       ██████    █████  █████ ██████████  █████████  ███████████                         ",
        "  ███░░░░░███  ███░░░░░███ ░░███░░░░███ ░░███░░░░░█     ███░░░░███ ░░███  ░░███ ░░███░░░░░█ ███░░░░░███░█░░░███░░░█     ███         ███     ",
        " ███     ░░░  ███     ░░███ ░███   ░░███ ░███  █ ░     ███    ░░███ ░███   ░███  ░███  █ ░ ░███    ░░░ ░   ░███  ░     ░███        ░███     ",
        "░███         ░███      ░███ ░███    ░███ ░██████      ░███     ░███ ░███   ░███  ░██████   ░░█████████     ░███     ███████████ ███████████ ",
        "░███         ░███      ░███ ░███    ░███ ░███░░█      ░███   ██░███ ░███   ░███  ░███░░█    ░░░░░░░░███    ░███      ░░░███░░░ ░░░░░███░░░  ",
        "░░███     ███░░███     ███  ░███    ███  ░███ ░   █   ░░███ ░░████  ░███   ░███  ░███ ░   █ ███    ░███    ░███        ░███        ░███     ",
        " ░░█████████  ░░░███████░   ██████████   ██████████    ░░░██████░██ ░░████████   ██████████░░█████████     █████       ░░░         ░░░      ",
        "  ░░░░░░░░░     ░░░░░░░    ░░░░░░░░░░   ░░░░░░░░░░      ░░░░░░ ░░   ░░░░░░░░   ░░░░░░░░░░  ░░░░░░░░░     ░░░░░                              "
    };

    std::cout << "\n";
    
    // Desenha a linha superior preenchendo a tela inteira baseada na função
    std::cout << std::string(larguraConsole, '=') << "\n\n";

    // Imprime a logo centralizada
    for (size_t i = 0; i < logo.size(); ++i) 
    {
        std::cout << padding << logo[i] << "\n";
    }

    std::cout << "\n";
    
    // Desenha a linha inferior █
    std::cout << std::string(larguraConsole, '=') << "\n\n";
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

void Menu::digitar(const std::string& texto, int velocidade) 
{
    for (char c : texto) 
    {
        std::cout << c << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(velocidade));
    }
}

bool Menu::exibirPreviaLadoALado(const std::string& tipo, const std::string& nome, const std::vector<std::string>& info, const std::vector<std::string>& arte) 
{
    limparTela();
    exibirLogo();
    int larguraTerminal = obterLarguraTerminal();
    std::cout << std::string(larguraTerminal, '-') << "\n";
    std::cout << " PREVIA DA " << tipo << ": " << nome << "\n";
    std::cout << std::string(larguraTerminal, '-') << "\n\n";

    int larguraArte = 0;
    for (const std::string& l : arte) if ((int)l.length() > larguraArte) larguraArte = (int)l.length();
    
    int larguraInfo = 40;
    for (const std::string& s : info) if ((int)s.length() > larguraInfo) larguraInfo = (int)s.length();
    
    int gap = 6;
    int recuo = (larguraTerminal - (larguraInfo + gap + larguraArte)) / 2;
    if (recuo < 0) recuo = 0;

    size_t maxL = std::max(arte.size(), info.size());
    for (size_t i = 0; i < maxL; ++i) 
    {
        std::cout << std::string(recuo, ' ');
        if (i < info.size()) std::cout << std::left << std::setw(larguraInfo) << info[i];
        else std::cout << std::string(larguraInfo, ' ');
        std::cout << std::string(gap, ' ');
        if (i < arte.size()) std::cout << arte[i];
        std::cout << "\n";
    }

    std::cout << "\n" << std::string(recuo, ' ') << "0. VOLTAR | 1. CONFIRMAR\n";
    std::cout << std::string(recuo, ' ') << "Escolha: ";
    int confirma; 
    if (!(std::cin >> confirma)) { std::cin.clear(); std::cin.ignore(1000, '\n'); return false; }
    return confirma == 1;
}

Personagem* Menu::criarPersonagem() 
{
    std::string nome = "";
    RacaBase* racaFinal = nullptr;
    ClasseBase* classeFinal = nullptr;
    int etapa = 1; 

    auto formatarAtributo = [](const std::string& n, int v) {
        return " - " + n + ": " + (v >= 0 ? "+" : "") + std::to_string(v);
    };

    while (etapa <= 3) 
    {
        if (etapa == 1) // --- ETAPA 1: NOME ---
        {
            limparTela();
            exibirLogo();
            std::cout << "\n";
            digitar(" [NARRACAO]: O reino clama por um novo destino...\n", 40);
            digitar(" [NARRACAO]: E todas lendas possuem um nome.\n\n", 40);
            std::cout << " > Escolha o nome do seu personagem (ou '0' para sair): ";
            
            std::cin.ignore(std::cin.rdbuf()->in_avail(), '\n');
            std::getline(std::cin, nome);

            if (nome == "0") exit(0);
            if (!nome.empty()) etapa = 2;
        }
        else if (etapa == 2) // --- ETAPA 2: RACA ---
        {
            limparTela();
            exibirLogo();
            std::cout << " JOGADOR: " << nome << "\n";
            std::cout << std::string(obterLarguraTerminal(), '-') << "\n";
            digitar(" [NARRACAO]: Qual sua origem?\n\n", 40);
            
            std::cout << "  [1] Dwarf\n";
            std::cout << "  [2] Elfo\n";
            std::cout << "  [3] Humano\n";
            std::cout << "  [4] Ork\n";
            std::cout << "\n  [0] VOLTAR (selecao de nome)\n";
            std::cout << "\n > Sua escolha: ";
            
            int escolha;
            if (!(std::cin >> escolha)) { std::cin.clear(); std::cin.ignore(1000, '\n'); continue; }
            if (escolha == 0) { etapa = 1; continue; }

            RacaBase* tempRaca = nullptr;
            switch(escolha) {
                case 1: tempRaca = new RacaDwarf(); break;
                case 2: tempRaca = new RacaElfo();  break;
                case 3: tempRaca = new RacaHumano(); break;
                case 4: tempRaca = new RacaOrk();   break;
            }

            if (tempRaca) 
            {
                Atributos stats = tempRaca->obterAtributosRaca();
                std::vector<std::string> info = {
                    "[ ATRIBUTOS BASE DE RAÇA ]",
                    formatarAtributo("Vida", stats.vida),
                    formatarAtributo("Forca", stats.forca),
                    formatarAtributo("Destreza", stats.destreza),
                    formatarAtributo("Resistencia", stats.resistencia),
                    formatarAtributo("Constituicao", stats.constituicao),
                    formatarAtributo("Inteligencia", stats.inteligencia),
                    formatarAtributo("Sabedoria", stats.sabedoria),
                    "",
                    "[ HABILIDADE PASSIVA ]",
                    " " + tempRaca->obterNomeHabilidadeRaca(),
                    " - " + tempRaca->obterDescricaoHabilidadeRaca()
                };
                
                if (exibirPreviaLadoALado("RACA", tempRaca->obterNomeRaca(), info, tempRaca->obterAparenciaRaca())) {
                    racaFinal = tempRaca; 
                    etapa = 3;
                } else { 
                    delete tempRaca; 
                }
            }
        }
        else if (etapa == 3) // --- ETAPA 3: CLASSE ---
        {
            limparTela();
            exibirLogo();
            std::cout << " JOGADOR: " << nome << " | RACA: " << racaFinal->obterNomeRaca() << "\n";
            std::cout << std::string(obterLarguraTerminal(), '-') << "\n";
            digitar(" [NARRACAO]: Qual caminho voce seguira neste mundo?\n\n", 40);
            
            std::cout << "  [1] Arqueiro\n";
            std::cout << "  [2] Bardo\n";
            std::cout << "  [3] Guerreiro\n";
            std::cout << "  [4] Mago\n";
            std::cout << "\n  [0] VOLTAR (selecao de raca)\n";
            std::cout << "\n > Sua escolha: ";
            
            int escolha;
            if (!(std::cin >> escolha)) { std::cin.clear(); std::cin.ignore(1000, '\n'); continue; }
            if (escolha == 0) { delete racaFinal; racaFinal = nullptr; etapa = 2; continue; }

            ClasseBase* temp = nullptr;
            switch(escolha) {
                case 1: temp = new ClasseArqueiro(); break;
                case 2: temp = new ClasseBardo(); break;
                case 3: temp = new ClasseGuerreiro(); break;
                case 4: temp = new ClasseMago(); break;
            }

            if (temp) 
            {
                Atributos stats = temp->obterAtributosClasse();
                std::vector<std::string> info = {
                    "[ ATRIBUTOS BONUS DE CLASSE ]",
                    formatarAtributo("Vida", stats.vida),
                    formatarAtributo("Forca", stats.forca),
                    formatarAtributo("Destreza", stats.destreza),
                    formatarAtributo("Resistencia", stats.resistencia),
                    formatarAtributo("Constituicao", stats.constituicao),
                    formatarAtributo("Inteligencia", stats.inteligencia),
                    formatarAtributo("Sabedoria", stats.sabedoria),
                    "",
                    "[ HABILIDADE ATIVA ]",
                    " " + temp->obterNomeHabilidadeClasseAtiva(),
                    " - " + temp->obterDescricaoHabilidadeClasseAtiva(),
                    "",
                    "[ EQUIPAMENTO INICIAL ]"
                };
                
                std::vector<Item*> kit = temp->gerarKitInicial();
                std::map<std::string, int> contagem;
                for (Item* i : kit) contagem[i->obterNomeItem()]++;
                for (auto const& [nomeI, qtd] : contagem) info.push_back(" - " + std::to_string(qtd) + "x " + nomeI);
                for (Item* i : kit) delete i;

                if (exibirPreviaLadoALado("CLASSE", temp->obterNomeClasse(), info, temp->obterAparenciaClasseMenu())) {
                    classeFinal = temp; 
                    etapa = 4;
                } else { 
                    delete temp; 
                }
            }
        }
    }
    Personagem* p = new Personagem(nome, racaFinal, classeFinal);
    std::cout << "\n";
    digitar(" [SISTEMA]: Personagem criado com sucesso! Iniciando jornada...\n", 40);
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
    
    std::vector<std::string> corpo = p->obterRaca()->obterAparenciaRaca();
    std::cout << std::string(largura, '=') << "\n";
    std::cout << "| " << corpo[0] << " |  JOGADOR: " << p->obterNome() << " (" << p->obterRaca()->obterNomeRaca() << " / " << p->obterNomeClasse() << ")\n";
    std::cout << "| " << corpo[1] << " |  HP: " << p->obterVida() << "/" << p->obterVidaMaxima() << " | OURO: " << p->obterInventario()->obterOuro() << "G\n";
    std::cout << "| " << corpo[2] << " |  EQUIP: " << arma << " | " << escu << " | " << dura << "\n";
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
        std::string id = inimigos[i]->obterNome() + " [" + std::to_string(i) + "]";
        int esp = (larguraColuna - (int)id.length()) / 2;
        std::cout << std::string(esp > 0 ? esp : 0, ' ') << std::left << std::setw(larguraColuna - esp) << id;
    }
    std::cout << "\n";
    for (size_t i = 0; i < inimigos.size(); i++) 
    {
        std::string hp = "HP: " + std::to_string(inimigos[i]->obterVida()) + "/" + std::to_string(inimigos[i]->obterVidaMaxima());
        int esp = (larguraColuna - (int)hp.length()) / 2;
        std::cout << std::string(esp > 0 ? esp : 0, ' ') << std::left << std::setw(larguraColuna - esp) << hp;
    }
    std::cout << "\n\n";
    for (size_t i = 0; i < arte.size(); i++) 
    {
        for (size_t j = 0; j < inimigos.size(); j++) 
        {
            int esp = (larguraColuna - (int)arte[i].length()) / 2;
            std::cout << std::string(esp > 0 ? esp : 0, ' ') << std::left << std::setw(larguraColuna - esp) << arte[i];
        }
        std::cout << "\n";
    }
    std::cout << std::string(larguraTerminal, '-') << "\n";
}