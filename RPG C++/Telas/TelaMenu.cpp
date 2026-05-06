#include "TelaMenu.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

#include "../Utilidades/SimplificacoesAparencia.h"
#include "../Sistemas/SistemaPersonagem.h"
#include "../Racas/RacaBase.h"

bool TelaMenu::exibirConfirmacaoDeEscolhaComArteLadoALado(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha, const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir) 
{
    SimplificacoesAparencia::limparTela();
    exibirLogoDoJogo("PREVIA DA " + tipoDeEscolha + ": " + nomeDaEscolha);
    int larguraTerminal = SimplificacoesAparencia::obterLarguraTerminal();

    int recuoEsquerdo = SimplificacoesAparencia::imprimirLadoALado(informacoesParaExibir, arteAsciiParaExibir, 40, 6, Cor::RESET, Cor::RESET);

    std::cout << "\n" << std::string(recuoEsquerdo, ' ') << "0. VOLTAR | 1. CONFIRMAR\n";
    std::cout << std::string(recuoEsquerdo, ' ') << "Escolha: ";
    int opcaoDeConfirmacao; 
    while (!(std::cin >> opcaoDeConfirmacao) || (opcaoDeConfirmacao != 0 && opcaoDeConfirmacao != 1)) { 
        std::cin.clear(); 
        std::cin.ignore(1000, '\n'); 
        std::cout << std::string(recuoEsquerdo, ' ') << "Opcao invalida. Escolha (0 ou 1): "; 
    }
    return opcaoDeConfirmacao == 1;
}

void TelaMenu::exibirLogoDoJogo(const std::string& tituloDaTela) 
{
    int larguraConsole = SimplificacoesAparencia::obterLarguraTerminal();
    
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

    int larguraLinhaCompleta = 140; 

    for (size_t i = 0; i < logoTexto.size(); ++i) 
    {
        int recuo = (larguraConsole - larguraLinhaCompleta) / 2;
        if (recuo < 0) recuo = 0;
        std::cout << std::string(recuo, ' ');

        std::cout << logoTexto[i];
        std::cout << SimplificacoesAparencia::cor(Cor::LARANJA) << logoPlus[i] << SimplificacoesAparencia::cor(Cor::RESET);
        std::cout << "\n";
    }

    std::cout << "\n";
    
    if (tituloDaTela.empty()) 
    {
        SimplificacoesAparencia::imprimirLinhaDivisoria();
        std::cout << "\n";
    } 
    else 
    {
        SimplificacoesAparencia::imprimirLinhaDivisoria();
        std::cout << SimplificacoesAparencia::espacosParaCentralizar(tituloDaTela.length()) << tituloDaTela << "\n";
        SimplificacoesAparencia::imprimirLinhaDivisoria();
        std::cout << "\n";
    }
}

std::vector<std::string> TelaMenu::comporQuadroDeAtributos(const Atributos& stats, const std::string& tituloSecao, const std::string& tituloHabilidade, const std::string& nomeHab, const std::string& descHab) {
    auto formatarAtributo = [](const std::string& nomeAtr, int valorAtr) { 
        return " - " + nomeAtr + ": " + (valorAtr >= 0 ? "+" : "") + std::to_string(valorAtr); 
    };
    return {
        tituloSecao,
        formatarAtributo("Vida", stats.vida),
        formatarAtributo("Forca", stats.forca),
        formatarAtributo("Destreza", stats.destreza),
        formatarAtributo("Resistencia", stats.resistencia),
        formatarAtributo("Constituicao", stats.constituicao),
        formatarAtributo("Inteligencia", stats.inteligencia),
        formatarAtributo("Sabedoria", stats.sabedoria),
        "",
        tituloHabilidade,
        " " + nomeHab,
        " - " + descHab
    };
}

void TelaMenu::exibirOpcoesMenuPrincipal(bool temSave) {
    SimplificacoesAparencia::limparTela();
    exibirLogoDoJogo("MENU PRINCIPAL");
    std::cout << "\n";
    int espacos = std::max(0, (SimplificacoesAparencia::obterLarguraTerminal() - 20) / 2);
    std::string margemEsquerda(espacos, ' ');
    std::cout << margemEsquerda << "[1] Novo Jogo\n";
    if (temSave) std::cout << margemEsquerda << "[2] Continuar Jogo\n";
    std::cout << margemEsquerda << "[0] Sair\n\n";
    std::cout << margemEsquerda << "Escolha: ";
}

void TelaMenu::exibirMenuCarregarJogo(const std::vector<std::string>& informacoesSaves) {
    SimplificacoesAparencia::limparTela();
    exibirLogoDoJogo("CARREGAR JOGO");
    int espacos = std::max(0, (SimplificacoesAparencia::obterLarguraTerminal() - 20) / 2);
    std::string margemEsquerda(espacos, ' ');
    std::cout << "\n" << margemEsquerda << "Selecione o save que deseja carregar:\n\n";
    for (const std::string& info : informacoesSaves) {
        std::cout << margemEsquerda << info << "\n";
    }
    std::cout << "\n" << margemEsquerda << "[0] Voltar\n\n";
    std::cout << margemEsquerda << "Escolha: ";
}

void TelaMenu::exibirPromptNome() {
    SimplificacoesAparencia::limparTela();
    exibirLogoDoJogo("INTRODUCAO AO RPG");
    SimplificacoesAparencia::imprimirDigitando(" [NARRACAO]: O reino clama por um novo destino...\n", 35);
    SimplificacoesAparencia::imprimirDigitando(" [NARRACAO]: E todas lendas possuem um nome.\n\n", 35);
    std::cout << " > Escolha o nome do seu personagem (ou '0' para sair): ";
}

void TelaMenu::exibirPromptRaca(const std::string& nome) {
    SimplificacoesAparencia::limparTela();
    exibirLogoDoJogo("SELECAO DE RACA");
    std::cout << "JOGADOR: " << nome << "\n";
    std::cout << std::string(SimplificacoesAparencia::obterLarguraTerminal(), '-') << "\n";
    SimplificacoesAparencia::imprimirDigitando(" [NARRACAO]: Qual sua origem?\n\n", 35);
    std::cout << "  [1] Dwarf\n  [2] Elfo\n  [3] Humano\n  [4] Ork\n\n  [0] VOLTAR (selecao de nome)\n\n > Sua escolha: ";
}

void TelaMenu::exibirPromptClasse(const std::string& nome, const std::string& nomeRaca) {
    SimplificacoesAparencia::limparTela();
    exibirLogoDoJogo("SELECAO DE CLASSE");
    std::cout << "JOGADOR: " << nome << " | RACA: " << nomeRaca << "\n";
    std::cout << std::string(SimplificacoesAparencia::obterLarguraTerminal(), '-') << "\n";
    SimplificacoesAparencia::imprimirDigitando(" [NARRACAO]: Qual caminho voce seguira neste mundo?\n\n", 35);
    std::cout << "  [1] Arqueiro\n  [2] Bardo\n  [3] Guerreiro\n  [4] Mago\n\n  [0] VOLTAR (selecao de raca)\n\n > Sua escolha: ";
}

void TelaMenu::exibirPromptParry(const std::string& nome, const std::string& nomeRaca, const std::string& nomeClasse) {
    SimplificacoesAparencia::limparTela();
    exibirLogoDoJogo("CONFIGURACOES DO JOGO");
    std::cout << "JOGADOR: " << nome << " | RACA: " << nomeRaca << " | CLASSE: " << nomeClasse << "\n";
    std::cout << std::string(SimplificacoesAparencia::obterLarguraTerminal(), '-') << "\n";
    SimplificacoesAparencia::imprimirDigitando(" [SISTEMA]: Deseja ativar o sistema de PARRY?\n\n", 35);
    SimplificacoesAparencia::imprimirDigitando(" (Permite reduzir danos ao digitar uma sequencia de numeros num tempo limite)\n\n", 35);
    std::cout << "  [1] LIGAR Parry\n  [2] DESLIGAR Parry\n\n  [0] VOLTAR (selecao de classe)\n\n > Sua escolha: ";
}

void TelaMenu::exibirPromptDificuldade(const std::string& nome, const std::string& nomeRaca, const std::string& nomeClasse) {
    SimplificacoesAparencia::limparTela();
    exibirLogoDoJogo("DIFICULDADE DO MUNDO");
    std::cout << "JOGADOR: " << nome << " | RACA: " << nomeRaca << " | CLASSE: " << nomeClasse << "\n";
    std::cout << std::string(SimplificacoesAparencia::obterLarguraTerminal(), '-') << "\n";
    SimplificacoesAparencia::imprimirDigitando(" [SISTEMA]: Escolha o nivel de desafio da sua jornada:\n\n", 35);
    std::cout << "  [1] FACIL   (Inimigos com 1x Atributos, sem habilidades de raca e sem classe)\n";
    std::cout << "  [2] NORMAL  (Inimigos com 1.5x Atributos, com habilidades de raca mas sem classes)\n";
    std::cout << "  [3] DIFICIL (Inimigos com 2x Atributos, com habilidades de raca e com classes)\n";
    std::cout << "\n  [0] VOLTAR (configuracao de parry)\n\n > Sua escolha: ";
}
