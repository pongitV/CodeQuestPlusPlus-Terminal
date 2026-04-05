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

void Menu::configurarTelaCheia() 
{
#ifdef _WIN32
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_MAXIMIZE);
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

void Menu::digitar(std::string texto, int velocidade) 
{
    for (char c : texto) 
    {
        std::cout << c << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(velocidade));
    }
}

Personagem* Menu::criarPersonagem() 
{
    std::string nome = "";
    RacaBase* racaFinal = nullptr;
    ClasseBase* classeFinal = nullptr;
    int etapa = 1; 

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
            if (escolha == 1)      tempRaca = new RacaDwarf();
            else if (escolha == 2) tempRaca = new RacaElfo();
            else if (escolha == 3) tempRaca = new RacaHumano();
            else if (escolha == 4) tempRaca = new RacaOrk();

            if (tempRaca) 
            {
                limparTela();
                exibirLogo();
                int larguraTerminal = obterLarguraTerminal();
                std::cout << std::string(larguraTerminal, '-') << "\n";
                std::cout << " PREVIA DA RACA: " << tempRaca->obterNomeRaca() << "\n";
                std::cout << std::string(larguraTerminal, '-') << "\n\n";

                // Informacoes da Raca com todos os atributos
                Atributos stats = tempRaca->obterAtributosRaca();
                std::vector<std::string> info;
                info.push_back("[ ATRIBUTOS BASE DE RAÇA ]");
                
                auto addStat = [&](std::string n, int v) {
                    std::string sinal = (v >= 0) ? "+" : "";
                    info.push_back(" - " + n + ": " + sinal + std::to_string(v));
                };

                addStat("Vida", stats.vida);
                addStat("Forca", stats.forca);
                addStat("Destreza", stats.destreza);
                addStat("Resistencia", stats.resistencia);
                addStat("Constituicao", stats.constituicao);
                addStat("Inteligencia", stats.inteligencia);
                addStat("Sabedoria", stats.sabedoria);

                info.push_back("");
                info.push_back("[ HABILIDADE PASSIVA ]");
                info.push_back(" " + tempRaca->obterNomeHabilidadeRaca());
                info.push_back(" - " + tempRaca->obterDescricaoHabilidadeRaca());

                std::vector<std::string> arte = tempRaca->obterAparenciaRaca();
                
                int larguraArte = 0;
                for (const std::string& l : arte) if ((int)l.length() > larguraArte) larguraArte = (int)l.length();
                int larguraInfo = 40, gap = 6;
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
                int confirma; std::cin >> confirma;
                if (confirma == 1) { racaFinal = tempRaca; etapa = 3; }
                else { delete tempRaca; }
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
            if (escolha == 1)      temp = new ClasseArqueiro();
            else if (escolha == 2) temp = new ClasseBardo();
            else if (escolha == 3) temp = new ClasseGuerreiro();
            else if (escolha == 4) temp = new ClasseMago();

            if (temp) 
            {
                limparTela();
                exibirLogo();
                int larguraTerminal = obterLarguraTerminal();
                std::cout << std::string(larguraTerminal, '-') << "\n";
                std::cout << " PREVIA DA CLASSE: " << temp->obterNomeClasse() << "\n";
                std::cout << std::string(larguraTerminal, '-') << "\n\n";

                // Atributos da Classe com todos os valores
                Atributos stats = temp->obterAtributosClasse();
                std::vector<std::string> listaStats;
                listaStats.push_back("[ ATRIBUTOS BONUS DE CLASSE ]");
                
                auto addStat = [&](std::string n, int v) {
                    std::string sinal = (v >= 0) ? "+" : "";
                    listaStats.push_back(" - " + n + ": " + sinal + std::to_string(v));
                };

                addStat("Vida", stats.vida);
                addStat("Forca", stats.forca);
                addStat("Destreza", stats.destreza);
                addStat("Resistencia", stats.resistencia);
                addStat("Constituicao", stats.constituicao);
                addStat("Inteligencia", stats.inteligencia);
                addStat("Sabedoria", stats.sabedoria);

                listaStats.push_back("");
                listaStats.push_back("[ HABILIDADE ATIVA ]");
                listaStats.push_back(" " + temp->obterNomeHabilidadeClasseAtiva());
                listaStats.push_back(" - " + temp->obterDescricaoHabilidadeClasseAtiva());

                std::vector<std::string> arte = temp->obterAparenciaClasseMenu();
                
                int larguraArte = 0;
                for (const std::string& l : arte) if ((int)l.length() > larguraArte) larguraArte = (int)l.length();
                
                int larguraStatsCol = 25;
                for (const std::string& s : listaStats) if ((int)s.length() > larguraStatsCol) larguraStatsCol = (int)s.length();
                int gap = 4;
                int recuo = (larguraTerminal - (larguraStatsCol + gap + larguraArte)) / 2;
                if (recuo < 0) recuo = 0;

                size_t maxLinhas = std::max(arte.size(), listaStats.size());
                for (size_t i = 0; i < maxLinhas; ++i) 
                {
                    std::cout << std::string(recuo, ' ');
                    if (i < listaStats.size()) std::cout << std::left << std::setw(larguraStatsCol) << listaStats[i];
                    else std::cout << std::string(larguraStatsCol, ' ');
                    std::cout << std::string(gap, ' ');
                    if (i < arte.size()) std::cout << arte[i];
                    std::cout << "\n";
                }

                // Equipamento
                std::vector<Item*> kit = temp->gerarKitInicial();
                std::map<std::string, int> contagem;
                for (Item* i : kit) contagem[i->obterNomeItem()]++;

                std::cout << "\n" << std::string(recuo, ' ') << "[ EQUIPAMENTO INICIAL ]\n";
                for (auto const& [nomeI, qtd] : contagem) 
                    std::cout << std::string(recuo, ' ') << " - " << qtd << "x " << nomeI << "\n";

                for (Item* i : kit) delete i;

                std::cout << "\n" << std::string(recuo, ' ') << "0. VOLTAR | 1. CONFIRMAR\n";
                std::cout << std::string(recuo, ' ') << "Escolha: ";
                int confirma; std::cin >> confirma;
                if (confirma == 1) { classeFinal = temp; etapa = 4; }
                else { delete temp; }
            }
        }
    }
    Personagem* p = new Personagem(nome, racaFinal, classeFinal);
    std::cout << "\n";
    digitar(" [SISTEMA]: Personagem criado com sucesso! Iniciando jornada...\n", 30);
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