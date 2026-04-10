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
    #include <conio.h>
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
#ifdef _WIN32
    // Limpa teclas residuais pressionadas antes da hora e aguarda estritamente pelo Enter (ASCII 13)
    while (_kbhit()) _getch();
    while (_getch() != 13) {}
#else
    std::cin.clear(); 
    if (std::cin.rdbuf()->in_avail() > 0) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::string temp;
    std::getline(std::cin, temp);
#endif
}

void Menu::digitar(const std::string& texto, int velocidade)
{
    // 1. Salva a posição atual do cursor para não perder o lugar do texto
    // 2. Move para a linha 24, coluna 1 (canto inferior esquerdo)
    // 3. Imprime a mensagem em cinza para não distrair
    // 4. Volta para a posição original
    std::cout << "\033[s\033[80;1H\033[1;90m[Pressione 'k' para pular]\033[u";

    for (size_t i = 0; i < texto.length(); ++i)
    {
        // Verifica se o usuário pressionou a tecla 'k'
        #ifdef _WIN32
            if (_kbhit()) {
                char tecla = _getch();
                if (tecla == 'k' || tecla == 'K') {
                    // Limpa a linha do "pular" antes de sair para não deixar lixo
                    std::cout << "\033[s\033[24;1H\033[K\033[u";
                    
                    // Pula o restante do texto
                    std::cout << texto.substr(i) << std::flush;
                    return; 
                }
            }
        #endif

        // Imprime o caractere atual do diálogo
        std::cout << texto[i] << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(velocidade));
    }
    std::cout << std::endl;
}

bool Menu::exibirPreviaLadoALado(const std::string& tipo, const std::string& nome, const std::vector<std::string>& info, const std::vector<std::string>& arte) 
{
    limparTela();
    exibirLogo("PREVIA DA " + tipo + ": " + nome);
    int larguraTerminal = obterLarguraTerminal();

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

void Menu::imprimirBlocoCentralizado(const std::vector<std::string>& linhas, int larguraVisual, const std::string& cor) 
{
    int larguraConsole = obterLarguraTerminal();
    
    if (larguraVisual <= 0) {
        for (const std::string& l : linhas) {
            if ((int)l.length() > larguraVisual) larguraVisual = (int)l.length();
        }
    }
    
    int espacosPad = (larguraConsole > larguraVisual) ? (larguraConsole - larguraVisual) / 2 : 0;
    std::string padding(espacosPad > 0 ? espacosPad : 0, ' ');

    for (const std::string& linha : linhas) 
    {
        std::cout << padding << (cor.empty() ? "" : cor) << linha << (cor.empty() ? "" : "\033[0m") << "\n";
    }
}

Personagem* Menu::criarPersonagem() 
{
    std::string nome = "";
    RacaBase* racaFinal = nullptr;
    ClasseBase* classeFinal = nullptr;
    bool parryFinal = false;
    int dificuldadeFinal = 2;
    int etapa = 1; 

    auto formatarAtributo = [](const std::string& n, int v) {
        return " - " + n + ": " + (v >= 0 ? "+" : "") + std::to_string(v);
    };

    while (etapa <= 5) 
    {
        if (etapa == 1) // --- ETAPA 1: NOME ---
        {
            limparTela();
            exibirLogo("INTRODUCAO AO RPG");
            digitar(" [NARRACAO]: O reino clama por um novo destino...\n", 35);
            digitar(" [NARRACAO]: E todas lendas possuem um nome.\n\n", 35);
            std::cout << " > Escolha o nome do seu personagem (ou '0' para sair): ";
            
            std::cin.ignore(std::cin.rdbuf()->in_avail(), '\n');
            std::getline(std::cin, nome);

            if (nome == "0") exit(0);
            if (!nome.empty()) etapa = 2;
        }
        else if (etapa == 2) // --- ETAPA 2: RACA ---
        {
            limparTela();
            exibirLogo("SELECAO DE RACA");
            std::cout << "JOGADOR: " << nome << "\n";
            std::cout << std::string(obterLarguraTerminal(), '-') << "\n";
            digitar(" [NARRACAO]: Qual sua origem?\n\n", 35);
            
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
            exibirLogo("SELECAO DE CLASSE");
            std::cout << "JOGADOR: " << nome << " | RACA: " << racaFinal->obterNomeRaca() << "\n";
            std::cout << std::string(obterLarguraTerminal(), '-') << "\n";
            digitar(" [NARRACAO]: Qual caminho voce seguira neste mundo?\n\n", 35);
            
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
        else if (etapa == 4) // --- ETAPA 4: CONFIGURACOES DO JOGO ---
        {
            limparTela();
            exibirLogo("CONFIGURACOES DO JOGO");
            std::cout << "JOGADOR: " << nome << " | RACA: " << racaFinal->obterNomeRaca() << " | CLASSE: " << classeFinal->obterNomeClasse() << "\n";
            std::cout << std::string(obterLarguraTerminal(), '-') << "\n";
            digitar(" [SISTEMA]: Deseja ativar o sistema de PARRY?\n\n", 35);
            digitar(" (Permite reduzir danos ao digitar uma sequencia de numeros num tempo limite)\n", 35);
            
            std::cout << "  [1] LIGAR Parry\n";
            std::cout << "  [2] DESLIGAR Parry\n";
            std::cout << "\n  [0] VOLTAR (selecao de classe)\n";
            std::cout << "\n > Sua escolha: ";
            
            int escolha;
            if (!(std::cin >> escolha)) { std::cin.clear(); std::cin.ignore(1000, '\n'); continue; }
            if (escolha == 0) { delete classeFinal; classeFinal = nullptr; etapa = 3; continue; }

            if (escolha == 1 || escolha == 2) 
            {
                parryFinal = (escolha == 1);
                etapa = 5;
            }
        }
        else if (etapa == 5) // --- ETAPA 5: DIFICULDADE ---
        {
            limparTela();
            exibirLogo("DIFICULDADE DO MUNDO");
            std::cout << "JOGADOR: " << nome << " | RACA: " << racaFinal->obterNomeRaca() << " | CLASSE: " << classeFinal->obterNomeClasse() << "\n";
            std::cout << std::string(obterLarguraTerminal(), '-') << "\n";
            digitar(" [SISTEMA]: Escolha o nivel de desafio da sua jornada:\n\n", 35);
            
            std::cout << "  [1] FACIL   (Inimigos com 1x Atributos, sem habilidades de raca e sem classe)\n";
            std::cout << "  [2] NORMAL  (Inimigos com 1.5x Atributos, com habilidades de raca mas sem classes)\n";
            std::cout << "  [3] DIFICIL (Inimigos com 2x Atributos, com habilidades de raca e com classes)\n";
            std::cout << "\n  [0] VOLTAR (configuracao de parry)\n";
            std::cout << "\n > Sua escolha: ";
            
            int escolha;
            if (!(std::cin >> escolha)) { std::cin.clear(); std::cin.ignore(1000, '\n'); continue; }
            if (escolha == 0) { etapa = 4; continue; }

            if (escolha >= 1 && escolha <= 3) 
            {
                dificuldadeFinal = escolha;
                etapa = 6;
            }
        }
    }
    Personagem* p = new Personagem(nome, racaFinal, classeFinal);
    p->definirParryAtivado(parryFinal);
    p->definirDificuldade(dificuldadeFinal);
    std::cout << "\n";
    digitar(" [SISTEMA]: Personagem criado com sucesso! Iniciando jornada...\n", 35);
    esperar();
    return p;
}

void Menu::exibirStatusJogador(Personagem* p) 
{
    if (p == nullptr) return;
    int largura = obterLarguraTerminal();
    std::string arma = (p->obterArma()) ? p->obterArma()->obterNomeItem() : "Punhos";
    std::string escu = (p->obterEscudo()) ? p->obterEscudo()->obterNomeItem() : "Nenhum";
    std::string dura = (p->obterArmadura()) ? p->obterArmadura()->obterNomeItem() : "Trapos";
    
    double percVida = static_cast<double>(p->obterVida()) / p->obterVidaMaxima();
    std::string corVerde = "\033[32m";    // Verde
    std::string corLaranja = "\033[33m";  // Laranja
    std::string corVermelho = "\033[31m"; // Vermelho
    std::string corReset = "\033[0m";     // Reset
    
    // --- LÓGICA DE COR PARA O HP ---
    std::string corHP;
    if (percVida > 0.70)      corHP = corVerde;
    else if (percVida > 0.30) corHP = corLaranja;
    else                      corHP = corVermelho;
    // -------------------------------

    std::vector<std::string> coracao;
    
    if (percVida > 0.70) 
    {
        coracao = {
            "   _   _   ",
            "  / \\_/ \\  ",
            "  \\     /  ",
            "   \\___/   "
        };
        coracao[0] = corVerde + coracao[0] + corReset;
        coracao[1] = corVerde + coracao[1] + corReset;
        coracao[2] = corVerde + coracao[2] + corReset;
        coracao[3] = corVerde + coracao[3] + corReset;
    }
    else if (percVida > 0.30) 
    {
        coracao = {
            "   _   _   ",
            "  / \\// \\  ",
            "  \\  \\ /   ",
            "   \\___/   "
        };
        coracao[0] = corLaranja + coracao[0] + corReset;
        coracao[1] = corLaranja + coracao[1] + corReset;
        coracao[2] = corLaranja + coracao[2] + corReset;
        coracao[3] = corLaranja + coracao[3] + corReset;
    }
    else 
    {
        coracao = {
            "  _     _  ",
            " / \\   / \\ ",
            " \\     \\_/ ",
            "  \\___/    "
        };
        coracao[0] = corVermelho + coracao[0] + corReset;
        coracao[1] = corVermelho + coracao[1] + corReset;
        coracao[2] = corVermelho + coracao[2] + corReset;
        coracao[3] = corVermelho + coracao[3] + corReset;
    }
    
    std::string barraXp = "[";
    int tamanhoBarra = 10;
    int preenchido = (p->obterXpAtual() * tamanhoBarra) / p->obterXpParaSubir();
    if (preenchido > tamanhoBarra) preenchido = tamanhoBarra;
    if (preenchido > 0) barraXp += "\033[34m" + std::string(preenchido, '#') + "\033[0m";
    if (tamanhoBarra > preenchido) barraXp += std::string(tamanhoBarra - preenchido, '-');
    barraXp += "] \033[34m" + std::to_string(p->obterXpAtual()) + "\033[0m/" + std::to_string(p->obterXpParaSubir());
    
    // Aplicando a cor dinâmica ao HP na linha do status
    std::vector<std::string> linhas = 
    {
        "| " + coracao[0] + " |",
        "| " + coracao[1] + " |  JOGADOR: " + p->obterNome() + " (" + p->obterRaca()->obterNomeRaca() + " / " + p->obterNomeClasse() + ") | NIVEL: " + std::to_string(p->obterNivel()),
        "| " + coracao[2] + " |  HP: " + corHP + std::to_string(p->obterVida()) + corReset + "/" + std::to_string(p->obterVidaMaxima()) + " | OURO: \033[33m" + std::to_string(p->obterInventario()->obterOuro()) + "G\033[0m | XP: " + barraXp,
        "| " + coracao[3] + " |  EQUIP: " + arma + " | " + escu + " | " + dura,
        "| " + std::string(11, ' ') + " |"
    };

    int maxLen = 0;
    for (const std::string& linha : linhas) 
    {
        if ((int)linha.length() > maxLen) maxLen = (int)linha.length();
    }
    int espacos = (largura - maxLen) / 2;
    std::string margem(espacos > 0 ? espacos : 0, ' ');

    std::cout << std::string(largura, '=') << "\n";
    for (const std::string& linha : linhas) 
    {
        std::cout << margem << linha << "\n";
    }
    std::cout << std::string(largura, '=') << "\n";
}

void Menu::exibirHorda(const std::vector<Personagem*>& inimigos) 
{
    if (inimigos.empty()) return;
    int larguraTerminal = obterLarguraTerminal();
    // Pega a arte dinamicamente da raca do inimigo que esta sendo enfrentado
    std::vector<std::string> arte = inimigos[0]->obterRaca()->obterAparenciaRaca();
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
    std::cout << std::string(larguraTerminal, '-') << "\n\n";
}

void Menu::exibirLogo(const std::string& titulo) 
{
#ifdef _WIN32
    SetConsoleOutputCP(65001); 
#endif
    int larguraConsole = obterLarguraTerminal();
    
    // 1. SEU VETOR DE ASCII (Apenas o texto "CODE QUEST", sem o ++)
    std::vector<std::string> logoTexto = 
    {
        "   █████████     ███████    ██████████   ██████████       ██████    █████  █████ ██████████  █████████  ███████████  ",
        "  ███░░░░░███  ███░░░░░███ ░░███░░░░███ ░░███░░░░░█     ███░░░░███ ░░███  ░░███ ░░███░░░░░█ ███░░░░░███░█░░░███░░░█  ",
        " ███     ░░░  ███     ░░███ ░███   ░░███ ░███  █ ░     ███    ░░███ ░███   ░███  ░███  █ ░ ░███    ░░░ ░   ░███  ░   ",
        "░███         ░███      ░███ ░███    ░███ ░██████      ░███     ░███ ░███   ░███  ░██████   ░░█████████     ░███      ",
        "░███         ░███      ░███ ░███    ░███ ░███░░█      ░███   ██░███ ░███   ░███  ░███░░█    ░░░░░░░░███    ░███      ",
        "░░███     ███░░███     ███  ░███    ███  ░███ ░   █   ░░███ ░░████  ░███   ░███  ░███ ░   █ ███    ░███    ░███      ",
        " ░░█████████  ░░░███████░   ██████████   ██████████    ░░░██████░██ ░░████████   ██████████░░█████████     █████     ",
        "  ░░░░░░░░░     ░░░░░░░    ░░░░░░░░░░   ░░░░░░░░░░      ░░░░░░ ░░   ░░░░░░░░   ░░░░░░░░░░  ░░░░░░░░░     ░░░░░       "
    };

    // 2. SEU VETOR DE ASCII (Apenas o "++")
    std::vector<std::string> logoPlus = 
    {
       "                          ",
       "     ███         ███      ",
       "    ░███        ░███      ",
       " ███████████ ███████████  ",
       "░░░░░███░░░ ░░░░░███░░░   ",
       "    ░███        ░███      ",
       "    ░░░         ░░░       ",
       "                          "               
    };

    std::cout << "\n" << std::string(larguraConsole, '=') << "\n\n";

    // 3. LÓGICA DE IMPRESSÃO CENTRALIZADA COM CORES DIFERENTES
    // Calculamos a largura total da linha (Texto + Espaço + ++) para centralizar corretamente
    int larguraLinhaCompleta = 140; 

    for (size_t i = 0; i < logoTexto.size(); ++i) {
        // Cálculo de recuo para centralizar a linha inteira
        int recuo = (larguraConsole - larguraLinhaCompleta) / 2;
        if (recuo < 0) recuo = 0;
        std::cout << std::string(recuo, ' ');

        // Imprime a parte principal (Branco/Padrão)
        std::cout << logoTexto[i];

        // Imprime o ++ (Laranja)
        std::cout << "\x1b[38;5;208m" << logoPlus[i] << "\x1b[0m";

        std::cout << "\n";
    }

    std::cout << "\n";
    
    // 4. IMPRESSÃO DO TÍTULO (Igual ao seu original)
    if (titulo.empty()) {
        std::cout << std::string(larguraConsole, '=') << "\n\n";
    } else {
        std::cout << std::string(larguraConsole, '=') << "\n";
        int espacos = (larguraConsole - (int)titulo.length()) / 2;
        std::cout << std::string(espacos > 0 ? espacos : 0, ' ') << titulo << "\n";
        std::cout << std::string(larguraConsole, '=') << "\n\n";
    }
}

void Menu::exibirLogoCombate(const std::string& titulo) 
{
#ifdef _WIN32
    // Configura o console para aceitar blocos UTF-8
    SetConsoleOutputCP(65001); 
#endif

    int larguraConsole = obterLarguraTerminal();
    
    std::vector<std::string> logo = 
    {
        "   █████████     ███████    ██████   ██████ ███████████    █████████    ███████████ ██████████ ",
        "  ███░░░░░███  ███░░░░░███ ░░██████ ██████ ░░███░░░░░███  ███░░░░░███ ░█░░░███░░░█░░███░░░░░█ ",
        " ███     ░░░  ███     ░░███ ░███░█████░███  ░███    ░███ ░███    ░███ ░   ░███  ░  ░███  █ ░  ",
        "░███         ░███      ░███ ░███░░███ ░███  ░██████████  ░███████████     ░███     ░██████    ",
        "░███         ░███      ░███ ░███ ░░░  ░███  ░███░░░░░███ ░███░░░░░███     ░███     ░███░░█    ",
        "░░███     ███░░███     ███  ░███      ░███  ░███    ░███ ░███    ░███     ░███     ░███ ░   █ ",
        " ░░█████████  ░░░███████░   █████     █████ ███████████  █████   █████    █████    ██████████ ",
        "  ░░░░░░░░░     ░░░░░░░    ░░░░░     ░░░░░ ░░░░░░░░░░░  ░░░░░   ░░░░░    ░░░░░    ░░░░░░░░░░  "
    };

    std::cout << "\n";
    std::cout << std::string(larguraConsole, '=') << "\n\n";

    // Imprime a logo centralizada com cor Vermelha
    imprimirBlocoCentralizado(logo, 95, "\033[31m");

    std::cout << "\n";
    
    std::cout << std::string(larguraConsole, '=') << "\n";
    int espacos = (larguraConsole - (int)titulo.length()) / 2;
    std::cout << std::string(espacos > 0 ? espacos : 0, ' ') << titulo << "\n";
    std::cout << std::string(larguraConsole, '=') << "\n\n";
}

void Menu::exibirTelaVitoria(Personagem* p, int ouro, int xp, int danoCausado, int danoRecebido)
{
    limparTela();

    int largura = obterLarguraTerminal();

    std::vector<std::string> logoVitoria = 
    {
       " █████   █████ █████ ███████████    ███████    ███████████   █████   █████████   ███ ",
       "░░███   ░░███ ░░███ ░█░░░███░░░█  ███░░░░░███ ░░███░░░░░███ ░░███   ███░░░░░███ ░███ ",
       " ░███    ░███  ░███ ░   ░███  ░  ███     ░░███ ░███    ░███  ░███  ░███    ░███ ░███ ",
       " ░███    ░███  ░███     ░███    ░███      ░███ ░██████████   ░███  ░███████████ ░███ ",
       " ░░███   ███   ░███     ░███    ░███      ░███ ░███░░░░░███  ░███  ░███░░░░░███ ░███ ",
       "  ░░░█████░    ░███     ░███    ░░███     ███  ░███    ░███  ░███  ░███    ░███ ░░░  ",
       "    ░░███      █████    █████    ░░░███████░   █████   █████ █████ █████   █████ ███ ",
       "     ░░░      ░░░░░    ░░░░░       ░░░░░░░    ░░░░░   ░░░░░ ░░░░░ ░░░░░   ░░░░░ ░░░  "
    };

    std::cout << "\n";
    std::cout << std::string(largura, '=') << "\n\n";

    // Imprime a logo centralizada com cor Verde (largura visual corrigida para 85)
    imprimirBlocoCentralizado(logoVitoria, 85, "\033[32m");
    std::cout << "\n" << std::string(largura, '=') << "\n\n";

    std::vector<std::string> linhas = {
        "NOME:           " + p->obterNome(),
        "RACA:           " + p->obterRaca()->obterNomeRaca(),
        "CLASSE:         " + p->obterNomeClasse(),
        "HP RESTANTE:    " + std::to_string(p->obterVida()) + "/" + std::to_string(p->obterVidaMaxima()),
        "OURO TOTAL:     " + std::to_string(p->obterInventario()->obterOuro()) + "G",
        "NIVEL:          " + std::to_string(p->obterNivel()) + " (XP: " + std::to_string(p->obterXpAtual()) + "/" + std::to_string(p->obterXpParaSubir()) + ")",
        "",
        "--- ESTATISTICAS DA BATALHA ---",
        "OURO OBTIDO:   +" + std::to_string(ouro) + "G",
        "XP OBTIDO:     +" + std::to_string(xp) + " XP",
        "DANO CAUSADO:   " + std::to_string(danoCausado),
        "DANO RECEBIDO:  " + std::to_string(danoRecebido)
    };

    // Imprime as estatisticas centralizadas e verdes (largura automatica = 0)
    imprimirBlocoCentralizado(linhas, 0, "\033[32m");

    std::cout << "\n" << std::string(largura, '=') << "\n";

    esperar();
}

void Menu::exibirTelaDerrota(Personagem* p, int ouro, int xp, int danoCausado, int danoRecebido)
{
    limparTela();

    int largura = obterLarguraTerminal();

    std::vector<std::string> logoDerrota = 
    {
        " ██████████   ██████████ ███████████   ███████████      ███████    ███████████   █████████           ",
        "░░███░░░░███ ░░███░░░░░█░░███░░░░░███ ░░███░░░░░███   ███░░░░░███ ░█░░░███░░░█  ███░░░░░███          ",
        " ░███   ░░███ ░███  █ ░  ░███    ░███  ░███    ░███  ███     ░░███░   ░███  ░  ░███    ░███          ",
        " ░███    ░███ ░██████    ░██████████   ░██████████  ░███      ░███    ░███     ░███████████          ",
        " ░███    ░███ ░███░░█    ░███░░░░░███  ░███░░░░░███ ░███      ░███    ░███     ░███░░░░░███          ",
        " ░███    ███  ░███ ░   █ ░███    ░███  ░███    ░███ ░░███     ███     ░███     ░███    ░███          ",
        " ██████████   ██████████ █████   █████ █████   █████ ░░░███████░      █████    █████   █████ ██ ██ ██",
        "░░░░░░░░░░   ░░░░░░░░░░ ░░░░░   ░░░░░ ░░░░░   ░░░░░    ░░░░░░░       ░░░░░    ░░░░░   ░░░░░ ░░ ░░ ░░ "
    };

    std::cout << "\n";
    std::cout << std::string(largura, '=') << "\n\n";

    // Imprime a logo centralizada com cor Vermelha (largura visual corrigida para 101)
    imprimirBlocoCentralizado(logoDerrota, 101, "\033[31m");
    std::cout << "\n" << std::string(largura, '=') << "\n\n";

    std::vector<std::string> linhas = {
        "NOME:           " + p->obterNome(),
        "RACA:           " + p->obterRaca()->obterNomeRaca(),
        "CLASSE:         " + p->obterNomeClasse(),
        "HP RESTANTE:    " + std::to_string(p->obterVida()) + "/" + std::to_string(p->obterVidaMaxima()),
        "OURO TOTAL:     " + std::to_string(p->obterInventario()->obterOuro()) + "G",
        "NIVEL:          " + std::to_string(p->obterNivel()) + " (XP: " + std::to_string(p->obterXpAtual()) + "/" + std::to_string(p->obterXpParaSubir()) + ")",
        "",
        "--- ESTATISTICAS DA BATALHA ---",
        "OURO OBTIDO:   +" + std::to_string(ouro) + "G",
        "XP OBTIDO:     +" + std::to_string(xp) + " XP",
        "DANO CAUSADO:   " + std::to_string(danoCausado),
        "DANO RECEBIDO:  " + std::to_string(danoRecebido)
    };

    // Imprime as estatisticas centralizadas e vermelhas
    imprimirBlocoCentralizado(linhas, 0, "\033[31m");
    
    std::cout << "\n";
    
    std::string msgMorte = "Voce pereceu em combate...";
    std::cout << "\n";
    imprimirBlocoCentralizado({msgMorte}, 0, "\033[31m");
    
    std::cout << "\n" << std::string(largura, '=') << "\n";

    esperar();
}

void Menu::exibirInventario(Personagem* p) 
{
    if (p == nullptr) return;
    limparTela();
    
#ifdef _WIN32
    // Configura o console para aceitar blocos UTF-8
    SetConsoleOutputCP(65001); 
#endif

    int largura = obterLarguraTerminal();
    std::vector<std::string> logoInventario = 
    {
        " █████ ██████   █████ █████   █████ ██████████ ██████   █████ ███████████   █████████   ███████████   █████    ███████   ",
        "░░███ ░░██████ ░░███ ░░███   ░░███ ░░███░░░░░█░░██████ ░░███ ░█░░░███░░░█  ███░░░░░███ ░░███░░░░░███ ░░███   ███░░░░░███ ",
        " ░███  ░███░███ ░███  ░███    ░███  ░███  █ ░  ░███░███ ░███ ░   ░███  ░  ░███    ░███  ░███    ░███  ░███  ███     ░░███",
        " ░███  ░███░░███░███  ░███    ░███  ░██████    ░███░░███░███     ░███     ░███████████  ░██████████   ░███ ░███      ░███",
        " ░███  ░███ ░░██████  ░░███   ███   ░███░░█    ░███ ░░██████     ░███     ░███░░░░░███  ░███░░░░░███  ░███ ░███      ░███",
        " ░███  ░███  ░░█████   ░░░█████░    ░███ ░   █ ░███  ░░█████     ░███     ░███    ░███  ░███    ░███  ░███ ░░███     ███ ",
        " █████ █████  ░░█████    ░░███      ██████████ █████  ░░█████    █████    █████   █████ █████   █████ █████ ░░░███████░  ",
        "░░░░░ ░░░░░    ░░░░░      ░░░      ░░░░░░░░░░ ░░░░░    ░░░░░    ░░░░░    ░░░░░   ░░░░░ ░░░░░   ░░░░░ ░░░░░    ░░░░░░░  "
    };

    std::cout << "\n" << std::string(largura, '=') << "\n\n";
    imprimirBlocoCentralizado(logoInventario, 121, "\033[33m"); // Cor Amarela para destacar o inventário
    std::cout << "\n" << std::string(largura, '=') << "\n\n";

    p->obterInventario()->listarItens(p->obterArma(), p->obterEscudo(), p->obterArmadura()); 
}

void Menu::exibirFichaJogador(Personagem* jogador)
{
    if (jogador == nullptr) return;
    
    limparTela();
    
#ifdef _WIN32
    // Configura o console para aceitar blocos UTF-8
    SetConsoleOutputCP(65001); 
#endif

    int largura = obterLarguraTerminal();
    std::vector<std::string> logoFicha = 
    {
        " ███████████ █████   █████████  █████   █████   █████████  ",
        "░░███░░░░░░█░░███   ███░░░░░███░░███   ░░███   ███░░░░░███ ",
        " ░███   █ ░  ░███  ███     ░░░  ░███    ░███  ░███    ░███ ",
        " ░███████    ░███ ░███          ░███████████  ░███████████ ",
        " ░███░░░█    ░███ ░███          ░███░░░░░███  ░███░░░░░███ ",
        " ░███  ░     ░███ ░░███     ███ ░███    ░███  ░███    ░███ ",
        " █████       █████ ░░█████████  █████   █████ █████   █████",
        "░░░░░       ░░░░░   ░░░░░░░░░  ░░░░░   ░░░░░ ░░░░░   ░░░░░ "
    };

    std::cout << "\n" << std::string(largura, '=') << "\n\n";
    imprimirBlocoCentralizado(logoFicha, 59, "\033[34m"); // Cor Ciano para Ficha de atributos
    std::cout << "\n" << std::string(largura, '=') << "\n\n";

    double m = jogador->obterMultiplicador();
    int t = jogador->obterTurnosBuff();

    std::vector<std::string> linhas = {
        "NOME:           " + jogador->obterNome(),
        "RACA:           " + jogador->obterRaca()->obterNomeRaca(),
        "CLASSE:         " + jogador->obterNomeClasse(),
        "NIVEL:          " + std::to_string(jogador->obterNivel()),
        "XP:             " + std::to_string(jogador->obterXpAtual()) + " / " + std::to_string(jogador->obterXpParaSubir()),
        "DIFICULDADE:    " + std::string(jogador->obterDificuldade() == 1 ? "Facil" : (jogador->obterDificuldade() == 2 ? "Normal" : "Dificil")),
        "[PARRY]:        " + std::string(jogador->obterParryAtivado() ? "Ligado" : "Desligado"),
        "OURO:           " + std::to_string(jogador->obterInventario()->obterOuro()) + "G",
        "",
        "PASSIVA RACA:   " + jogador->obterRaca()->obterNomeHabilidadeRaca(),
        "-> " + jogador->obterRaca()->obterDescricaoHabilidadeRaca(),
        "",
        "ATIVA CLASSE:   " + jogador->obterClasse()->obterNomeHabilidadeClasseAtiva(),
        "-> " + jogador->obterClasse()->obterDescricaoHabilidadeClasseAtiva(),
        "",
        "ATRIBUTOS TOTAIS:",
        "- HP: " + std::to_string(jogador->obterVida()) + "/" + std::to_string(jogador->obterVidaMaxima()) + " (0)"
    };

    auto addAtributo = [&](std::string nome, int valorBase) 
    {
        std::string linha = "- " + nome + ": " + std::to_string(valorBase);
        if (t > 0) {
            std::string sm = std::to_string(m);
            sm.erase(sm.find_last_not_of('0') + 1, std::string::npos);
            if (sm.back() == '.') sm += "0";
            linha += " (" + std::to_string(static_cast<int>(valorBase * m)) + "){x" + sm + "} por " + std::to_string(t) + " turnos";
        } else {
            linha += " (0)";
        }
        linhas.push_back(linha);
    };

    std::string cl = jogador->obterNomeClasse();
    addAtributo(cl == "Guerreiro" ? "Forca [DANO]" : "Forca", jogador->obterForca());
    addAtributo(cl == "Arqueiro" ? "Destreza [DANO]" : "Destreza", jogador->obterDestreza());
    addAtributo("Resistencia", jogador->obterResistencia());
    addAtributo("Constituicao", jogador->obterConstituicao());
    addAtributo(cl == "Mago" ? "Inteligencia [DANO]" : "Inteligencia", jogador->obterInteligencia());
    addAtributo(cl == "Bardo" ? "Sabedoria [DANO]" : "Sabedoria", jogador->obterSabedoria());
    
    int larguraConsole = obterLarguraTerminal();
    int maxLen = 0;
    for (const std::string& linha : linhas) {
        if ((int)linha.length() > maxLen) maxLen = (int)linha.length();
    }
    int espacos = (larguraConsole - maxLen) / 2;
    std::string margem(espacos > 0 ? espacos : 0, ' ');

    for (const std::string& linha : linhas) {
        std::cout << margem << linha << "\n";
    }
    
    std::cout << "\n" << std::string(larguraConsole, '=') << "\n";
}
