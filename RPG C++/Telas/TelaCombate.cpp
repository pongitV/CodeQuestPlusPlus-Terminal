#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

#include "TelaCombate.h"
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

void TelaCombate::exibirHordaDeInimigosLadoALado(const std::vector<SistemaPersonagem*>& listaDeInimigos) 
{
    if (listaDeInimigos.empty()) return;
    int larguraTerminal = SimplificacoesAparencia::obterLarguraTerminal();
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
        std::cout << "\n";
        
        std::vector<EfeitoID> efeitosAtivos;
        for (size_t indiceInimigo = 0; indiceInimigo < listaDeInimigos.size(); indiceInimigo++)
        {
            std::vector<std::pair<std::string, std::string>> debuffs;
            listaDeInimigos[indiceInimigo]->obterIDsEfeitosAtivos(efeitosAtivos);
            for (auto& id : efeitosAtivos) {
                if (id == EfeitoID::Sangramento) debuffs.push_back({"[Sangramento]", SimplificacoesAparencia::cor(Cor::VERMELHO) + "[Sangramento]" + SimplificacoesAparencia::cor(Cor::RESET)});
                else if (id == EfeitoID::Lentidao) debuffs.push_back({"[Lentidao]", SimplificacoesAparencia::cor(Cor::MAGENTA) + "[Lentidao]" + SimplificacoesAparencia::cor(Cor::RESET)});
                else if (id == EfeitoID::Fraqueza) debuffs.push_back({"[Fraqueza]", SimplificacoesAparencia::cor(Cor::AMARELO) + "[Fraqueza]" + SimplificacoesAparencia::cor(Cor::RESET)});
                else if (id == EfeitoID::QuebraResistencia) debuffs.push_back({"[Quebra Def.]", SimplificacoesAparencia::cor(Cor::CIANO) + "[Quebra Def.]" + SimplificacoesAparencia::cor(Cor::RESET)});
            }

            std::string visualStr = "";
            std::string printStr = "";
            for (size_t i = 0; i < debuffs.size(); ++i) {
                visualStr += debuffs[i].first;
                printStr += debuffs[i].second;
                if (i < debuffs.size() - 1) {
                    visualStr += " ";
                    printStr += " ";
                }
            }

            int espacosEsquerda = (larguraSeparadaParaCadaColuna - (int)visualStr.length()) / 2;
            if (espacosEsquerda < 0) espacosEsquerda = 0;
            int espacosDireita = larguraSeparadaParaCadaColuna - espacosEsquerda - (int)visualStr.length();
            if (espacosDireita < 0) espacosDireita = 0;
            
            std::cout << std::string(espacosEsquerda, ' ') << printStr << std::string(espacosDireita, ' ');
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
