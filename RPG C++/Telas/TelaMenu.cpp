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
    } 
    else 
    {
        SimplificacoesAparencia::imprimirLinhaDivisoria();
        SimplificacoesAparencia::imprimirCentralizado(tituloDaTela);
        SimplificacoesAparencia::imprimirLinhaDivisoria();
    }
    std::cout << "\n";
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
    
    std::vector<std::string> opcoes = { "[1] Novo Jogo" };
    if (temSave) opcoes.push_back("[2] Continuar Jogo");
    opcoes.push_back("[0] Sair");

    std::cout << "\n";
    SimplificacoesAparencia::imprimirBlocoCentralizado(opcoes);
    std::cout << "\n";
    SimplificacoesAparencia::exibirPrompt("Escolha: ");
}

void TelaMenu::exibirMenuCarregarJogo(const std::vector<std::string>& informacoesSaves) {
    SimplificacoesAparencia::limparTela();
    exibirLogoDoJogo("CARREGAR JOGO");
    
    std::cout << "\n";
    SimplificacoesAparencia::imprimirCentralizado("Selecione o save que deseja carregar:");
    std::cout << "\n";

    std::vector<std::string> opcoes = informacoesSaves;
    opcoes.push_back("");
    opcoes.push_back("[0] Voltar");

    SimplificacoesAparencia::imprimirBlocoCentralizado(opcoes);
    std::cout << "\n";
    SimplificacoesAparencia::exibirPrompt("Escolha: ");
}

void TelaMenu::exibirPromptNome() {
    SimplificacoesAparencia::limparTela();
    exibirLogoDoJogo("INTRODUCAO AO RPG");
    std::string t1 = "[NARRACAO]: O reino clama por um novo destino...";
    std::string t2 = "[NARRACAO]: E todas lendas possuem um nome.";
    
    int maxLen = std::max(t1.length(), t2.length());
    std::string margem = SimplificacoesAparencia::espacosParaCentralizar(maxLen);
    SimplificacoesAparencia::imprimirDigitando(margem + t1 + "\n", 35);
    SimplificacoesAparencia::imprimirDigitando(margem + t2 + "\n\n", 35);
    SimplificacoesAparencia::exibirPrompt(" > Escolha o nome do seu personagem (ou '0' para sair): ");
}

void TelaMenu::exibirPromptRaca(const std::string& nome) {
    SimplificacoesAparencia::limparTela();
    exibirLogoDoJogo("SELECAO DE RACA");
    
    std::string info = "| JOGADOR: " + nome + " |";
    std::string borda = "+" + std::string(info.length() - 2, '-') + "+";
    SimplificacoesAparencia::imprimirCentralizado(borda);
    SimplificacoesAparencia::imprimirCentralizado(info);
    SimplificacoesAparencia::imprimirCentralizado(borda);
    std::cout << "\n";
    
    std::string t1 = "[NARRACAO]: Qual sua origem?";
    SimplificacoesAparencia::imprimirDigitando(SimplificacoesAparencia::espacosParaCentralizar(t1.length()) + t1 + "\n\n", 35);
    
    std::vector<std::string> opcoes = {
        "[1] Dwarf", "[2] Elfo", "[3] Humano", "[4] Ork", "", "[0] VOLTAR (selecao de nome)"
    };
    SimplificacoesAparencia::imprimirBlocoCentralizado(opcoes);
    SimplificacoesAparencia::exibirPrompt(" > Sua escolha: ");
}

void TelaMenu::exibirPromptClasse(const std::string& nome, const std::string& nomeRaca) {
    SimplificacoesAparencia::limparTela();
    exibirLogoDoJogo("SELECAO DE CLASSE");
    
    std::string info = "| JOGADOR: " + nome + " | RACA: " + nomeRaca + " |";
    std::string borda = "+" + std::string(info.length() - 2, '-') + "+";
    SimplificacoesAparencia::imprimirCentralizado(borda);
    SimplificacoesAparencia::imprimirCentralizado(info);
    SimplificacoesAparencia::imprimirCentralizado(borda);
    std::cout << "\n";
    
    std::string t1 = "[NARRACAO]: Qual caminho voce seguira neste mundo?";
    SimplificacoesAparencia::imprimirDigitando(SimplificacoesAparencia::espacosParaCentralizar(t1.length()) + t1 + "\n\n", 35);

    std::vector<std::string> opcoes = {
        "[1] Arqueiro", "[2] Bardo", "[3] Guerreiro", "[4] Mago", "", "[0] VOLTAR (selecao de raca)"
    };
    SimplificacoesAparencia::imprimirBlocoCentralizado(opcoes);
    SimplificacoesAparencia::exibirPrompt(" > Sua escolha: ");
}

void TelaMenu::exibirPromptParry(const std::string& nome, const std::string& nomeRaca, const std::string& nomeClasse) {
    SimplificacoesAparencia::limparTela();
    exibirLogoDoJogo("CONFIGURACOES DO JOGO");
    
    std::string info = "| JOGADOR: " + nome + " | RACA: " + nomeRaca + " | CLASSE: " + nomeClasse + " |";
    std::string borda = "+" + std::string(info.length() - 2, '-') + "+";
    SimplificacoesAparencia::imprimirCentralizado(borda);
    SimplificacoesAparencia::imprimirCentralizado(info);
    SimplificacoesAparencia::imprimirCentralizado(borda);
    std::cout << "\n";
    
    std::string t1 = "[SISTEMA]: Deseja ativar o sistema de PARRY?";
    std::string t2 = "(Permite reduzir danos ao digitar uma sequencia de numeros num tempo limite)";
    
    int maxLen = std::max(t1.length(), t2.length());
    std::string margem = SimplificacoesAparencia::espacosParaCentralizar(maxLen);
    SimplificacoesAparencia::imprimirDigitando(margem + t1 + "\n", 35);
    SimplificacoesAparencia::imprimirDigitando(margem + t2 + "\n\n", 35);
    
    std::vector<std::string> opcoes = {
        "[1] LIGAR Parry", "[2] DESLIGAR Parry", "", "[0] VOLTAR (selecao de classe)"
    };
    SimplificacoesAparencia::imprimirBlocoCentralizado(opcoes);
    SimplificacoesAparencia::exibirPrompt(" > Sua escolha: ");
}

void TelaMenu::exibirPromptDificuldade(const std::string& nome, const std::string& nomeRaca, const std::string& nomeClasse) {
    SimplificacoesAparencia::limparTela();
    exibirLogoDoJogo("DIFICULDADE DO MUNDO");
    
    std::string info = "| JOGADOR: " + nome + " | RACA: " + nomeRaca + " | CLASSE: " + nomeClasse + " |";
    std::string borda = "+" + std::string(info.length() - 2, '-') + "+";
    SimplificacoesAparencia::imprimirCentralizado(borda);
    SimplificacoesAparencia::imprimirCentralizado(info);
    SimplificacoesAparencia::imprimirCentralizado(borda);
    std::cout << "\n";
    
    std::string t1 = "[SISTEMA]: Escolha o nivel de desafio da sua jornada:";
    SimplificacoesAparencia::imprimirDigitando(SimplificacoesAparencia::espacosParaCentralizar(t1.length()) + t1 + "\n\n", 35);
    
    std::vector<std::string> opcoes = {
        "[1] FACIL   (Inimigos com 1x Atributos, sem hab. raca/classe)",
        "[2] NORMAL  (Inimigos com 1.5x Atributos, com hab. raca)",
        "[3] DIFICIL (Inimigos com 2x Atributos, com hab. raca/classe)",
        "",
        "[0] VOLTAR (configuracao de parry)"
    };
    SimplificacoesAparencia::imprimirBlocoCentralizado(opcoes);
    SimplificacoesAparencia::exibirPrompt(" > Sua escolha: ");
}
