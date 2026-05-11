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
#include "../Utilidades/ControleDeInput.h"

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

    std::string gerarBarraDeXp(SistemaPersonagem* jogadorAtual, const std::string& corMagenta, const std::string& corReset) {
        int tamanho = 10;
        int preenchido = std::min(tamanho, (jogadorAtual->obterXpAtual() * tamanho) / std::max(1, jogadorAtual->obterXpParaSubir()));
        std::string barra = "[" + corMagenta + std::string(preenchido, '#') + corReset + std::string(tamanho - preenchido, '-') + "] ";
        return barra + corMagenta + std::to_string(jogadorAtual->obterXpAtual()) + corReset + "/" + std::to_string(jogadorAtual->obterXpParaSubir());
    }

    std::string gerarStringDeStatus(SistemaPersonagem* jogadorAtual, const std::string& corVerdeClaro, const std::string& corLaranja, const std::string& corVermelho, const std::string& corCiano, const std::string& corAzul, const std::string& corMagenta, const std::string& corReset) {
        std::string status = "";
        if (jogadorAtual->possuiEfeito(EfeitoID::BuffAtributos)) status += corVerdeClaro + "[Buff Atributos]" + corReset + " ";
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
        
        std::string output = buffer.str();
        
        int alturaTerminal = Aparencia::obterAlturaTerminal();
        // Reserva um espaco de seguranca para evitar scroll acidental na ultima linha
        int maxLinhas = (alturaTerminal > 2) ? alturaTerminal - 1 : 24; 
        
        std::vector<std::string> linhas;
        size_t start = 0, end = output.find('\n');
        while (end != std::string::npos) {
            linhas.push_back(output.substr(start, end - start));
            start = end + 1;
            end = output.find('\n', start);
        }
        if (start < output.length()) linhas.push_back(output.substr(start));

        // Para evitar que a Logo seja cortada (causando sobreposição grotesca) quando há muitas mensagens de drops/ataques,
        // nós truncamos as mensagens antigas a partir da divisória do HUD, preservando a arte e as mensagens mais recentes.
        if (static_cast<int>(linhas.size()) > maxLinhas) {
            int linhasParaRemover = static_cast<int>(linhas.size()) - maxLinhas;
            int indiceDivisoria = -1;
            for (int i = static_cast<int>(linhas.size()) - 1; i >= 0; --i) {
                if (linhas[i].find("=====") != std::string::npos) {
                    indiceDivisoria = i;
                    break;
                }
            }

            if (indiceDivisoria != -1 && indiceDivisoria + 1 < static_cast<int>(linhas.size())) {
                int inicioRemocao = indiceDivisoria + 1;
                int disponivelParaRemover = static_cast<int>(linhas.size()) - inicioRemocao;
                if (linhasParaRemover <= disponivelParaRemover) {
                    linhas.erase(linhas.begin() + inicioRemocao, linhas.begin() + inicioRemocao + linhasParaRemover);
                    linhasParaRemover = 0;
                } else {
                    linhas.erase(linhas.begin() + inicioRemocao, linhas.end());
                    linhasParaRemover -= disponivelParaRemover;
                }
            } else {
                // Fallback seguro caso não ache a divisória
                linhas.erase(linhas.begin(), linhas.begin() + linhasParaRemover);
                linhasParaRemover = 0;
            }
            
            // Se ainda precisar remover (ex: arte gigantesca e as msg nao bastarem), removemos do topo da Logo
            if (linhasParaRemover > 0) {
                linhas.erase(linhas.begin(), linhas.begin() + linhasParaRemover);
            }
        }

        std::string finalOutput = "";
        for (size_t i = 0; i < linhas.size(); ++i) {
            finalOutput += linhas[i] + "\033[K";
            if (i < linhas.size() - 1) finalOutput += "\n";
        }
        
        std::cout << "\033[H" << finalOutput << "\033[J" << std::flush;
    }

    void renderizarCenaPadrao(const std::string& titulo, const std::vector<SistemaPersonagem*>& inimigos, SistemaPersonagem* alvoAnimacao, int frame, bool isCura, bool isMorte, Item* arma, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& aliados, SistemaPersonagem* alvoDanoJogador = nullptr, Cor corDanoJogador = Cor::RESET) {
        renderizarFrameBufferizado([&]() {
            (void)titulo; // Parametro nao e mais usado visualmente na interface limpa
            std::cout << Aparencia::cor(Cor::BRANCO);
            Aparencia::imprimirLinhaDivisoria('=');
            std::cout << Aparencia::cor(Cor::RESET) << "\n";
            TelaCombate::exibirHordaDeInimigosLadoALado(inimigos, alvoAnimacao, frame, isCura, false, isMorte, arma);
            
            TelaCombate::exibirBarraDeStatusDoJogador(jogadorAtual, (alvoDanoJogador == jogadorAtual) ? corDanoJogador : Cor::RESET);
            for (auto* aliado : aliados) {
                TelaCombate::exibirBarraDeStatusDoJogador(aliado, (alvoDanoJogador == aliado) ? corDanoJogador : Cor::RESET);
            }
            
            Aparencia::imprimirLinhaDivisoria();
            for (const auto& msg : mensagensFixasCombate) std::cout << msg;
        });
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
    std::string corVerdeClaro = Aparencia::cor(Cor::VERDE_CLARO);
    std::string corReset = Aparencia::cor(Cor::RESET);
    
    std::string corVida = (porcentagemDeVida > 0.70) ? corVerde : (porcentagemDeVida > 0.30) ? corLaranja : corVermelho;
    auto arteDoCoracao = gerarArteCoracao(porcentagemDeVida, corVerde, corLaranja, corVermelho, corReset);
    std::string arteDeBarraDeXp = gerarBarraDeXp(jogadorAtual, corMagenta, corReset);
    std::string statusStr = gerarStringDeStatus(jogadorAtual, corVerdeClaro, corLaranja, corVermelho, corCiano, corAzul, corMagenta, corReset);

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

void TelaCombate::exibirHordaDeInimigosLadoALado(const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* alvoAnimacao, int frameAnimacao, bool isCura, bool animarSurgimento, bool isMorte, Item* armaAtacante) 
{
    if (listaDeInimigos.empty()) return;
    int larguraTerminal = Aparencia::obterLarguraTerminal();
    const std::vector<std::string>& arteDoInimigo = listaDeInimigos[0]->obterRaca()->obterAparenciaRaca();
    int quantidadeTotalDeInimigosNaHorda = static_cast<int>(listaDeInimigos.size());
    int larguraSeparadaParaCadaColuna = larguraTerminal / quantidadeTotalDeInimigosNaHorda; 

    auto splitUTF8 = [](const std::string& s) {
        std::vector<std::string> chars;
        for (size_t i = 0; i < s.length(); ) {
            int len = 1;
            unsigned char c = static_cast<unsigned char>(s[i]);
            if ((c & 0x80) == 0) len = 1;
            else if ((c & 0xE0) == 0xC0) len = 2;
            else if ((c & 0xF0) == 0xE0) len = 3;
            else if ((c & 0xF8) == 0xF0) len = 4;
            chars.push_back(s.substr(i, len));
            i += len;
        }
        return chars;
    };

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
            int visivelLen = static_cast<int>(splitUTF8(arteDoInimigo[indiceDaLinhaDaArte]).size());
            int espacosParaCentralizarAArte = (larguraSeparadaParaCadaColuna - visivelLen) / 2;
            std::string espacos(espacosParaCentralizarAArte > 0 ? espacosParaCentralizarAArte : 0, ' ');
            
            SistemaPersonagem* inimigoAtual = listaDeInimigos[indiceDoInimigoParaDesenhar];
            std::string linhaArte = arteDoInimigo[indiceDaLinhaDaArte];
            
            if (isMorte && inimigoAtual == alvoAnimacao) {
                int totalLinhasArte = static_cast<int>(arteDoInimigo.size());
                if (static_cast<int>(indiceDaLinhaDaArte) >= totalLinhasArte - frameAnimacao) {
                    linhaArte = std::string(visivelLen, ' ');
                }
            }

            linhaAtual += espacos;
            
            if (inimigoAtual == alvoAnimacao && frameAnimacao > 0 && !isMorte) {
                std::string baseLinha;
                std::string corDestaque = isCura ? Aparencia::cor(Cor::VERDE) : Aparencia::cor(Cor::VERMELHO);
                
                if (frameAnimacao % 2 == 1) {
                    baseLinha = linhaArte;
                } else {
                    baseLinha = std::string(visivelLen, ' ');
                }
                
                bool temImpactoNaLinha = false;
                std::vector<std::string> arteImpacto;
                int linhaImpacto = 0;
                
                if (!isCura) {
                    arteImpacto = {

                        "░█░█░▀█▀░▀█▀░",
                        "░█▀█░░█░░░█░░",
                        "░▀░▀░▀▀▀░░▀░░"
                    };
                    int centroY = static_cast<int>(arteDoInimigo.size()) / 2;
                    int inicioY = centroY - 1;
                    int fimY = centroY + 1;
                    
                    if (static_cast<int>(indiceDaLinhaDaArte) >= inicioY && static_cast<int>(indiceDaLinhaDaArte) <= fimY) {
                        linhaImpacto = static_cast<int>(indiceDaLinhaDaArte) - inicioY;
                        temImpactoNaLinha = true;
                    }
                }
                
                if (temImpactoNaLinha) {
                    std::vector<std::string> baseChars = splitUTF8(baseLinha);
                    
                    if (linhaImpacto < 0) linhaImpacto = 0;
                    if (linhaImpacto >= static_cast<int>(arteImpacto.size())) linhaImpacto = static_cast<int>(arteImpacto.size()) - 1;
                    std::vector<std::string> impChars = splitUTF8(arteImpacto[linhaImpacto]);

                    int centroX = static_cast<int>(baseChars.size()) / 2;
                    int startX = centroX - static_cast<int>(impChars.size()) / 2;
                    if (startX < 0) startX = 0;
                    
                    std::string linhaConstruida = (frameAnimacao % 2 == 1) ? corDestaque : "";
                    bool inWhite = false;
                    for (int i = 0; i < static_cast<int>(baseChars.size()); ++i) {
                        if (i >= startX && i < startX + static_cast<int>(impChars.size())) {
                            std::string cImpacto = impChars[i - startX];
                            if (cImpacto == " ") {
                                if (inWhite) { 
                                    linhaConstruida += (frameAnimacao % 2 == 1) ? corDestaque : Aparencia::cor(Cor::RESET); 
                                    inWhite = false; 
                                }
                                linhaConstruida += baseChars[i];
                            } else {
                                if (!inWhite) { linhaConstruida += Aparencia::cor(Cor::BRANCO); inWhite = true; }
                                linhaConstruida += cImpacto;
                            }
                        } else {
                            if (inWhite) { 
                                linhaConstruida += (frameAnimacao % 2 == 1) ? corDestaque : Aparencia::cor(Cor::RESET); 
                                inWhite = false; 
                            }
                            linhaConstruida += baseChars[i];
                        }
                    }
                    linhaConstruida += Aparencia::cor(Cor::RESET);
                    linhaAtual += linhaConstruida;
                } else {
                    if (frameAnimacao % 2 == 1) {
                        linhaAtual += corDestaque + baseLinha + Aparencia::cor(Cor::RESET);
                    } else {
                        linhaAtual += baseLinha;
                    }
                }
            } else {
                linhaAtual += linhaArte;
            }
            
            int espacosDireita = larguraSeparadaParaCadaColuna - espacosParaCentralizarAArte - visivelLen;
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

void TelaCombate::animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* alvoAnimacao, SistemaPersonagem* atacante, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& listaDeAliados)
{
    Item* armaAtacante = (atacante != nullptr) ? atacante->obterArma() : nullptr;
    
    for (int frame = 1; frame <= 8; ++frame) {
        renderizarCenaPadrao(tituloCombate, listaDeInimigos, alvoAnimacao, frame, false, false, armaAtacante, jogadorAtual, listaDeAliados);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    renderizarCenaPadrao(tituloCombate, listaDeInimigos, alvoAnimacao, 0, false, false, nullptr, jogadorAtual, listaDeAliados);
}

void TelaCombate::exibirCabecalhoDoTurno(int turnoAtual, const std::string& nomePersonagem) {
    std::string textoDoTurno = "--- TURNO " + std::to_string(turnoAtual) + " | VEZ DE " + nomePersonagem + " ---";
    std::string msgTurno = "\n" + Aparencia::espacosParaCentralizar(textoDoTurno.length()) + textoDoTurno + "\n";
    std::cout << msgTurno;
    adicionarMensagemFixa(msgTurno);
}

int TelaCombate::obterAcaoDoJogador() {
    std::string textoAcoes = "1. Atacar | 2. Defender | 3. Habilidade | 4. Inventario | 5. Jogador | 6. Bestiario | 7. Log | Escolha: ";
    return ControleDeInput::lerInteiroComLimites(textoAcoes, 1, 7, true);
}

int TelaCombate::obterAlvoAtaque(int maxIndice) {
    return ControleDeInput::lerInteiroComLimites("Escolha o alvo (0 a " + std::to_string(maxIndice) + "): ", 0, maxIndice, false, Aparencia::margemCombate());
}

int TelaCombate::obterAlvoItem(const std::vector<SistemaPersonagem*>& listaDeInimigos) {
    std::cout << "\n" << Aparencia::margemCombate() << "--- ESCOLHA UM ALVO ---\n";
    for (size_t i = 0; i < listaDeInimigos.size(); ++i) {
        std::cout << Aparencia::margemCombate() << "[" << i << "] " << listaDeInimigos[i]->obterNome() << " (HP: " << listaDeInimigos[i]->obterVida() << ")\n";
    }
    int maxIndice = static_cast<int>(listaDeInimigos.size()) - 1;
    return ControleDeInput::lerInteiroComLimites("Escolha (ou -1 para CANCELAR): ", -1, maxIndice, false, Aparencia::margemCombate());
}

int TelaCombate::obterEscolhaDeEscudo(const std::string& nomePersonagem, const std::vector<Item*>& listaDeEscudos) {
    std::cout << "\n" << Aparencia::margemCombate() << "=== SELECIONE UM ESCUDO PARA " << nomePersonagem << " ===\n";
    for (size_t indice = 0; indice < listaDeEscudos.size(); indice++) {
        std::cout << Aparencia::margemCombate() << " [" << indice + 1 << "] " << listaDeEscudos[indice]->obterNomeItem() << listaDeEscudos[indice]->obterInfoStatus() << "\n";
    }
    std::cout << Aparencia::margemCombate() << " [0] Cancelar\n\n";
    return ControleDeInput::lerInteiroComLimites("Escolha: ", 0, static_cast<int>(listaDeEscudos.size()), false, Aparencia::margemCombate());
}

void TelaCombate::notificarInimigosMaisAgeis() {
    std::string msg = "[SISTEMA]: Os inimigos sao mais ageis e atacam primeiro!";
    std::cout << "\n" << Aparencia::margemCombate() << Aparencia::cor(Cor::VERMELHO) << msg << Aparencia::cor(Cor::RESET) << "\n";
    Aparencia::registrarLogBatalha(msg);
    Aparencia::aguardarEnter();
}

void TelaCombate::notificarTurnoExtra(int destrezaJogador, int maxDestrezaInimigos) {
    std::string msg = "[SISTEMA]: Sua agilidade extrema (" + std::to_string(destrezaJogador) + " VS " + std::to_string(maxDestrezaInimigos) + ") permite que voce aja novamente!";
    std::cout << "\n" << Aparencia::margemCombate() << Aparencia::cor(Cor::CIANO) << msg << Aparencia::cor(Cor::RESET) << "\n";
    Aparencia::registrarLogBatalha(msg);
    Aparencia::aguardarEnter();
}

void TelaCombate::notificarDesprevencaoInventario() {
    std::string msg = "[SISTEMA]: O inimigo te pegou desprevinido enquanto voce usava o inventario!";
    std::cout << "\n" << Aparencia::margemCombate() << Aparencia::cor(Cor::AMARELO) << msg << Aparencia::cor(Cor::RESET) << "\n";
    Aparencia::registrarLogBatalha(msg);
}

void TelaCombate::notificarSemEscudos(const std::string& nomePersonagem) {
    std::cout << "\n" << Aparencia::margemCombate() << "[!] " << nomePersonagem << " nao possui escudos no inventario para usar!\n";
}

void TelaCombate::notificarDesequilibrioDefesa(const std::string& nomePersonagem) {
    std::string msg = "[ERRO]: " + nomePersonagem + " se desequilibrou e precisa de 1 turno para poder defender novamente!";
    std::cout << "\n" << Aparencia::margemCombate() << msg << "\n";
    Aparencia::registrarLogBatalha(msg);
    Aparencia::aguardarEnter();
}

void TelaCombate::notificarPosturaDefensiva(const std::string& nomePersonagem, const std::string& nomeEscudo) {
    std::string msg = "[SISTEMA]: " + nomePersonagem + " assumiu uma postura defensiva com " + nomeEscudo + "!";
    std::cout << "\n" << Aparencia::margemCombate() << msg << "\n";
    Aparencia::registrarLogBatalha(msg);
    Aparencia::aguardarEnter();
}

void TelaCombate::notificarAcaoInvalida() {
    std::cout << "\n" << Aparencia::margemCombate() << "[ERRO] Acao invalida!\n";
    Aparencia::aguardarEnter();
}

void TelaCombate::notificarCancelamentoItem() {
    std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA] Uso do frasco cancelado. O item voltou para a mochila.\n";
}

void TelaCombate::notificarRequisitoNaoAtendido(const std::string& mensagemRequisito) {
    if (mensagemRequisito.substr(0, 1) == "\n") {
        std::cout << "\n" << Aparencia::margemCombate() << mensagemRequisito.substr(1);
    } else {
        std::cout << Aparencia::margemCombate() << mensagemRequisito;
    }
    Aparencia::aguardarEnter();
}

void TelaCombate::atualizarTelaEstatica(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& listaDeAliados)
{
    renderizarCenaPadrao(tituloCombate, listaDeInimigos, nullptr, 0, false, false, nullptr, jogadorAtual, listaDeAliados);
}

void TelaCombate::animarMorteInimigo(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* inimigoMorto, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& listaDeAliados)
{
    if (listaDeInimigos.empty()) return;
    
    int totalLinhas = static_cast<int>(listaDeInimigos[0]->obterRaca()->obterAparenciaRaca().size());

    for (int frame = 1; frame <= totalLinhas; frame += 2) { // += 2 Para dar um efeito acelerado satisfatório
        renderizarCenaPadrao(tituloCombate, listaDeInimigos, inimigoMorto, frame, false, true, nullptr, jogadorAtual, listaDeAliados);
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
    
    // Frame final para garantir que toda a arte seja apagada (corrige a falha quando totalLinhas for par)
    renderizarCenaPadrao(tituloCombate, listaDeInimigos, inimigoMorto, totalLinhas, false, true, nullptr, jogadorAtual, listaDeAliados);
}

void TelaCombate::animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* alvoAnimacao, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& listaDeAliados)
{
    for (int frame = 1; frame <= 4; ++frame) {
        renderizarCenaPadrao(tituloCombate, listaDeInimigos, alvoAnimacao, frame, true, false, nullptr, jogadorAtual, listaDeAliados);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    renderizarCenaPadrao(tituloCombate, listaDeInimigos, alvoAnimacao, 0, true, false, nullptr, jogadorAtual, listaDeAliados);
}

void TelaCombate::animarDanoNoJogador(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* alvoAnimacao, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& listaDeAliados, bool isParry)
{
    Cor corDestaque = isParry ? Cor::CIANO : Cor::VERMELHO;

    for (int frame = 1; frame <= 4; ++frame) {
        Cor corAplicada = (frame % 2 == 1) ? corDestaque : Cor::RESET;
        renderizarCenaPadrao(tituloCombate, listaDeInimigos, nullptr, 0, false, false, nullptr, jogadorAtual, listaDeAliados, alvoAnimacao, corAplicada);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    renderizarCenaPadrao(tituloCombate, listaDeInimigos, nullptr, 0, false, false, nullptr, jogadorAtual, listaDeAliados);
}
