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

void Menu::maximizarJanelaDoTerminal()
{
#ifdef _WIN32
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_MAXIMIZE);
#endif
}

int Menu::obterLarguraDoTerminalEmColunas() 
{
    int largura = 120; // Tamanho padrão de fallback

#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) 
    {
        largura = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
#endif
    return largura;
}

void Menu::limparTelaDoTerminal() 
{
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void Menu::aguardarPressionamentoDeEnter() 
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

void Menu::imprimirTextoComEfeitoDeDigitacao(const std::string& textoParaImprimir, int tempoDeEsperaEmMilissegundos)
{
    // 1. Salva a posição atual do cursor para não perder o lugar do texto
    // 2. Move para a linha 24, coluna 1 (canto inferior esquerdo)
    // 3. Imprime a mensagem em cinza para não distrair
    // 4. Volta para a posição original
    std::cout << "\033[s\033[80;1H\033[1;90m[Pressione 'k' para pular]\033[u";

    for (size_t i = 0; i < textoParaImprimir.length(); ++i)
    {
        // Verifica se o usuário pressionou a tecla 'k'
        #ifdef _WIN32
            if (_kbhit()) 
            {
                char tecla = _getch();
                if (tecla == 'k' || tecla == 'K') 
                {
                    // Limpa a linha do "pular" antes de sair para não deixar lixo
                    std::cout << "\033[s\033[24;1H\033[K\033[u";
                    
                    // Pula o restante do texto
                    std::cout << textoParaImprimir.substr(i) << std::flush;
                    return; 
                }
            }
        #endif

        // Imprime o caractere atual do diálogo
        std::cout << textoParaImprimir[i] << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(tempoDeEsperaEmMilissegundos));
    }
    std::cout << std::endl;
}

bool Menu::exibirConfirmacaoDeEscolhaComArteLadoALado(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha, const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir) 
{
    limparTelaDoTerminal();
    exibirLogoDoJogo("PREVIA DA " + tipoDeEscolha + ": " + nomeDaEscolha);
    int larguraTerminal = obterLarguraDoTerminalEmColunas();

    int larguraArte = 0;
    for (const std::string& l : arteAsciiParaExibir) if ((int)l.length() > larguraArte) larguraArte = (int)l.length();
    
    int larguraInfo = 40;
    for (const std::string& s : informacoesParaExibir) if ((int)s.length() > larguraInfo) larguraInfo = (int)s.length();
    
    int gap = 6;
    int recuo = (larguraTerminal - (larguraInfo + gap + larguraArte)) / 2;
    if (recuo < 0) recuo = 0;

    size_t maxL = std::max(arteAsciiParaExibir.size(), informacoesParaExibir.size());
    for (size_t i = 0; i < maxL; ++i) 
    {
        std::cout << std::string(recuo, ' ');
        if (i < informacoesParaExibir.size()) std::cout << std::left << std::setw(larguraInfo) << informacoesParaExibir[i];
        else std::cout << std::string(larguraInfo, ' ');
        std::cout << std::string(gap, ' ');
        if (i < arteAsciiParaExibir.size()) std::cout << arteAsciiParaExibir[i];
        std::cout << "\n";
    }

    std::cout << "\n" << std::string(recuo, ' ') << "0. VOLTAR | 1. CONFIRMAR\n";
    std::cout << std::string(recuo, ' ') << "Escolha: ";
    int confirma; 
    if (!(std::cin >> confirma)) { std::cin.clear(); std::cin.ignore(1000, '\n'); return false; }
    return confirma == 1;
}

void Menu::imprimirLinhasCentralizadasNaTela(const std::vector<std::string>& linhasParaImprimir, int larguraVisualDaArte, const std::string& codigoDeCorAnsi) 
{
    int larguraConsole = obterLarguraDoTerminalEmColunas();
    
    if (larguraVisualDaArte <= 0) 
    {
        for (const std::string& linhaTexto : linhasParaImprimir) 
        {
            if ((int)linhaTexto.length() > larguraVisualDaArte) larguraVisualDaArte = (int)linhaTexto.length();
        }
    }
    
    int espacosPad = (larguraConsole > larguraVisualDaArte) ? (larguraConsole - larguraVisualDaArte) / 2 : 0;
    std::string padding(espacosPad > 0 ? espacosPad : 0, ' ');

    for (const std::string& linhaTexto : linhasParaImprimir) 
    {
        std::cout << padding << (codigoDeCorAnsi.empty() ? "" : codigoDeCorAnsi) << linhaTexto << (codigoDeCorAnsi.empty() ? "" : "\033[0m") << "\n";
    }
}

Personagem* Menu::iniciarCriacaoDePersonagem() 
{
    std::string nomeDoPersonagem = "";
    RacaBase* racaEscolhida = nullptr;
    ClasseBase* classeEscolhida = nullptr;
    bool sistemaDeParryAtivado = false;
    int nivelDeDificuldadeEscolhido = 2;
    int etapaDeCriacaoAtual = 1; 

    auto formatarAtributo = [](const std::string& n, int v) 
    {
        return " - " + n + ": " + (v >= 0 ? "+" : "") + std::to_string(v);
    };

    while (etapaDeCriacaoAtual <= 5) 
    {
        if (etapaDeCriacaoAtual == 1) // --- ETAPA 1: NOME ---
        {
            limparTelaDoTerminal();
            exibirLogoDoJogo("INTRODUCAO AO RPG");
            imprimirTextoComEfeitoDeDigitacao(" [NARRACAO]: O reino clama por um novo destino...\n", 35);
            imprimirTextoComEfeitoDeDigitacao(" [NARRACAO]: E todas lendas possuem um nome.\n\n", 35);
            std::cout << " > Escolha o nome do seu personagem (ou '0' para sair): ";
            
            std::cin.ignore(std::cin.rdbuf()->in_avail(), '\n');
            std::getline(std::cin, nomeDoPersonagem);

            if (nomeDoPersonagem == "0") exit(0);
            if (!nomeDoPersonagem.empty()) etapaDeCriacaoAtual = 2;
        }
        else if (etapaDeCriacaoAtual == 2) // --- ETAPA 2: RACA ---
        {
            limparTelaDoTerminal();
            exibirLogoDoJogo("SELECAO DE RACA");
            std::cout << "JOGADOR: " << nomeDoPersonagem << "\n";
            std::cout << std::string(obterLarguraDoTerminalEmColunas(), '-') << "\n";
            imprimirTextoComEfeitoDeDigitacao(" [NARRACAO]: Qual sua origem?\n\n", 35);
            
            std::cout << "  [1] Dwarf\n";
            std::cout << "  [2] Elfo\n";
            std::cout << "  [3] Humano\n";
            std::cout << "  [4] Ork\n";
            std::cout << "\n  [0] VOLTAR (selecao de nome)\n";
            std::cout << "\n > Sua escolha: ";
            
            int escolha;
            if (!(std::cin >> escolha)) { std::cin.clear(); std::cin.ignore(1000, '\n'); continue; }
            if (escolha == 0) { etapaDeCriacaoAtual = 1; continue; }

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
                
                if (exibirConfirmacaoDeEscolhaComArteLadoALado("RACA", tempRaca->obterNomeRaca(), info, tempRaca->obterAparenciaRaca())) 
                {
                    racaEscolhida = tempRaca; 
                    etapaDeCriacaoAtual = 3;
                } 
                
                else 
                { 
                    delete tempRaca; 
                }
            }
        }
        else if (etapaDeCriacaoAtual == 3) // --- ETAPA 3: CLASSE ---
        {
            limparTelaDoTerminal();
            exibirLogoDoJogo("SELECAO DE CLASSE");
            std::cout << "JOGADOR: " << nomeDoPersonagem << " | RACA: " << racaEscolhida->obterNomeRaca() << "\n";
            std::cout << std::string(obterLarguraDoTerminalEmColunas(), '-') << "\n";
            imprimirTextoComEfeitoDeDigitacao(" [NARRACAO]: Qual caminho voce seguira neste mundo?\n\n", 35);
            
            std::cout << "  [1] Arqueiro\n";
            std::cout << "  [2] Bardo\n";
            std::cout << "  [3] Guerreiro\n";
            std::cout << "  [4] Mago\n";
            std::cout << "\n  [0] VOLTAR (selecao de raca)\n";
            std::cout << "\n > Sua escolha: ";
            
            int escolha;
            if (!(std::cin >> escolha)) { std::cin.clear(); std::cin.ignore(1000, '\n'); continue; }
            if (escolha == 0) { delete racaEscolhida; racaEscolhida = nullptr; etapaDeCriacaoAtual = 2; continue; }

            ClasseBase* temp = nullptr;
            switch(escolha) 
            {
                case 1: temp = new ClasseArqueiro(); break;
                case 2: temp = new ClasseBardo(); break;
                case 3: temp = new ClasseGuerreiro(); break;
                case 4: temp = new ClasseMago(); break;
            }

            if (temp) 
            {
                Atributos stats = temp->obterAtributosClasse();
                std::vector<std::string> info = 
                {
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
                    " " + temp->obterNomeHabilidadeClasse(),
                    " - " + temp->obterDescricaoHabilidadeClasse(),
                    "",
                    "[ EQUIPAMENTO INICIAL ]"
                };
                
                std::vector<Item*> kit = temp->obterEquipamentoClasse();
                std::map<std::string, int> contagem;
                for (Item* i : kit) contagem[i->obterNomeItem()]++;
                for (auto const& [nomeI, qtd] : contagem) info.push_back(" - " + std::to_string(qtd) + "x " + nomeI);
                for (Item* i : kit) delete i;

                if (exibirConfirmacaoDeEscolhaComArteLadoALado("CLASSE", temp->obterNomeClasse(), info, temp->obterAparenciaClasseMenu())) 
                {
                    classeEscolhida = temp; 
                    etapaDeCriacaoAtual = 4;
                } 

                else 
                { 
                    delete temp; 
                }
            }
        }
        else if (etapaDeCriacaoAtual == 4) // --- ETAPA 4: CONFIGURACOES DO JOGO ---
        {
            limparTelaDoTerminal();
            exibirLogoDoJogo("CONFIGURACOES DO JOGO");
            std::cout << "JOGADOR: " << nomeDoPersonagem << " | RACA: " << racaEscolhida->obterNomeRaca() << " | CLASSE: " << classeEscolhida->obterNomeClasse() << "\n";
            std::cout << std::string(obterLarguraDoTerminalEmColunas(), '-') << "\n";
            imprimirTextoComEfeitoDeDigitacao(" [SISTEMA]: Deseja ativar o sistema de PARRY?\n\n", 35);
            imprimirTextoComEfeitoDeDigitacao(" (Permite reduzir danos ao digitar uma sequencia de numeros num tempo limite)\n\n", 35);
            
            std::cout << "  [1] LIGAR Parry\n";
            std::cout << "  [2] DESLIGAR Parry\n";
            std::cout << "\n  [0] VOLTAR (selecao de classe)\n";
            std::cout << "\n > Sua escolha: ";
            
            int escolha;
            if (!(std::cin >> escolha)) { std::cin.clear(); std::cin.ignore(1000, '\n'); continue; }
            if (escolha == 0) { delete classeEscolhida; classeEscolhida = nullptr; etapaDeCriacaoAtual = 3; continue; }

            if (escolha == 1 || escolha == 2) 
            {
                sistemaDeParryAtivado = (escolha == 1);
                etapaDeCriacaoAtual = 5;
            }
        }
        else if (etapaDeCriacaoAtual == 5) // --- ETAPA 5: DIFICULDADE ---
        {
            limparTelaDoTerminal();
            exibirLogoDoJogo("DIFICULDADE DO MUNDO");
            std::cout << "JOGADOR: " << nomeDoPersonagem << " | RACA: " << racaEscolhida->obterNomeRaca() << " | CLASSE: " << classeEscolhida->obterNomeClasse() << "\n";
            std::cout << std::string(obterLarguraDoTerminalEmColunas(), '-') << "\n";
            imprimirTextoComEfeitoDeDigitacao(" [SISTEMA]: Escolha o nivel de desafio da sua jornada:\n\n", 35);
            
            std::cout << "  [1] FACIL   (Inimigos com 1x Atributos, sem habilidades de raca e sem classe)\n";
            std::cout << "  [2] NORMAL  (Inimigos com 1.5x Atributos, com habilidades de raca mas sem classes)\n";
            std::cout << "  [3] DIFICIL (Inimigos com 2x Atributos, com habilidades de raca e com classes)\n";
            std::cout << "\n  [0] VOLTAR (configuracao de parry)\n";
            std::cout << "\n > Sua escolha: ";
            
            int escolha;
            if (!(std::cin >> escolha)) { std::cin.clear(); std::cin.ignore(1000, '\n'); continue; }
            if (escolha == 0) { etapaDeCriacaoAtual = 4; continue; }

            if (escolha >= 1 && escolha <= 3) 
            {
                nivelDeDificuldadeEscolhido = escolha;
                etapaDeCriacaoAtual = 6;
            }
        }
    }
    Personagem* personagemCriado = new Personagem(nomeDoPersonagem, racaEscolhida, classeEscolhida);
    personagemCriado->definirParryAtivado(sistemaDeParryAtivado);
    personagemCriado->definirDificuldade(nivelDeDificuldadeEscolhido);
    std::cout << "\n";
    imprimirTextoComEfeitoDeDigitacao(" [SISTEMA]: Personagem criado com sucesso! Iniciando jornada...\n", 35);
    aguardarPressionamentoDeEnter();
    return personagemCriado;
}

void Menu::exibirBarraDeStatusDoJogador(Personagem* jogadorAtual) 
{
    if (jogadorAtual == nullptr) return;
    int larguraDoTerminal = obterLarguraDoTerminalEmColunas();
    std::string nomeDaArma = (jogadorAtual->obterArma()) ? jogadorAtual->obterArma()->obterNomeItem() : "Punhos";
    std::string nomeDoEscudo = (jogadorAtual->obterEscudo()) ? jogadorAtual->obterEscudo()->obterNomeItem() : "Nenhum";
    std::string nomeDaArmadura = (jogadorAtual->obterArmadura()) ? jogadorAtual->obterArmadura()->obterNomeItem() : "Trapos";
    
    double porcentagemDeVida = static_cast<double>(jogadorAtual->obterVida()) / jogadorAtual->obterVidaMaxima();
    std::string corVerde = "\033[32m";    // Verde
    std::string corLaranja = "\033[33m";  // Laranja
    std::string corVermelho = "\033[31m"; // Vermelho
    std::string corReset = "\033[0m";     // Reset
    
    // --- LÓGICA DE COR PARA O HP ---
    std::string corSelecionadaParaOValorDeVida;
    if (porcentagemDeVida > 0.70)      corSelecionadaParaOValorDeVida = corVerde;
    else if (porcentagemDeVida > 0.30) corSelecionadaParaOValorDeVida = corLaranja;
    else                               corSelecionadaParaOValorDeVida = corVermelho;
    // -------------------------------

    std::vector<std::string> arteDoCoracao;
    
    if (porcentagemDeVida > 0.70) 
    {
        arteDoCoracao = {
            "   _   _   ",
            "  / \\_/ \\  ",
            "  \\     /  ",
            "   \\___/   "
        };
        arteDoCoracao[0] = corVerde + arteDoCoracao[0] + corReset;
        arteDoCoracao[1] = corVerde + arteDoCoracao[1] + corReset;
        arteDoCoracao[2] = corVerde + arteDoCoracao[2] + corReset;
        arteDoCoracao[3] = corVerde + arteDoCoracao[3] + corReset;
    }
    else if (porcentagemDeVida > 0.30) 
    {
        arteDoCoracao = {
            "   _   _   ",
            "  / \\// \\  ",
            "  \\  \\ /   ",
            "   \\___/   "
        };
        arteDoCoracao[0] = corLaranja + arteDoCoracao[0] + corReset;
        arteDoCoracao[1] = corLaranja + arteDoCoracao[1] + corReset;
        arteDoCoracao[2] = corLaranja + arteDoCoracao[2] + corReset;
        arteDoCoracao[3] = corLaranja + arteDoCoracao[3] + corReset;
    }
    else 
    {
        arteDoCoracao = {
            "  _     _  ",
            " / \\   / \\ ",
            " \\     \\_/ ",
            "  \\___/    "
        };
        arteDoCoracao[0] = corVermelho + arteDoCoracao[0] + corReset;
        arteDoCoracao[1] = corVermelho + arteDoCoracao[1] + corReset;
        arteDoCoracao[2] = corVermelho + arteDoCoracao[2] + corReset;
        arteDoCoracao[3] = corVermelho + arteDoCoracao[3] + corReset;
    }
    
    std::string arteDeBarraDeXp = "[";
    int tamanhoBarra = 10;
    int preenchido = (jogadorAtual->obterXpAtual() * tamanhoBarra) / jogadorAtual->obterXpParaSubir();
    if (preenchido > tamanhoBarra) preenchido = tamanhoBarra;
    if (preenchido > 0) arteDeBarraDeXp += "\033[34m" + std::string(preenchido, '#') + "\033[0m";
    if (tamanhoBarra > preenchido) arteDeBarraDeXp += std::string(tamanhoBarra - preenchido, '-');
    arteDeBarraDeXp += "] \033[34m" + std::to_string(jogadorAtual->obterXpAtual()) + "\033[0m/" + std::to_string(jogadorAtual->obterXpParaSubir());
    
    // Aplicando a cor dinâmica ao HP na linha do status
    std::vector<std::string> linhasParaImprimir = 
    {
        "| " + arteDoCoracao[0] + " |",
        "| " + arteDoCoracao[1] + " |  JOGADOR: " + jogadorAtual->obterNome() + " (" + jogadorAtual->obterRaca()->obterNomeRaca() + " / " + jogadorAtual->obterNomeClasse() + ") | NIVEL: " + std::to_string(jogadorAtual->obterNivel()),
        "| " + arteDoCoracao[2] + " |  HP: " + corSelecionadaParaOValorDeVida + std::to_string(jogadorAtual->obterVida()) + corReset + "/" + std::to_string(jogadorAtual->obterVidaMaxima()) + " | OURO: \033[33m" + std::to_string(jogadorAtual->obterInventario()->obterQuantidadeDeOuro()) + "G\033[0m | XP: " + arteDeBarraDeXp,
        "| " + arteDoCoracao[3] + " |  EQUIP: " + nomeDaArma + " | " + nomeDoEscudo + " | " + nomeDaArmadura,
        "| " + std::string(11, ' ') + " |"
    };

    int maxLen = 0;
    for (const std::string& linhaDeTextoAtual : linhasParaImprimir) 
    {
        if ((int)linhaDeTextoAtual.length() > maxLen) maxLen = (int)linhaDeTextoAtual.length();
    }
    int espacosParaCentralizar = (larguraDoTerminal - maxLen) / 2;
    std::string margemEsquerda(espacosParaCentralizar > 0 ? espacosParaCentralizar : 0, ' ');

    std::cout << std::string(larguraDoTerminal, '=') << "\n";
    for (const std::string& linhaDeTextoAtual : linhasParaImprimir) 
    {
        std::cout << margemEsquerda << linhaDeTextoAtual << "\n";
    }
    std::cout << std::string(larguraDoTerminal, '=') << "\n";
}

void Menu::exibirHordaDeInimigosLadoALado(const std::vector<Personagem*>& listaDeInimigos) 
{
    if (listaDeInimigos.empty()) return;
    int larguraTerminal = obterLarguraDoTerminalEmColunas();
    // Pega a arte dinamicamente da raca do inimigo que esta sendo enfrentado
    std::vector<std::string> arteDoInimigo = listaDeInimigos[0]->obterRaca()->obterAparenciaRaca();
    int quantidadeTotalDeInimigosNaHorda = static_cast<int>(listaDeInimigos.size());
    int larguraSeparadaParaCadaColuna = larguraTerminal / quantidadeTotalDeInimigosNaHorda; 

    std::cout << std::string(larguraTerminal, '-') << "\n";
    for (size_t indiceInimigo = 0; indiceInimigo < listaDeInimigos.size(); indiceInimigo++) 
    {
        std::string tagIdentificadoraDoInimigo = listaDeInimigos[indiceInimigo]->obterNome() + " [" + std::to_string(indiceInimigo) + "]";
        int espacosParaCentralizarOId = (larguraSeparadaParaCadaColuna - (int)tagIdentificadoraDoInimigo.length()) / 2;
        std::cout << std::string(espacosParaCentralizarOId > 0 ? espacosParaCentralizarOId : 0, ' ') << std::left << std::setw(larguraSeparadaParaCadaColuna - espacosParaCentralizarOId) << tagIdentificadoraDoInimigo;
    }
    std::cout << "\n";
    for (size_t indiceInimigo = 0; indiceInimigo < listaDeInimigos.size(); indiceInimigo++) 
    {
        std::string valorDePontosDeVidaDoInimigo = "HP: " + std::to_string(listaDeInimigos[indiceInimigo]->obterVida()) + "/" + std::to_string(listaDeInimigos[indiceInimigo]->obterVidaMaxima());
        int espacosParaCentralizarOHp = (larguraSeparadaParaCadaColuna - (int)valorDePontosDeVidaDoInimigo.length()) / 2;
        std::cout << std::string(espacosParaCentralizarOHp > 0 ? espacosParaCentralizarOHp : 0, ' ') << std::left << std::setw(larguraSeparadaParaCadaColuna - espacosParaCentralizarOHp) << valorDePontosDeVidaDoInimigo;
    }
    std::cout << "\n\n";
    for (size_t indiceDaLinhaDaArte = 0; indiceDaLinhaDaArte < arteDoInimigo.size(); indiceDaLinhaDaArte++) 
    {
        for (size_t indiceDoInimigoParaDesenhar = 0; indiceDoInimigoParaDesenhar < listaDeInimigos.size(); indiceDoInimigoParaDesenhar++) 
        {
            int espacosParaCentralizarAArte = (larguraSeparadaParaCadaColuna - (int)arteDoInimigo[indiceDaLinhaDaArte].length()) / 2;
            std::cout << std::string(espacosParaCentralizarAArte > 0 ? espacosParaCentralizarAArte : 0, ' ') << std::left << std::setw(larguraSeparadaParaCadaColuna - espacosParaCentralizarAArte) << arteDoInimigo[indiceDaLinhaDaArte];
        }
        std::cout << "\n";
    }
    std::cout << std::string(larguraTerminal, '-') << "\n\n";
}

void Menu::exibirLogoDoJogo(const std::string& tituloDaTela) 
{
#ifdef _WIN32
    SetConsoleOutputCP(65001); 
#endif
    int larguraConsole = obterLarguraDoTerminalEmColunas();
    
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

    for (size_t i = 0; i < logoTexto.size(); ++i) 
    {
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
    if (tituloDaTela.empty()) 
    {
        std::cout << std::string(larguraConsole, '=') << "\n\n";
    } 
    else 
    {
        std::cout << std::string(larguraConsole, '=') << "\n";
        int espacos = (larguraConsole - (int)tituloDaTela.length()) / 2;
        std::cout << std::string(espacos > 0 ? espacos : 0, ' ') << tituloDaTela << "\n";
        std::cout << std::string(larguraConsole, '=') << "\n\n";
    }
}

void Menu::exibirLogoParaTelaDeCombate(const std::string& tituloDaTela) 
{
#ifdef _WIN32
    // Configura o console para aceitar blocos UTF-8
    SetConsoleOutputCP(65001); 
#endif

    int larguraConsole = obterLarguraDoTerminalEmColunas();
    
    std::vector<std::string> logo = 
    {
        "   █████████     ███████    ██████   ██████ ███████████    █████████    ███████████ ██████████",
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
    imprimirLinhasCentralizadasNaTela(logo, 95, "\033[31m");

    std::cout << "\n";
    
    std::cout << std::string(larguraConsole, '=') << "\n";
    int espacos = (larguraConsole - (int)tituloDaTela.length()) / 2;
    std::cout << std::string(espacos > 0 ? espacos : 0, ' ') << tituloDaTela << "\n";
    std::cout << std::string(larguraConsole, '=') << "\n\n";
}

void Menu::exibirTelaDeVitoria(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido)
{
    limparTelaDoTerminal();

    int largura = obterLarguraDoTerminalEmColunas();

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
    imprimirLinhasCentralizadasNaTela(logoVitoria, 85, "\033[32m");
    std::cout << "\n" << std::string(largura, '=') << "\n\n";

    std::vector<std::string> linhas = {
        "NOME:           " + jogadorAtual->obterNome(),
        "RACA:           " + jogadorAtual->obterRaca()->obterNomeRaca(),
        "CLASSE:         " + jogadorAtual->obterNomeClasse(),
        "HP RESTANTE:    " + std::to_string(jogadorAtual->obterVida()) + "/" + std::to_string(jogadorAtual->obterVidaMaxima()),
        "OURO TOTAL:     " + std::to_string(jogadorAtual->obterInventario()->obterQuantidadeDeOuro()) + "G",
        "NIVEL:          " + std::to_string(jogadorAtual->obterNivel()) + " (XP: " + std::to_string(jogadorAtual->obterXpAtual()) + "/" + std::to_string(jogadorAtual->obterXpParaSubir()) + ")",
        "",
        "--- ESTATISTICAS DA BATALHA ---",
        "OURO OBTIDO:   +" + std::to_string(quantidadeDeOuroObtido) + "G",
        "XP OBTIDO:     +" + std::to_string(quantidadeDeXpObtido) + " XP",
        "DANO CAUSADO:   " + std::to_string(totalDeDanoCausado),
        "DANO RECEBIDO:  " + std::to_string(totalDeDanoRecebido)
    };

    // Imprime as estatisticas centralizadas e verdes (largura automatica = 0)
    imprimirLinhasCentralizadasNaTela(linhas, 0, "\033[32m");

    std::cout << "\n" << std::string(largura, '=') << "\n";

    aguardarPressionamentoDeEnter();
}

void Menu::exibirTelaDeDerrota(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido)
{
    limparTelaDoTerminal();

    int largura = obterLarguraDoTerminalEmColunas();

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
    imprimirLinhasCentralizadasNaTela(logoDerrota, 101, "\033[31m");
    std::cout << "\n" << std::string(largura, '=') << "\n\n";

    std::vector<std::string> linhas = {
        "NOME:           " + jogadorAtual->obterNome(),
        "RACA:           " + jogadorAtual->obterRaca()->obterNomeRaca(),
        "CLASSE:         " + jogadorAtual->obterNomeClasse(),
        "HP RESTANTE:    " + std::to_string(jogadorAtual->obterVida()) + "/" + std::to_string(jogadorAtual->obterVidaMaxima()),
        "OURO TOTAL:     " + std::to_string(jogadorAtual->obterInventario()->obterQuantidadeDeOuro()) + "G",
        "NIVEL:          " + std::to_string(jogadorAtual->obterNivel()) + " (XP: " + std::to_string(jogadorAtual->obterXpAtual()) + "/" + std::to_string(jogadorAtual->obterXpParaSubir()) + ")",
        "",
        "--- ESTATISTICAS DA BATALHA ---",
        "OURO OBTIDO:   +" + std::to_string(quantidadeDeOuroObtido) + "G",
        "XP OBTIDO:     +" + std::to_string(quantidadeDeXpObtido) + " XP",
        "DANO CAUSADO:   " + std::to_string(totalDeDanoCausado),
        "DANO RECEBIDO:  " + std::to_string(totalDeDanoRecebido)
    };

    // Imprime as estatisticas centralizadas e vermelhas
    imprimirLinhasCentralizadasNaTela(linhas, 0, "\033[31m");
    
    std::cout << "\n";
    
    std::string msgMorte = "Voce pereceu em combate...";
    std::cout << "\n";
    imprimirLinhasCentralizadasNaTela({msgMorte}, 0, "\033[31m");
    
    std::cout << "\n" << std::string(largura, '=') << "\n";

    aguardarPressionamentoDeEnter();
}

void Menu::exibirTelaDeInventario(Personagem* jogadorAtual) 
{
    if (jogadorAtual == nullptr) return;
    limparTelaDoTerminal();
    
#ifdef _WIN32
    // Configura o console para aceitar blocos UTF-8
    SetConsoleOutputCP(65001); 
#endif

    int largura = obterLarguraDoTerminalEmColunas();
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
    imprimirLinhasCentralizadasNaTela(logoInventario, 121, "\033[33m"); 
    std::cout << "\n" << std::string(largura, '=') << "\n\n";

    jogadorAtual->obterInventario()->exibirInventarioOrganizadoPorCategorias(jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura()); 
}

void Menu::exibirTelaDeAtributosDoJogador(Personagem* jogadorAtual)
{
    if (jogadorAtual == nullptr) return;
    
    limparTelaDoTerminal();
    
#ifdef _WIN32
    // Configura o console para aceitar blocos UTF-8
    SetConsoleOutputCP(65001); 
#endif

    int largura = obterLarguraDoTerminalEmColunas();
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
    imprimirLinhasCentralizadasNaTela(logoFicha, 59, "\033[34m"); 
    std::cout << "\n" << std::string(largura, '=') << "\n\n";

    double multiplicadorDeAtributosAtual = jogadorAtual->obterMultiplicador();
    int quantidadeDeTurnosRestantesDoBuff = jogadorAtual->obterTurnosBuff();

    std::vector<std::string> linhas = {
        "NOME:           " + jogadorAtual->obterNome(),
        "RACA:           " + jogadorAtual->obterRaca()->obterNomeRaca(),
        "CLASSE:         " + jogadorAtual->obterNomeClasse(),
        "NIVEL:          " + std::to_string(jogadorAtual->obterNivel()),
        "XP:             " + std::to_string(jogadorAtual->obterXpAtual()) + " / " + std::to_string(jogadorAtual->obterXpParaSubir()),
        "DIFICULDADE:    " + std::string(jogadorAtual->obterDificuldade() == 1 ? "Facil" : (jogadorAtual->obterDificuldade() == 2 ? "Normal" : "Dificil")),
        "[PARRY]:        " + std::string(jogadorAtual->obterParryAtivado() ? "Ligado" : "Desligado"),
        "OURO:           " + std::to_string(jogadorAtual->obterInventario()->obterQuantidadeDeOuro()) + "G",
        "",
        "PASSIVA RACA:   " + jogadorAtual->obterRaca()->obterNomeHabilidadeRaca(),
        "-> " + jogadorAtual->obterRaca()->obterDescricaoHabilidadeRaca(),
        "",
        "ATIVA CLASSE:   " + jogadorAtual->obterClasse()->obterNomeHabilidadeClasse(),
        "-> " + jogadorAtual->obterClasse()->obterDescricaoHabilidadeClasse(),
        "",
        "ATRIBUTOS TOTAIS:",
        "- HP: " + std::to_string(jogadorAtual->obterVida()) + "/" + std::to_string(jogadorAtual->obterVidaMaxima()) + " (0)"
    };

    auto addAtributo = [&](std::string nome, int valorBase) 
    {
        std::string linha = "- " + nome + ": " + std::to_string(valorBase);
        if (quantidadeDeTurnosRestantesDoBuff > 0) 
        {
            std::string sm = std::to_string(multiplicadorDeAtributosAtual);
            sm.erase(sm.find_last_not_of('0') + 1, std::string::npos);
            if (sm.back() == '.') sm += "0";
            linha += " (" + std::to_string(static_cast<int>(valorBase * multiplicadorDeAtributosAtual)) + "){x" + sm + "} por " + std::to_string(quantidadeDeTurnosRestantesDoBuff) + " turnos";
        } 
        else
        {
            linha += " (0)";
        }
        linhas.push_back(linha);
    };

    std::string nomeDaClasse = jogadorAtual->obterNomeClasse();
    addAtributo(nomeDaClasse == "Guerreiro" ? "Forca [DANO]" : "Forca", jogadorAtual->obterForca());
    addAtributo(nomeDaClasse == "Arqueiro" ? "Destreza [DANO]" : "Destreza", jogadorAtual->obterDestreza());
    addAtributo("Resistencia", jogadorAtual->obterResistencia());
    addAtributo("Constituicao", jogadorAtual->obterConstituicao());
    addAtributo(nomeDaClasse == "Mago" ? "Inteligencia [DANO]" : "Inteligencia", jogadorAtual->obterInteligencia());
    addAtributo(nomeDaClasse == "Bardo" ? "Sabedoria [DANO]" : "Sabedoria", jogadorAtual->obterSabedoria());
    
    int larguraConsole = obterLarguraDoTerminalEmColunas();
    int maxLen = 0;
    for (const std::string& linha : linhas) 
    {
        if ((int)linha.length() > maxLen) maxLen = (int)linha.length();
    }
    int espacos = (larguraConsole - maxLen) / 2;
    std::string margem(espacos > 0 ? espacos : 0, ' ');

    for (const std::string& linha : linhas) 
    {
        std::cout << margem << linha << "\n";
    }
    
    std::cout << "\n" << std::string(larguraConsole, '=') << "\n";
}
