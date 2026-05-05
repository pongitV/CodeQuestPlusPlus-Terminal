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

    int larguraArte = 0;
    for (const std::string& l : arteAsciiParaExibir) if ((int)l.length() > larguraArte) larguraArte = (int)l.length();
    
    int larguraInfo = 40;
    for (const std::string& s : informacoesParaExibir) if ((int)s.length() > larguraInfo) larguraInfo = (int)s.length();
    
    int espacamentoEntreColunas = 6;
    int recuoEsquerdo = (larguraTerminal - (larguraInfo + espacamentoEntreColunas + larguraArte)) / 2;
    if (recuoEsquerdo < 0) recuoEsquerdo = 0;

    size_t maximoDeLinhas = std::max(arteAsciiParaExibir.size(), informacoesParaExibir.size());
    for (size_t i = 0; i < maximoDeLinhas; ++i) 
    {
        std::cout << std::string(recuoEsquerdo, ' ');
        if (i < informacoesParaExibir.size()) std::cout << std::left << std::setw(larguraInfo) << informacoesParaExibir[i];
        else std::cout << std::string(larguraInfo, ' ');
        std::cout << std::string(espacamentoEntreColunas, ' ');
        if (i < arteAsciiParaExibir.size()) std::cout << arteAsciiParaExibir[i];
        std::cout << "\n";
    }

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

std::vector<std::string> TelaMenu::comporEstatisticasBatalha(SistemaPersonagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate) {
    return {
        "NOME:           " + jogadorAtual->obterNome(),
        "RACA:           " + jogadorAtual->obterRaca()->obterNomeRaca(),
        "CLASSE:         " + jogadorAtual->obterNomeClasse(),
        "HP RESTANTE:    " + std::to_string(jogadorAtual->obterVida()) + "/" + std::to_string(jogadorAtual->obterVidaMaxima()),
        "OURO TOTAL:     " + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G",
        "NIVEL:          " + std::to_string(jogadorAtual->obterNivel()) + " (XP: " + std::to_string(jogadorAtual->obterXpAtual()) + "/" + std::to_string(jogadorAtual->obterXpParaSubir()) + ")",
        "",
        "--- ESTATISTICAS DA BATALHA ---",
        "OURO OBTIDO:   +" + std::to_string(quantidadeDeOuroObtido) + "G",
        "XP OBTIDO:     +" + std::to_string(quantidadeDeXpObtido) + " XP",
        "DANO TOTAL CAUSADO:   " + std::to_string(totalDeDanoCausado),
        "DANO TOTAL RECEBIDO:  " + std::to_string(totalDeDanoRecebido),
        "CURA TOTAL RECEBIDA:  " + std::to_string(curaTotalRecebida),
        "NUMERO DE TURNOS:         " + std::to_string(turnosCombate)
    };
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
