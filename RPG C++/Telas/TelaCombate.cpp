#include <iostream>
#include <vector>
#include <string>

#include "TelaCombate.h"
#include "../Gerenciadores/GerenciadorMenu.h"
#include "../Sistemas/SistemaPersonagem.h"
#include "../Utilidades/SimplificacoesAparencia.h"
#include "../Racas/RacaBase.h"
#include "../Classes/ClasseBase.h"
#include "../Inventario/Item.h"

namespace {
    std::vector<std::string> gerarArteCoracao(double porcentagemDeVida, const std::string& corVerde, const std::string& corLaranja, const std::string& corVermelho, const std::string& corReset) {
        std::vector<std::string> arte;
        std::string corAtual = (porcentagemDeVida > 0.70) ? corVerde : (porcentagemDeVida > 0.30) ? corLaranja : corVermelho;
        if (porcentagemDeVida > 0.70)      arte = { "   _   _   ", "  / \\_/ \\  ", "  \\     /  ", "   \\___/   " };
        else if (porcentagemDeVida > 0.30) arte = { "   _   _   ", "  / \\// \\  ", "  \\  \\ /   ", "   \\___/   " };
        else                               arte = { "  _     _  ", " / \\   / \\ ", " \\     \\_/ ", "  \\___/    " };
        for (auto& linha : arte) linha = corAtual + linha + corReset;
        return arte;
    }

    std::string gerarBarraDeXp(SistemaPersonagem* jogadorAtual, const std::string& corAzul, const std::string& corReset) {
        int tamanho = 10;
        int preenchido = std::min(tamanho, (jogadorAtual->obterXpAtual() * tamanho) / std::max(1, jogadorAtual->obterXpParaSubir()));
        std::string barra = "[" + corAzul + std::string(preenchido, '#') + corReset + std::string(tamanho - preenchido, '-') + "] ";
        return barra + corAzul + std::to_string(jogadorAtual->obterXpAtual()) + corReset + "/" + std::to_string(jogadorAtual->obterXpParaSubir());
    }

    std::string gerarStringDeStatus(SistemaPersonagem* jogadorAtual, const std::string& cVerde, const std::string& cLaranja, const std::string& cVermelho, const std::string& cCiano, const std::string& cAzul, const std::string& cMagenta, const std::string& cReset) {
        std::string status = "";
        if (jogadorAtual->possuiEfeito(EfeitoID::BuffAtributos)) status += cVerde + "[Buff Atributos]" + cReset + " ";
        if (jogadorAtual->possuiEfeito(EfeitoID::MetadeDano)) status += cCiano + "[Metade Dano]" + cReset + " ";
        if (jogadorAtual->possuiEfeito(EfeitoID::Inviolavel)) status += cAzul + "[Inviolavel]" + cReset + " ";
        if (jogadorAtual->possuiEfeito(EfeitoID::Sangramento)) status += cVermelho + "[Sangrando]" + cReset + " ";
        if (jogadorAtual->possuiEfeito(EfeitoID::Lentidao)) status += cMagenta + "[Lento]" + cReset + " ";
        if (jogadorAtual->possuiEfeito(EfeitoID::Fraqueza)) status += cLaranja + "[Fraqueza]" + cReset + " ";
        if (jogadorAtual->possuiEfeito(EfeitoID::QuebraResistencia)) status += cCiano + "[Quebra Def.]" + cReset + " ";
        return status.empty() ? "Nenhum" : status;
    }
}

void TelaCombate::exibirLogoParaTelaDeCombate(const std::string& tituloDaTela) 
{
    int larguraConsole = SimplificacoesAparencia::obterLarguraTerminal();
    
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
    SimplificacoesAparencia::imprimirCentralizadoMultilinha(logo, 95, SimplificacoesAparencia::cor(Cor::VERMELHO));

    std::cout << "\n";
    
    std::cout << std::string(larguraConsole, '=') << "\n";
    int espacos = (larguraConsole - (int)tituloDaTela.length()) / 2;
    std::cout << std::string(espacos > 0 ? espacos : 0, ' ') << tituloDaTela << "\n";
    std::cout << std::string(larguraConsole, '=') << "\n\n";
}

void TelaCombate::exibirBarraDeStatusDoJogador(SistemaPersonagem* jogadorAtual) 
{
    if (jogadorAtual == nullptr) return;
    int larguraDoTerminal = SimplificacoesAparencia::obterLarguraTerminal();
    std::string nomeDaArma = (jogadorAtual->obterArma()) ? jogadorAtual->obterArma()->obterNomeItem() + jogadorAtual->obterArma()->obterInfoStatus() : "Punhos";
    std::string nomeDoEscudo = (jogadorAtual->obterEscudo()) ? jogadorAtual->obterEscudo()->obterNomeItem() + jogadorAtual->obterEscudo()->obterInfoStatus() : "Nenhum";
    std::string nomeDaArmadura = (jogadorAtual->obterArmadura()) ? jogadorAtual->obterArmadura()->obterNomeItem() + jogadorAtual->obterArmadura()->obterInfoStatus() : "Trapos";
    
    double porcentagemDeVida = static_cast<double>(jogadorAtual->obterVida()) / jogadorAtual->obterVidaMaxima();
    std::string corVerde = SimplificacoesAparencia::cor(Cor::VERDE);
    std::string corLaranja = SimplificacoesAparencia::cor(Cor::AMARELO);
    std::string corVermelho = SimplificacoesAparencia::cor(Cor::VERMELHO);
    std::string corAzul = SimplificacoesAparencia::cor(Cor::AZUL);
    std::string corCiano = SimplificacoesAparencia::cor(Cor::CIANO);
    std::string corMagenta = SimplificacoesAparencia::cor(Cor::MAGENTA);
    std::string corReset = SimplificacoesAparencia::cor(Cor::RESET);
    
    std::string corVida = (porcentagemDeVida > 0.70) ? corVerde : (porcentagemDeVida > 0.30) ? corLaranja : corVermelho;
    auto arteDoCoracao = gerarArteCoracao(porcentagemDeVida, corVerde, corLaranja, corVermelho, corReset);
    std::string arteDeBarraDeXp = gerarBarraDeXp(jogadorAtual, corAzul, corReset);
    std::string statusStr = gerarStringDeStatus(jogadorAtual, corVerde, corLaranja, corVermelho, corCiano, corAzul, corMagenta, corReset);

    // Aplicando a cor dinâmica ao HP na linha do status
    std::vector<std::string> linhasParaImprimir = 
    {
        "| " + arteDoCoracao[0] + " |",
        "| " + arteDoCoracao[1] + " |  JOGADOR: " + jogadorAtual->obterNome() + " (" + jogadorAtual->obterRaca()->obterNomeRaca() + " / " + jogadorAtual->obterNomeClasse() + ") | NIVEL: " + std::to_string(jogadorAtual->obterNivel()),
        "| " + arteDoCoracao[2] + " |  HP: " + corVida + std::to_string(jogadorAtual->obterVida()) + corReset + "/" + std::to_string(jogadorAtual->obterVidaMaxima()) + " | OURO: " + corLaranja + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G" + corReset + " | XP: " + arteDeBarraDeXp,
        "| " + arteDoCoracao[3] + " |  EQUIP: " + nomeDaArma + " | " + nomeDoEscudo + " | " + nomeDaArmadura,
        "| " + std::string(11, ' ') + " |  STATUS: " + statusStr
    };

    int maxLen = 0;
    for (const std::string& linhaDeTextoAtual : linhasParaImprimir) 
        if ((int)linhaDeTextoAtual.length() > maxLen) maxLen = (int)linhaDeTextoAtual.length();
    std::string margemEsquerda(std::max(0, (larguraDoTerminal - maxLen) / 2), ' ');

    std::cout << std::string(larguraDoTerminal, '=') << "\n";
    for (const std::string& linhaDeTextoAtual : linhasParaImprimir) 
        std::cout << margemEsquerda << linhaDeTextoAtual << "\n";
    std::cout << std::string(larguraDoTerminal, '=') << "\n";
}
