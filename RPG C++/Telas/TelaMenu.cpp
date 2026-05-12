#include "TelaMenu.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

#include "../Utilidades/Aparencia.h"
#include "../Sistemas/SistemaPersonagem.h"
#include "../Utilidades/ControleDeInput.h"
#include "../Racas/RacaBase.h"

namespace {
    int exibirPromptGenerico(const std::string& titulo, const std::string& infoBox, const std::vector<std::string>& narracao, const std::vector<std::string>& opcoes) {
        Aparencia::limparTela();
        TelaMenu::exibirLogoDoJogo(titulo);
        
        if (!infoBox.empty()) {
            std::string cleanInfoBox = infoBox;
            size_t pos = 0;
            while ((pos = cleanInfoBox.find('|', pos)) != std::string::npos) {
                cleanInfoBox.replace(pos, 1, "║");
                pos += 3; // Avança 3 bytes (tamanho de "║" em UTF-8)
            }
            
            int infoLength = Aparencia::obterComprimentoVisual(infoBox);
            std::string tracos = "";
            for (int i = 0; i < infoLength - 2; ++i) tracos += "═";
            
            std::string margem = Aparencia::espacosParaCentralizar(infoLength);
            std::cout << margem << "╔" << tracos << "╗\n";
            std::cout << margem << cleanInfoBox << "\n";
            std::cout << margem << "╚" << tracos << "╝\n\n";
        }
        
        Aparencia::imprimirBlocoCentralizadoDigitando(narracao);
        std::cout << "\n";
        
        return ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);
    }
}

bool TelaMenu::exibirConfirmacaoDeEscolhaComArteLadoALado(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha, const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir) 
{
    Aparencia::limparTela();
    exibirLogoDoJogo("PREVIA DA " + tipoDeEscolha + ": " + nomeDaEscolha);
    int larguraTerminal = Aparencia::obterLarguraTerminal();

    int recuoEsquerdo = Aparencia::imprimirLadoALado(informacoesParaExibir, arteAsciiParaExibir, 40, 6, Cor::RESET, Cor::RESET);

    std::string margem = std::string(recuoEsquerdo, ' ');
    std::cout << "\n";
    std::vector<std::string> opcoes = { "VOLTAR", "CONFIRMAR" };
    int opcaoDeConfirmacao = ControleDeInput::lerSelecaoMenuComSetas(opcoes, false, margem);
    
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

    std::string linhaDupla = "";
    for(int i = 0; i < larguraConsole; ++i) linhaDupla += "═";
    std::cout << "\n" << linhaDupla << "\n\n";

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

int TelaMenu::exibirOpcoesMenuPrincipal(bool temSave) {
    Aparencia::limparTela();
    exibirLogoDoJogo("MENU PRINCIPAL");
    
    std::vector<std::string> opcoes = { "Novo Jogo" };
    if (temSave) opcoes.push_back("Continuar Jogo");
    opcoes.push_back("Sair");

    std::cout << "\n";
    return ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);
}

int TelaMenu::exibirMenuCarregarJogo(const std::vector<std::string>& informacoesSaves) {
    Aparencia::limparTela();
    exibirLogoDoJogo("CARREGAR JOGO");
    
    std::cout << "\n";
    Aparencia::imprimirCentralizado("Selecione o save que deseja carregar:");
    std::cout << "\n";

    std::vector<std::string> opcoes = informacoesSaves;
    opcoes.push_back("Voltar");

    return ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);
}

void TelaMenu::exibirPromptNome() {
    Aparencia::limparTela();
    exibirLogoDoJogo("INTRODUCAO AO RPG");
    
    Aparencia::imprimirBlocoCentralizadoDigitando({
        "[NARRACAO]: O mudno clama por um novo destino...",
        "[NARRACAO]: E todas lendas possuem um nome, Qual o seu?"
    });
    std::cout << "\n";
    Aparencia::exibirPrompt(" > Escolha o nome do seu personagem (ou '0' para sair): ");
}

int TelaMenu::exibirPromptRaca(const std::string& nome) {
    return exibirPromptGenerico("SELECAO DE RACA", "| JOGADOR: " + nome + " |", 
        {"[NARRACAO]: Qual sua origem?"}, 
        {"Dwarf", "Elfo", "Humano", "Ork", "VOLTAR (selecao de nome)"});
}

int TelaMenu::exibirPromptClasse(const std::string& nome, const std::string& nomeRaca) {
    return exibirPromptGenerico("SELECAO DE CLASSE", "| JOGADOR: " + nome + " | RACA: " + nomeRaca + " |", 
        {"[NARRACAO]: Qual seu caminho?"}, 
        {"Arqueiro", "Bardo", "Guerreiro", "Mago", "VOLTAR (selecao de raca)"});
}

int TelaMenu::exibirPromptDificuldade(const std::string& nome, const std::string& nomeRaca, const std::string& nomeClasse) {
    return exibirPromptGenerico("DIFICULDADE DO MUNDO", "| JOGADOR: " + nome + " | RACA: " + nomeRaca + " | CLASSE: " + nomeClasse + " |", 
        {"[SISTEMA]: Escolha o nivel de desafio da sua jornada:"}, 
        {
            "FACIL   (Inimigos com 1x Atributos, sem habilidades adicionais)",
            "NORMAL  (Inimigos com 1.5x Atributos, com habilidades de raca)",
            "DIFICIL (Inimigos com 2x Atributos, com habilidades de raca e classe)",
            "VOLTAR (selecao de classe)"
        });
}

int TelaMenu::exibirPromptParry(const std::string& nome, const std::string& nomeRaca, const std::string& nomeClasse) {
    return exibirPromptGenerico("CONFIGURACOES DO JOGO", "| JOGADOR: " + nome + " | RACA: " + nomeRaca + " | CLASSE: " + nomeClasse + " |", 
        {
            "[SISTEMA]: Deseja ativar o sistema de PARRY?",
            "(Permite reduzir danos ao digitar uma sequencia de numeros num tempo limite)",
            "(O tutorial esta disponivel apenas neste momento, mas voce pode ligar/desligar depois)"
        }, 
        {"Parry LIGADO (Inicia o Tutorial)", "Parry DESLIGADO (Pula o tutorial)", "VOLTAR (selecao de dificuldade)"});
}