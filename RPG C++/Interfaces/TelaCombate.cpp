#include <iostream>
#include <vector>
#include <string>

#ifdef _WIN32
    #include <windows.h>
#endif

#include "TelaCombate.h"
#include "../Sistema/Menu.h"
#include "../Sistema/Personagem.h"
#include "../Sistema/Tipos.h"
#include "../Raças/RacaBase.h"
#include "../Classes/ClasseBase.h"
#include "../Inventario/Item.h"

void TelaCombate::exibirLogoParaTelaDeCombate(const std::string& tituloDaTela) 
{
#ifdef _WIN32
    // Configura o console para aceitar blocos UTF-8
    SetConsoleOutputCP(65001); 
#endif

    int larguraConsole = Menu::obterLarguraDoTerminalEmColunas();
    
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
    Menu::imprimirLinhasCentralizadasNaTela(logo, 95, "\033[31m");

    std::cout << "\n";
    
    std::cout << std::string(larguraConsole, '=') << "\n";
    int espacos = (larguraConsole - (int)tituloDaTela.length()) / 2;
    std::cout << std::string(espacos > 0 ? espacos : 0, ' ') << tituloDaTela << "\n";
    std::cout << std::string(larguraConsole, '=') << "\n\n";
}

void TelaCombate::exibirBarraDeStatusDoJogador(Personagem* jogadorAtual) 
{
    if (jogadorAtual == nullptr) return;
    int larguraDoTerminal = Menu::obterLarguraDoTerminalEmColunas();
    std::string nomeDaArma = (jogadorAtual->obterArma()) ? jogadorAtual->obterArma()->obterNomeItem() + jogadorAtual->obterArma()->obterInfoStatus() : "Punhos";
    std::string nomeDoEscudo = (jogadorAtual->obterEscudo()) ? jogadorAtual->obterEscudo()->obterNomeItem() + jogadorAtual->obterEscudo()->obterInfoStatus() : "Nenhum";
    std::string nomeDaArmadura = (jogadorAtual->obterArmadura()) ? jogadorAtual->obterArmadura()->obterNomeItem() + jogadorAtual->obterArmadura()->obterInfoStatus() : "Trapos";
    
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
    
    std::string statusStr = "";
    if (jogadorAtual->possuiEfeito(EfeitoNomes::BUFF_ATRIBUTOS)) statusStr += "\033[32m[Buff Atributos]\033[0m ";
    if (jogadorAtual->possuiEfeito(EfeitoNomes::METADE_DANO)) statusStr += "\033[36m[Metade Dano]\033[0m ";
    if (jogadorAtual->possuiEfeito(EfeitoNomes::INVOIVEL)) statusStr += "\033[34m[Inviolavel]\033[0m ";
    if (jogadorAtual->possuiEfeito(EfeitoNomes::SANGRAMENTO)) statusStr += "\033[31m[Sangrando]\033[0m ";
    if (jogadorAtual->possuiEfeito(EfeitoNomes::LENTIDAO)) statusStr += "\033[35m[Lento]\033[0m ";
    if (jogadorAtual->possuiEfeito(EfeitoNomes::FRAQUEZA)) statusStr += "\033[33m[Fraqueza]\033[0m ";
    if (jogadorAtual->possuiEfeito(EfeitoNomes::QUEBRA_RESISTENCIA)) statusStr += "\033[36m[Quebra Def.]\033[0m ";
    if (statusStr.empty()) statusStr = "Nenhum";

    // Aplicando a cor dinâmica ao HP na linha do status
    std::vector<std::string> linhasParaImprimir = 
    {
        "| " + arteDoCoracao[0] + " |",
        "| " + arteDoCoracao[1] + " |  JOGADOR: " + jogadorAtual->obterNome() + " (" + jogadorAtual->obterRaca()->obterNomeRaca() + " / " + jogadorAtual->obterNomeClasse() + ") | NIVEL: " + std::to_string(jogadorAtual->obterNivel()),
        "| " + arteDoCoracao[2] + " |  HP: " + corSelecionadaParaOValorDeVida + std::to_string(jogadorAtual->obterVida()) + corReset + "/" + std::to_string(jogadorAtual->obterVidaMaxima()) + " | OURO: \033[33m" + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G\033[0m | XP: " + arteDeBarraDeXp,
        "| " + arteDoCoracao[3] + " |  EQUIP: " + nomeDaArma + " | " + nomeDoEscudo + " | " + nomeDaArmadura,
        "| " + std::string(11, ' ') + " |  STATUS: " + statusStr
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
