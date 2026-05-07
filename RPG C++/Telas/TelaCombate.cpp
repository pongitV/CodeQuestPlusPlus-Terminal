#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <thread>
#include <chrono>
#include <sstream>
#include <functional>

#include "TelaCombate.h"
#include "../Sistemas/SistemaPersonagem.h"
#include "../Utilidades/Aparencia.h"
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

    std::vector<std::string> mensagensFixasCombate;

    void renderizarFrameBufferizado(const std::function<void()>& renderFunc) {
        std::ostringstream buffer;
        std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());
        renderFunc();
        std::cout.rdbuf(oldCout);
        std::cout << "\033[2J\033[3J\033[H" << buffer.str() << std::flush;
    }
}

void TelaCombate::adicionarMensagemFixa(const std::string& msg) {
    mensagensFixasCombate.push_back(msg);
}

void TelaCombate::limparMensagensFixas() {
    mensagensFixasCombate.clear();
}

void TelaCombate::exibirLogoParaTelaDeCombate(const std::string& tituloDaTela) 
{
    int larguraConsole = Aparencia::obterLarguraTerminal();
    
    std::vector<std::string> logo = 
    {
        "   █████████     ███████    ██████   ██████ ███████████    █████████   ███████████ ██████████",
        "  ███░░░░░███  ███░░░░░███ ░░██████ ██████ ░░███░░░░░███  ███░░░░░███ ░█░░░███░░░█░░███░░░░░█ ",
        " ███     ░░░  ███     ░░███ ░███░█████░███  ░███    ░███ ░███    ░███ ░   ░███  ░  ░███  █ ░  ",
        "░███         ░███      ░███ ░███░░███ ░███  ░██████████  ░███████████     ░███     ░██████    ",
        "░███         ░███      ░███ ░███ ░░░  ░███  ░███░░░░░███ ░███░░░░░███     ░███     ░███░░█    ",
        "░░███     ███░░███     ███  ░███      ░███  ░███    ░███ ░███    ░███     ░███     ░███ ░   █ ",
        " ░░█████████  ░░░███████░   █████     █████ ███████████  █████   █████    █████    ██████████ ",
        "  ░░░░░░░░░     ░░░░░░░    ░░░░░     ░░░░░ ░░░░░░░░░░░  ░░░░░   ░░░░░    ░░░░░    ░░░░░░░░░░  "
    };

    Aparencia::exibirLogoAscii(logo, 95, Cor::VERMELHO, tituloDaTela);
}

void TelaCombate::exibirBarraDeStatusDoJogador(SistemaPersonagem* jogadorAtual, Cor corDestaque) 
{
    if (jogadorAtual == nullptr) return;
    std::string nomeDaArma = (jogadorAtual->obterArma()) ? jogadorAtual->obterArma()->obterNomeItem() + jogadorAtual->obterArma()->obterInfoStatus() : "Punhos";
    std::string nomeDoEscudo = (jogadorAtual->obterEscudo()) ? jogadorAtual->obterEscudo()->obterNomeItem() + jogadorAtual->obterEscudo()->obterInfoStatus() : "Nenhum";
    std::string nomeDaArmadura = (jogadorAtual->obterArmadura()) ? jogadorAtual->obterArmadura()->obterNomeItem() + jogadorAtual->obterArmadura()->obterInfoStatus() : "Trapos";
    
    double porcentagemDeVida = static_cast<double>(jogadorAtual->obterVida()) / jogadorAtual->obterVidaMaxima();
    std::string corVerde = Aparencia::cor(Cor::VERDE);
    std::string corLaranja = Aparencia::cor(Cor::AMARELO);
    std::string corVermelho = Aparencia::cor(Cor::VERMELHO);
    std::string corAzul = Aparencia::cor(Cor::AZUL);
    std::string corCiano = Aparencia::cor(Cor::CIANO);
    std::string corMagenta = Aparencia::cor(Cor::MAGENTA);
    std::string corReset = Aparencia::cor(Cor::RESET);
    
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

    Aparencia::imprimirLinhaDivisoria();
    Aparencia::imprimirCentralizadoMultilinha(linhasParaImprimir, 95);
}

void TelaCombate::exibirHordaDeInimigosLadoALado(const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* alvoAnimacao, int frameAnimacao, bool isCura, bool animarSurgimento, bool isMorte) 
{
    if (listaDeInimigos.empty()) return;
    int larguraTerminal = Aparencia::obterLarguraTerminal();
    const std::vector<std::string>& arteDoInimigo = listaDeInimigos[0]->obterRaca()->obterAparenciaRaca();
    int quantidadeTotalDeInimigosNaHorda = static_cast<int>(listaDeInimigos.size());
    int larguraSeparadaParaCadaColuna = larguraTerminal / quantidadeTotalDeInimigosNaHorda; 

    for (size_t indiceInimigo = 0; indiceInimigo < listaDeInimigos.size(); indiceInimigo++) 
    {
        std::string tagIdentificadoraDoInimigo = listaDeInimigos[indiceInimigo]->obterNome() + " [" + std::to_string(indiceInimigo) + "]";
        int espacosParaCentralizarOId = (larguraSeparadaParaCadaColuna - (int)tagIdentificadoraDoInimigo.length()) / 2;
        std::cout << std::string(espacosParaCentralizarOId > 0 ? espacosParaCentralizarOId : 0, ' ') << tagIdentificadoraDoInimigo;
        if (indiceInimigo < listaDeInimigos.size() - 1) {
            int espacosDir = larguraSeparadaParaCadaColuna - espacosParaCentralizarOId - tagIdentificadoraDoInimigo.length();
            std::cout << std::string(espacosDir > 0 ? espacosDir : 0, ' ');
        }
    }
    std::cout << "\n";
    for (size_t indiceInimigo = 0; indiceInimigo < listaDeInimigos.size(); indiceInimigo++) 
    {
        std::string valorDePontosDeVidaDoInimigo = "HP: " + std::to_string(listaDeInimigos[indiceInimigo]->obterVida()) + "/" + std::to_string(listaDeInimigos[indiceInimigo]->obterVidaMaxima());
        int espacosParaCentralizarOHp = (larguraSeparadaParaCadaColuna - (int)valorDePontosDeVidaDoInimigo.length()) / 2;
        std::cout << std::string(espacosParaCentralizarOHp > 0 ? espacosParaCentralizarOHp : 0, ' ') << valorDePontosDeVidaDoInimigo;
        if (indiceInimigo < listaDeInimigos.size() - 1) {
            int espacosDir = larguraSeparadaParaCadaColuna - espacosParaCentralizarOHp - valorDePontosDeVidaDoInimigo.length();
            std::cout << std::string(espacosDir > 0 ? espacosDir : 0, ' ');
        }
    }
        std::cout << "\n";
        
        bool hordaTemDebuffs = false;
        std::vector<std::vector<std::pair<std::string, std::string>>> todosDebuffs(listaDeInimigos.size());
        for (size_t indiceInimigo = 0; indiceInimigo < listaDeInimigos.size(); indiceInimigo++) {
            std::vector<EfeitoID> efeitosAtivos;
            listaDeInimigos[indiceInimigo]->obterIDsEfeitosAtivos(efeitosAtivos);
            for (auto& id : efeitosAtivos) {
                if (id == EfeitoID::Sangramento) todosDebuffs[indiceInimigo].push_back({"[Sangramento]", Aparencia::cor(Cor::VERMELHO) + "[Sangramento]" + Aparencia::cor(Cor::RESET)});
                else if (id == EfeitoID::Lentidao) todosDebuffs[indiceInimigo].push_back({"[Lentidao]", Aparencia::cor(Cor::MAGENTA) + "[Lentidao]" + Aparencia::cor(Cor::RESET)});
                else if (id == EfeitoID::Fraqueza) todosDebuffs[indiceInimigo].push_back({"[Fraqueza]", Aparencia::cor(Cor::AMARELO) + "[Fraqueza]" + Aparencia::cor(Cor::RESET)});
                else if (id == EfeitoID::QuebraResistencia) todosDebuffs[indiceInimigo].push_back({"[Quebra Def.]", Aparencia::cor(Cor::CIANO) + "[Quebra Def.]" + Aparencia::cor(Cor::RESET)});
            }
            if (!todosDebuffs[indiceInimigo].empty()) hordaTemDebuffs = true;
        }
        if (hordaTemDebuffs) {
            for (size_t indiceInimigo = 0; indiceInimigo < listaDeInimigos.size(); indiceInimigo++) {
                std::string visualStr = "", printStr = "";
                for (size_t i = 0; i < todosDebuffs[indiceInimigo].size(); ++i) {
                    visualStr += todosDebuffs[indiceInimigo][i].first;
                    printStr += todosDebuffs[indiceInimigo][i].second;
                    if (i < todosDebuffs[indiceInimigo].size() - 1) { visualStr += " "; printStr += " "; }
                }
                int espacosEsquerda = (larguraSeparadaParaCadaColuna - (int)visualStr.length()) / 2;
                if (espacosEsquerda < 0) espacosEsquerda = 0;
                std::cout << std::string(espacosEsquerda, ' ') << printStr;
                if (indiceInimigo < listaDeInimigos.size() - 1) {
                    int espacosDireita = larguraSeparadaParaCadaColuna - espacosEsquerda - (int)visualStr.length();
                    if (espacosDireita < 0) espacosDireita = 0;
                    std::cout << std::string(espacosDireita, ' ');
                }
            }
            std::cout << "\n";
        }
    std::cout << "\n";
        
    std::vector<std::string> linhasDaArte;
    for (size_t indiceDaLinhaDaArte = 0; indiceDaLinhaDaArte < arteDoInimigo.size(); indiceDaLinhaDaArte++) 
    {
        std::string linhaAtual = "";
        for (size_t indiceDoInimigoParaDesenhar = 0; indiceDoInimigoParaDesenhar < listaDeInimigos.size(); indiceDoInimigoParaDesenhar++) 
        {
            int espacosParaCentralizarAArte = (larguraSeparadaParaCadaColuna - (int)arteDoInimigo[indiceDaLinhaDaArte].length()) / 2;
            std::string espacos(espacosParaCentralizarAArte > 0 ? espacosParaCentralizarAArte : 0, ' ');
            
            SistemaPersonagem* inimigoAtual = listaDeInimigos[indiceDoInimigoParaDesenhar];
            std::string linhaArte = arteDoInimigo[indiceDaLinhaDaArte];
            
            if (isMorte && inimigoAtual == alvoAnimacao) {
                int totalLinhasArte = static_cast<int>(arteDoInimigo.size());
                if (static_cast<int>(indiceDaLinhaDaArte) >= totalLinhasArte - frameAnimacao) {
                    linhaArte = std::string(linhaArte.length(), ' ');
                }
            }

            linhaAtual += espacos;
            
            if (inimigoAtual == alvoAnimacao && frameAnimacao > 0 && !isMorte) {
                if (frameAnimacao % 2 == 1) {
                    Cor corDestaque = isCura ? Cor::VERDE : Cor::VERMELHO;
                    linhaAtual += Aparencia::cor(corDestaque) + linhaArte + Aparencia::cor(Cor::RESET);
                } else {
                    linhaAtual += std::string(linhaArte.length(), ' ');
                }
            } else {
                linhaAtual += linhaArte;
            }
            
            int espacosDireita = larguraSeparadaParaCadaColuna - espacosParaCentralizarAArte - (int)linhaArte.length();
            if (indiceDoInimigoParaDesenhar < listaDeInimigos.size() - 1) {
                linhaAtual += std::string(espacosDireita > 0 ? espacosDireita : 0, ' ');
            }
        }
        linhasDaArte.push_back(linhaAtual);
    }
    
    if (animarSurgimento) {
        Aparencia::imprimirVetorAnimado(linhasDaArte, 12);
    } else {
        for (const auto& linha : linhasDaArte) std::cout << linha << "\n";
    }
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
    for (int frame = 1; frame <= 4; ++frame) {
        renderizarFrameBufferizado([&]() {
            exibirLogoParaTelaDeCombate(tituloCombate);
            exibirHordaDeInimigosLadoALado(listaDeInimigos, alvoAnimacao, frame, false);
            exibirBarraDeStatusDoJogador(jogadorAtual);
            for (auto* aliado : listaDeAliados) exibirBarraDeStatusDoJogador(aliado);
            Aparencia::imprimirLinhaDivisoria();
            for (const auto& msg : mensagensFixasCombate) std::cout << msg;
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    renderizarFrameBufferizado([&]() {
        exibirLogoParaTelaDeCombate(tituloCombate);
        exibirHordaDeInimigosLadoALado(listaDeInimigos, alvoAnimacao, 0, false);
        exibirBarraDeStatusDoJogador(jogadorAtual);
        for (auto* aliado : listaDeAliados) exibirBarraDeStatusDoJogador(aliado);
        Aparencia::imprimirLinhaDivisoria();
        for (const auto& msg : mensagensFixasCombate) std::cout << msg;
    });
}

void TelaCombate::animarMorteInimigo(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* inimigoMorto, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& listaDeAliados)
{
    if (listaDeInimigos.empty()) return;
    
    int totalLinhas = static_cast<int>(listaDeInimigos[0]->obterRaca()->obterAparenciaRaca().size());

    for (int frame = 1; frame <= totalLinhas; frame += 2) { // += 2 Para dar um efeito acelerado satisfatório
        renderizarFrameBufferizado([&]() {
            exibirLogoParaTelaDeCombate(tituloCombate);
            exibirHordaDeInimigosLadoALado(listaDeInimigos, inimigoMorto, frame, false, false, true);
            exibirBarraDeStatusDoJogador(jogadorAtual);
            for (auto* aliado : listaDeAliados) exibirBarraDeStatusDoJogador(aliado);
            Aparencia::imprimirLinhaDivisoria();
            for (const auto& msg : mensagensFixasCombate) std::cout << msg;
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
}

void TelaCombate::animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* alvoAnimacao, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& listaDeAliados)
{
    for (int frame = 1; frame <= 4; ++frame) {
        renderizarFrameBufferizado([&]() {
            exibirLogoParaTelaDeCombate(tituloCombate);
            exibirHordaDeInimigosLadoALado(listaDeInimigos, alvoAnimacao, frame, true);
            exibirBarraDeStatusDoJogador(jogadorAtual);
            for (auto* aliado : listaDeAliados) exibirBarraDeStatusDoJogador(aliado);
            Aparencia::imprimirLinhaDivisoria();
            for (const auto& msg : mensagensFixasCombate) std::cout << msg;
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    renderizarFrameBufferizado([&]() {
        exibirLogoParaTelaDeCombate(tituloCombate);
        exibirHordaDeInimigosLadoALado(listaDeInimigos, alvoAnimacao, 0, true);
        exibirBarraDeStatusDoJogador(jogadorAtual);
        for (auto* aliado : listaDeAliados) exibirBarraDeStatusDoJogador(aliado);
        Aparencia::imprimirLinhaDivisoria();
        for (const auto& msg : mensagensFixasCombate) std::cout << msg;
    });
}

void TelaCombate::animarDanoNoJogador(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* alvoAnimacao, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& listaDeAliados, bool isParry)
{
    Cor corDestaque = isParry ? Cor::CIANO : Cor::VERMELHO;

    for (int frame = 1; frame <= 4; ++frame) {
        renderizarFrameBufferizado([&]() {
            exibirLogoParaTelaDeCombate(tituloCombate);
            exibirHordaDeInimigosLadoALado(listaDeInimigos, nullptr, 0, false);
            
            Cor corAplicada = (frame % 2 == 1) ? corDestaque : Cor::RESET;

            if (jogadorAtual == alvoAnimacao) exibirBarraDeStatusDoJogador(jogadorAtual, corAplicada);
            else exibirBarraDeStatusDoJogador(jogadorAtual, Cor::RESET);

            for (auto* aliado : listaDeAliados) {
                if (aliado == alvoAnimacao) exibirBarraDeStatusDoJogador(aliado, corAplicada);
                else exibirBarraDeStatusDoJogador(aliado, Cor::RESET);
            }
            Aparencia::imprimirLinhaDivisoria();
            for (const auto& msg : mensagensFixasCombate) std::cout << msg;
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    renderizarFrameBufferizado([&]() {
        exibirLogoParaTelaDeCombate(tituloCombate);
        exibirHordaDeInimigosLadoALado(listaDeInimigos, nullptr, 0, false);
        exibirBarraDeStatusDoJogador(jogadorAtual, Cor::RESET);
        for (auto* aliado : listaDeAliados) exibirBarraDeStatusDoJogador(aliado, Cor::RESET);
        Aparencia::imprimirLinhaDivisoria();
        for (const auto& msg : mensagensFixasCombate) std::cout << msg;
    });
}
