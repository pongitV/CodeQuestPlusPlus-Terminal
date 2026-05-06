#include "TelaMenu.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

#include "../Utilidades/Aparencia.h"
#include "../Sistemas/SistemaPersonagem.h"
#include "../Racas/RacaBase.h"

bool TelaMenu::exibirConfirmacaoDeEscolhaComArteLadoALado(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha, const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir) 
{
    Aparencia::limparTela();
    exibirLogoDoJogo("PREVIA DA " + tipoDeEscolha + ": " + nomeDaEscolha);
    int larguraTerminal = Aparencia::obterLarguraTerminal();

    int recuoEsquerdo = Aparencia::imprimirLadoALado(informacoesParaExibir, arteAsciiParaExibir, 40, 6, Cor::RESET, Cor::RESET);

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
    int larguraConsole = Aparencia::obterLarguraTerminal();
    
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
        std::cout << Aparencia::cor(Cor::LARANJA) << logoPlus[i] << Aparencia::cor(Cor::RESET);
        std::cout << "\n";
    }

    std::cout << "\n";
    
    if (tituloDaTela.empty()) 
    {
        Aparencia::imprimirLinhaDivisoria();
    } 
    else 
    {
        Aparencia::imprimirLinhaDivisoria();
        Aparencia::imprimirCentralizado(tituloDaTela);
        Aparencia::imprimirLinhaDivisoria();
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
    Aparencia::limparTela();
    exibirLogoDoJogo("MENU PRINCIPAL");
    
    std::vector<std::string> opcoes = { "[1] Novo Jogo" };
    if (temSave) opcoes.push_back("[2] Continuar Jogo");
    opcoes.push_back("[0] Sair");

    std::cout << "\n";
    Aparencia::imprimirBlocoCentralizado(opcoes);
    std::cout << "\n";
    Aparencia::exibirPrompt("Escolha: ");
}

void TelaMenu::exibirMenuCarregarJogo(const std::vector<std::string>& informacoesSaves) {
    Aparencia::limparTela();
    exibirLogoDoJogo("CARREGAR JOGO");
    
    std::cout << "\n";
    Aparencia::imprimirCentralizado("Selecione o save que deseja carregar:");
    std::cout << "\n";

    std::vector<std::string> opcoes = informacoesSaves;
    opcoes.push_back("");
    opcoes.push_back("[0] Voltar");

    Aparencia::imprimirBlocoCentralizado(opcoes);
    std::cout << "\n";
    Aparencia::exibirPrompt("Escolha: ");
}

void TelaMenu::exibirPromptNome() {
    Aparencia::limparTela();
    exibirLogoDoJogo("INTRODUCAO AO RPG");
    std::string t1 = "[NARRACAO]: O reino clama por um novo destino...";
    std::string t2 = "[NARRACAO]: E todas lendas possuem um nome.";
    
    int maxLen = std::max(t1.length(), t2.length());
    std::string margem = Aparencia::espacosParaCentralizar(maxLen);
    Aparencia::imprimirDigitando(margem + t1 + "\n", 35);
    Aparencia::imprimirDigitando(margem + t2 + "\n\n", 35);
    Aparencia::exibirPrompt(" > Escolha o nome do seu personagem (ou '0' para sair): ");
}

void TelaMenu::exibirPromptRaca(const std::string& nome) {
    Aparencia::limparTela();
    exibirLogoDoJogo("SELECAO DE RACA");
    
    std::string info = "| JOGADOR: " + nome + " |";
    std::string borda = "+" + std::string(info.length() - 2, '-') + "+";
    Aparencia::imprimirCentralizado(borda);
    Aparencia::imprimirCentralizado(info);
    Aparencia::imprimirCentralizado(borda);
    std::cout << "\n";
    
    std::string t1 = "[NARRACAO]: Qual sua origem?";
    Aparencia::imprimirDigitando(Aparencia::espacosParaCentralizar(t1.length()) + t1 + "\n\n", 35);
    
    std::vector<std::string> opcoes = {
        "[1] Dwarf", "[2] Elfo", "[3] Humano", "[4] Ork", "", "[0] VOLTAR (selecao de nome)"
    };
    Aparencia::imprimirBlocoCentralizado(opcoes);
    Aparencia::exibirPrompt(" > Sua escolha: ");
}

void TelaMenu::exibirPromptClasse(const std::string& nome, const std::string& nomeRaca) {
    Aparencia::limparTela();
    exibirLogoDoJogo("SELECAO DE CLASSE");
    
    std::string info = "| JOGADOR: " + nome + " | RACA: " + nomeRaca + " |";
    std::string borda = "+" + std::string(info.length() - 2, '-') + "+";
    Aparencia::imprimirCentralizado(borda);
    Aparencia::imprimirCentralizado(info);
    Aparencia::imprimirCentralizado(borda);
    std::cout << "\n";
    
    std::string t1 = "[NARRACAO]: Qual caminho voce seguira neste mundo?";
    Aparencia::imprimirDigitando(Aparencia::espacosParaCentralizar(t1.length()) + t1 + "\n\n", 35);

    std::vector<std::string> opcoes = {
        "[1] Arqueiro", "[2] Bardo", "[3] Guerreiro", "[4] Mago", "", "[0] VOLTAR (selecao de raca)"
    };
    Aparencia::imprimirBlocoCentralizado(opcoes);
    Aparencia::exibirPrompt(" > Sua escolha: ");
}

void TelaMenu::exibirPromptParry(const std::string& nome, const std::string& nomeRaca, const std::string& nomeClasse) {
    Aparencia::limparTela();
    exibirLogoDoJogo("CONFIGURACOES DO JOGO");
    
    std::string info = "| JOGADOR: " + nome + " | RACA: " + nomeRaca + " | CLASSE: " + nomeClasse + " |";
    std::string borda = "+" + std::string(info.length() - 2, '-') + "+";
    Aparencia::imprimirCentralizado(borda);
    Aparencia::imprimirCentralizado(info);
    Aparencia::imprimirCentralizado(borda);
    std::cout << "\n";
    
    std::string t1 = "[SISTEMA]: Deseja ativar o sistema de PARRY?";
    std::string t2 = "(Permite reduzir danos ao digitar uma sequencia de numeros num tempo limite)";
    
    int maxLen = std::max(t1.length(), t2.length());
    std::string margem = Aparencia::espacosParaCentralizar(maxLen);
    Aparencia::imprimirDigitando(margem + t1 + "\n", 35);
    Aparencia::imprimirDigitando(margem + t2 + "\n\n", 35);
    
    std::vector<std::string> opcoes = {
        "[1] LIGAR Parry", "[2] DESLIGAR Parry", "", "[0] VOLTAR (selecao de classe)"
    };
    Aparencia::imprimirBlocoCentralizado(opcoes);
    Aparencia::exibirPrompt(" > Sua escolha: ");
}

void TelaMenu::exibirPromptDificuldade(const std::string& nome, const std::string& nomeRaca, const std::string& nomeClasse) {
    Aparencia::limparTela();
    exibirLogoDoJogo("DIFICULDADE DO MUNDO");
    
    std::string info = "| JOGADOR: " + nome + " | RACA: " + nomeRaca + " | CLASSE: " + nomeClasse + " |";
    std::string borda = "+" + std::string(info.length() - 2, '-') + "+";
    Aparencia::imprimirCentralizado(borda);
    Aparencia::imprimirCentralizado(info);
    Aparencia::imprimirCentralizado(borda);
    std::cout << "\n";
    
    std::string t1 = "[SISTEMA]: Escolha o nivel de desafio da sua jornada:";
    Aparencia::imprimirDigitando(Aparencia::espacosParaCentralizar(t1.length()) + t1 + "\n\n", 35);
    
    std::vector<std::string> opcoes = {
        "[1] FACIL   (Inimigos com 1x Atributos, sem hab. raca/classe)",
        "[2] NORMAL  (Inimigos com 1.5x Atributos, com hab. raca)",
        "[3] DIFICIL (Inimigos com 2x Atributos, com hab. raca/classe)",
        "",
        "[0] VOLTAR (configuracao de parry)"
    };
    Aparencia::imprimirBlocoCentralizado(opcoes);
    Aparencia::exibirPrompt(" > Sua escolha: ");
}
