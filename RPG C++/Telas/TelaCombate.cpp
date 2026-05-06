#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <thread>
#include <chrono>

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

    std::string gerarStringDeStatus(SistemaPersonagem* jogadorAtual, const std::string& corVerde, const std::string& corLaranja, const std::string& corVermelho, const std::string& corCiano, const std::string& corAzul, const std::string& corMagenta, const std::string& corReset) {
        std::string status = "";
        if (jogadorAtual->possuiEfeito(EfeitoID::BuffAtributos)) status += corVerde + "[Buff Atributos]" + corReset + " ";
        if (jogadorAtual->possuiEfeito(EfeitoID::MetadeDano)) status += corCiano + "[Metade Dano]" + corReset + " ";
        if (jogadorAtual->possuiEfeito(EfeitoID::Inviolavel)) status += corAzul + "[Inviolavel]" + corReset + " ";
        if (jogadorAtual->possuiEfeito(EfeitoID::Sangramento)) status += corVermelho + "[Sangrando]" + corReset + " ";
        if (jogadorAtual->possuiEfeito(EfeitoID::Lentidao)) status += corMagenta + "[Lento]" + corReset + " ";
        if (jogadorAtual->possuiEfeito(EfeitoID::Fraqueza)) status += corLaranja + "[Fraqueza]" + corReset + " ";
        if (jogadorAtual->possuiEfeito(EfeitoID::QuebraResistencia)) status += corCiano + "[Quebra Def.]" + corReset + " ";
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

    SimplificacoesAparencia::exibirLogoAscii(logo, 95, Cor::VERMELHO, tituloDaTela);
}

void TelaCombate::exibirBarraDeStatusDoJogador(SistemaPersonagem* jogadorAtual) 
{
    if (jogadorAtual == nullptr) return;
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

    SimplificacoesAparencia::imprimirLinhaDivisoria();
    SimplificacoesAparencia::imprimirCentralizadoMultilinha(linhasParaImprimir, 95);
    SimplificacoesAparencia::imprimirLinhaDivisoria();
}

void TelaCombate::exibirHordaDeInimigosLadoALado(const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* alvoAnimacao, int frameAnimacao, bool isCura) 
{
    if (listaDeInimigos.empty()) return;
    int larguraTerminal = SimplificacoesAparencia::obterLarguraTerminal();
    const std::vector<std::string>& arteDoInimigo = listaDeInimigos[0]->obterRaca()->obterAparenciaRaca();
    int quantidadeTotalDeInimigosNaHorda = static_cast<int>(listaDeInimigos.size());
    int larguraSeparadaParaCadaColuna = larguraTerminal / quantidadeTotalDeInimigosNaHorda; 

    SimplificacoesAparencia::imprimirLinhaDivisoria('-');
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
            std::string espacos(espacosParaCentralizarAArte > 0 ? espacosParaCentralizarAArte : 0, ' ');
            
            SistemaPersonagem* inimigoAtual = listaDeInimigos[indiceDoInimigoParaDesenhar];
            std::string linhaArte = arteDoInimigo[indiceDaLinhaDaArte];
            
            std::cout << espacos;
            
            if (inimigoAtual == alvoAnimacao && frameAnimacao > 0) {
                if (frameAnimacao % 2 == 1) {
                    Cor corDestaque = isCura ? Cor::VERDE : Cor::VERMELHO;
                    std::cout << SimplificacoesAparencia::cor(corDestaque) << linhaArte << SimplificacoesAparencia::cor(Cor::RESET);
                } else {
                    std::cout << std::string(linhaArte.length(), ' ');
                }
            } else {
                std::cout << linhaArte;
            }
            
            int espacosDireita = larguraSeparadaParaCadaColuna - espacosParaCentralizarAArte - (int)linhaArte.length();
            std::cout << std::string(espacosDireita > 0 ? espacosDireita : 0, ' ');
        }
        std::cout << "\n";
    }
    SimplificacoesAparencia::imprimirLinhaDivisoria('-');
    std::cout << "\n";
}

std::vector<std::string> TelaCombate::comporEstatisticasBatalha(SistemaPersonagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate) 
{
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

void TelaCombate::animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* alvoAnimacao, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& listaDeAliados)
{
    int cursorYAnterior = SimplificacoesAparencia::obterPosicaoCursorY();

    for (int frame = 1; frame <= 4; ++frame) {
        SimplificacoesAparencia::moverCursor(0, 0); // Desenha por cima (sem piscar a tela toda)
        exibirLogoParaTelaDeCombate(tituloCombate);
        exibirHordaDeInimigosLadoALado(listaDeInimigos, alvoAnimacao, frame);
        exibirBarraDeStatusDoJogador(jogadorAtual);
        for (auto* aliado : listaDeAliados) {
            exibirBarraDeStatusDoJogador(aliado);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Desenha o ultimo quadro (Normal) sem limpar a tela
    SimplificacoesAparencia::moverCursor(0, 0);
    exibirLogoParaTelaDeCombate(tituloCombate);
    exibirHordaDeInimigosLadoALado(listaDeInimigos, alvoAnimacao, 0);
    exibirBarraDeStatusDoJogador(jogadorAtual);
    for (auto* aliado : listaDeAliados) {
        exibirBarraDeStatusDoJogador(aliado);
    }
    
    // Retorna o cursor lá pro fundo da tela para o jogo continuar imprimindo o log de combate!
    SimplificacoesAparencia::moverCursor(0, cursorYAnterior); 
}

void TelaCombate::animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* alvoAnimacao, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& listaDeAliados)
{
    int cursorYAnterior = SimplificacoesAparencia::obterPosicaoCursorY();

    for (int frame = 1; frame <= 4; ++frame) {
        SimplificacoesAparencia::moverCursor(0, 0); // Desenha por cima (sem piscar a tela toda)
        exibirLogoParaTelaDeCombate(tituloCombate);
        exibirHordaDeInimigosLadoALado(listaDeInimigos, alvoAnimacao, frame, true); // true = piscar em verde
        exibirBarraDeStatusDoJogador(jogadorAtual);
        for (auto* aliado : listaDeAliados) {
            exibirBarraDeStatusDoJogador(aliado);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Desenha o ultimo quadro (Normal) sem limpar a tela
    SimplificacoesAparencia::moverCursor(0, 0);
    exibirLogoParaTelaDeCombate(tituloCombate);
    exibirHordaDeInimigosLadoALado(listaDeInimigos, alvoAnimacao, 0);
    exibirBarraDeStatusDoJogador(jogadorAtual);
    for (auto* aliado : listaDeAliados) {
        exibirBarraDeStatusDoJogador(aliado);
    }
    
    // Retorna o cursor lá pro fundo da tela para o jogo continuar imprimindo o log de combate!
    SimplificacoesAparencia::moverCursor(0, cursorYAnterior); 
}
