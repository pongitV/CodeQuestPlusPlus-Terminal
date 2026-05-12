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

    std::string gerarBarraDeXp(SistemaPersonagem* jogadorAtual, const std::string& corXp, const std::string& corReset) {
        int tamanho = 10;
        int preenchido = std::min(tamanho, (jogadorAtual->obterXpAtual() * tamanho) / std::max(1, jogadorAtual->obterXpParaSubir()));
        std::string barra = "[" + corXp + std::string(preenchido, '#') + corReset + std::string(tamanho - preenchido, '-') + "] ";
        return barra + corXp + std::to_string(jogadorAtual->obterXpAtual()) + corReset + "/" + std::to_string(jogadorAtual->obterXpParaSubir());
    }

    struct DisplayEfeito { std::string nome; Cor cor; };
    DisplayEfeito obterDisplayEfeito(EfeitoID id) {
        switch(id) {
            case EfeitoID::BuffAtributos: return {"Buff Atributos", Cor::VERDE_CLARO};
            case EfeitoID::MetadeDano: return {"Metade Dano", Cor::CIANO};
            case EfeitoID::Inviolavel: return {"Inviolavel", Cor::AZUL};
            case EfeitoID::Sangramento: return {"Sangramento", Cor::VERMELHO};
            case EfeitoID::Lentidao: return {"Lentidao", Cor::MAGENTA};
            case EfeitoID::Fraqueza: return {"Fraqueza", Cor::AMARELO};
            case EfeitoID::QuebraResistencia: return {"Quebra Def.", Cor::CIANO};
            default: return {"", Cor::RESET};
        }
    }

    std::string gerarStringDeStatus(SistemaPersonagem* jogadorAtual) {
        std::vector<EfeitoID> efeitos;
        jogadorAtual->obterIDsEfeitosAtivos(efeitos);
        std::string status = "";
        for (EfeitoID id : efeitos) {
            auto disp = obterDisplayEfeito(id);
            if (!disp.nome.empty()) {
                status += Aparencia::cor(disp.cor) + "[" + disp.nome + "]" + Aparencia::cor(Cor::RESET) + " ";
            }
        }
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

    void renderizarCenaPadrao(const std::string& titulo, const std::vector<SistemaPersonagem*>& inimigos, SistemaPersonagem* alvoAnimacao, int frame, bool isCura, bool isMorte, Item* arma, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& aliados, SistemaPersonagem* alvoDanoJogador = nullptr, Cor corDanoJogador = Cor::RESET, int danoAnimacao = -1, const std::vector<std::string>& dropsAnimacao = {}) {
        renderizarFrameBufferizado([&]() {
            (void)titulo; // Parametro nao e mais usado visualmente na interface limpa
            std::cout << Aparencia::cor(Cor::BRANCO);
            Aparencia::imprimirLinhaDivisoria('=');
            std::cout << Aparencia::cor(Cor::RESET) << "\n";
            TelaCombate::exibirHordaDeInimigosLadoALado(inimigos, alvoAnimacao, frame, isCura, false, isMorte, arma, danoAnimacao, dropsAnimacao);
            
            TelaCombate::exibirBarraDeStatusDoJogador(jogadorAtual, (alvoDanoJogador == jogadorAtual) ? corDanoJogador : Cor::RESET, (alvoDanoJogador == jogadorAtual) ? danoAnimacao : -1, (alvoDanoJogador == jogadorAtual) ? frame : 0);
            for (auto* aliado : aliados) {
                TelaCombate::exibirBarraDeStatusDoJogador(aliado, (alvoDanoJogador == aliado) ? corDanoJogador : Cor::RESET, (alvoDanoJogador == aliado) ? danoAnimacao : -1, (alvoDanoJogador == aliado) ? frame : 0);
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

void TelaCombate::exibirBarraDeStatusDoJogador(SistemaPersonagem* jogadorAtual, Cor corDestaque, int danoAnimacao, int frameAnimacao) 
{
    if (jogadorAtual == nullptr) return;
    std::string nomeDaArma = (jogadorAtual->obterArma()) ? jogadorAtual->obterArma()->obterNomeItem() + jogadorAtual->obterArma()->obterInfoStatus() : "Punhos";
    std::string nomeDoEscudo = (jogadorAtual->obterEscudo()) ? jogadorAtual->obterEscudo()->obterNomeItem() + jogadorAtual->obterEscudo()->obterInfoStatus() : "Nenhum";
    std::string nomeDaArmadura = (jogadorAtual->obterArmadura()) ? jogadorAtual->obterArmadura()->obterNomeItem() + jogadorAtual->obterArmadura()->obterInfoStatus() : "Trapos";
    
    double porcentagemDeVida = static_cast<double>(jogadorAtual->obterVida()) / jogadorAtual->obterVidaMaxima();
    std::string corVerde = Aparencia::cor(Cor::VERDE);
    std::string corLaranja = Aparencia::cor(Cor::AMARELO);
    std::string corVermelho = Aparencia::cor(Cor::VERMELHO);
    std::string corCiano = Aparencia::cor(Cor::CIANO);
    std::string corReset = Aparencia::cor(Cor::RESET);
    
    std::string corVida = (porcentagemDeVida > 0.70) ? corVerde : (porcentagemDeVida > 0.30) ? corLaranja : corVermelho;
    auto arteDoCoracao = gerarArteCoracao(porcentagemDeVida, corVerde, corLaranja, corVermelho, corReset);
    std::string arteDeBarraDeXp = gerarBarraDeXp(jogadorAtual, corCiano, corReset);
    std::string statusStr = gerarStringDeStatus(jogadorAtual);

    std::string fctVisual = "";
    std::string fctPrint = "";
    if (danoAnimacao > 0 && frameAnimacao > 0) {
        fctVisual = "  -" + std::to_string(danoAnimacao) + "!";
        std::string corFCT;
        if (frameAnimacao <= 3) corFCT = "\033[1;38;2;255;200;0m";
        else if (frameAnimacao <= 6) corFCT = "\033[1;38;2;255;100;0m";
        else if (frameAnimacao <= 9) corFCT = "\033[1;38;2;255;0;0m";
        else corFCT = "\033[1;38;2;150;0;0m";
        fctPrint = "  " + corFCT + "-" + std::to_string(danoAnimacao) + "!" + "\033[0m";
    }
    int extraPad = 10 - static_cast<int>(fctVisual.length());
    std::string paddingRight = std::string(extraPad > 0 ? extraPad : 0, ' ');
    fctPrint += paddingRight;
    std::string emptyPad(10, ' ');

    std::string playerTag = (corDestaque != Cor::RESET) ? Aparencia::cor(corDestaque) + "JOGADOR: " + jogadorAtual->obterNome() + Aparencia::cor(Cor::RESET) : "JOGADOR: " + jogadorAtual->obterNome();

    // Aplicando a cor dinâmica ao HP na linha do status
    std::vector<std::string> linhasParaImprimir = 
    {
        "| " + arteDoCoracao[0] + " |  " + playerTag + " (" + jogadorAtual->obterRaca()->obterNomeRaca() + " / " + jogadorAtual->obterNomeClasse() + ") | NIVEL: " + std::to_string(jogadorAtual->obterNivel()) + " | HP: " + corVida + std::to_string(jogadorAtual->obterVida()) + corReset + "/" + std::to_string(jogadorAtual->obterVidaMaxima()) + fctPrint,
        "| " + arteDoCoracao[1] + " |  XP: " + arteDeBarraDeXp + " | OURO: " + corLaranja + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G" + corReset + emptyPad,
        "| " + arteDoCoracao[2] + " |  EQUIP: " + nomeDaArma + " | " + nomeDoEscudo + " | " + nomeDaArmadura + emptyPad,
        "| " + arteDoCoracao[3] + " |  STATUS: " + statusStr + emptyPad
    };

    Aparencia::imprimirLinhaDivisoria();
    Aparencia::imprimirCentralizadoMultilinha(linhasParaImprimir, 95);
}

void TelaCombate::exibirHordaDeInimigosLadoALado(const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* alvoAnimacao, int frameAnimacao, bool isCura, bool animarSurgimento, bool isMorte, Item* armaAtacante, int danoAnimacao, const std::vector<std::string>& dropsAnimacao) 
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

    auto imprimirLinhaHorda = [&](const std::function<std::pair<std::string, std::string>(SistemaPersonagem*, size_t)>& gerador) {
        for (size_t i = 0; i < listaDeInimigos.size(); ++i) {
            auto [textoVisual, textoPrint] = gerador(listaDeInimigos[i], i);
            int espacosEsq = std::max(0, (larguraSeparadaParaCadaColuna - static_cast<int>(textoVisual.length())) / 2);
            std::cout << std::string(espacosEsq, ' ') << textoPrint;
            if (i < listaDeInimigos.size() - 1) {
                int espacosDir = std::max(0, larguraSeparadaParaCadaColuna - espacosEsq - static_cast<int>(textoVisual.length()));
                std::cout << std::string(espacosDir, ' ');
            }
        }
        std::cout << "\n";
    };

    imprimirLinhaHorda([](SistemaPersonagem* inimigo, size_t i) {
        std::string tag = inimigo->obterNome() + " [" + std::to_string(i) + "]";
        return std::make_pair(tag, tag);
    });

    imprimirLinhaHorda([](SistemaPersonagem* inimigo, size_t i) {
        std::string hp = "HP: " + std::to_string(inimigo->obterVida()) + "/" + std::to_string(inimigo->obterVidaMaxima());
        return std::make_pair(hp, hp);
    });

    bool hordaTemDebuffs = false;
    for (auto* ini : listaDeInimigos) {
        std::vector<EfeitoID> effs; ini->obterIDsEfeitosAtivos(effs);
        if (!effs.empty()) { hordaTemDebuffs = true; break; }
    }

    if (hordaTemDebuffs) {
        imprimirLinhaHorda([&](SistemaPersonagem* inimigo, size_t i) {
            std::vector<EfeitoID> efeitosAtivos;
            inimigo->obterIDsEfeitosAtivos(efeitosAtivos);
            std::string visualStr = "", printStr = "";
            for (size_t e = 0; e < efeitosAtivos.size(); ++e) {
                auto disp = obterDisplayEfeito(efeitosAtivos[e]);
                if (!disp.nome.empty()) {
                    visualStr += "[" + disp.nome + "]";
                    printStr += Aparencia::cor(disp.cor) + "[" + disp.nome + "]" + Aparencia::cor(Cor::RESET);
                    if (e < efeitosAtivos.size() - 1) { visualStr += " "; printStr += " "; }
                }
            }
            return std::make_pair(visualStr, printStr);
        });
    }
    
    for (int fctLine = 0; fctLine < 2; ++fctLine) {
        imprimirLinhaHorda([&](SistemaPersonagem* inimigo, size_t i) {
            std::string visualStr = "", printStr = "";
            if (inimigo == alvoAnimacao && danoAnimacao > 0 && !isMorte && frameAnimacao > 0) {
                int targetLine = (frameAnimacao < 4) ? 1 : 0;
                if (fctLine == targetLine) {
                    std::string textoDano = "-" + std::to_string(danoAnimacao) + "!";
                    std::string corFCT;
                    if (frameAnimacao <= 2) corFCT = "\033[1;38;2;255;200;0m";
                    else if (frameAnimacao <= 4) corFCT = "\033[1;38;2;255;100;0m";
                    else if (frameAnimacao <= 6) corFCT = "\033[1;38;2;255;0;0m";
                    else corFCT = "\033[1;38;2;150;0;0m";
                    visualStr = textoDano;
                    printStr = corFCT + textoDano + "\033[0m";
                }
            }
            return std::make_pair(visualStr, printStr);
        });
    }
        
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
                    
                    if (frameAnimacao >= totalLinhasArte) {
                        int totalDrops = static_cast<int>(dropsAnimacao.size()) + 1; // +1 para exibir "DERROTADO!"
                        int startDropLine = (totalLinhasArte - totalDrops) / 2;
                        if (startDropLine < 0) startDropLine = 0;
                        
                        int currentDropIndex = static_cast<int>(indiceDaLinhaDaArte) - startDropLine;
                        if (currentDropIndex >= 0 && currentDropIndex < totalDrops) {
                            std::string txt = (currentDropIndex == 0) ? "DERROTADO!" : dropsAnimacao[currentDropIndex - 1];
                            std::string corDrop = Aparencia::cor(Cor::BRANCO);
                            if (currentDropIndex == 0) corDrop = Aparencia::cor(Cor::VERMELHO);
                            else if (txt.find("XP") != std::string::npos) corDrop = Aparencia::cor(Cor::CIANO);
                            else if (txt.find("G") != std::string::npos) corDrop = Aparencia::cor(Cor::AMARELO);
                            
                            int txtLen = static_cast<int>(txt.length());
                            if (txtLen > visivelLen) txt = txt.substr(0, visivelLen);
                            txtLen = static_cast<int>(txt.length());
                            
                            int esp = (visivelLen - txtLen) / 2;
                            int rem = visivelLen - esp - txtLen;
                            linhaArte = std::string(esp > 0 ? esp : 0, ' ') + corDrop + txt + Aparencia::cor(Cor::RESET) + std::string(rem > 0 ? rem : 0, ' ');
                        }
                    }
                }
            }

            linhaAtual += espacos;
            
            if (inimigoAtual == alvoAnimacao && frameAnimacao > 0 && !isMorte) {
                std::string baseLinha;
                std::string corDestaque = isCura ? Aparencia::cor(Cor::VERDE) : Aparencia::cor(Cor::VERMELHO);
                
                bool isPiscarColorido = (frameAnimacao <= 4 && frameAnimacao % 2 == 1);
                bool isInvisivel = (frameAnimacao <= 4 && frameAnimacao % 2 == 0);

                if (!isInvisivel) {
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
                    
                    std::string linhaConstruida = isPiscarColorido ? corDestaque : "";
                    bool inWhite = false;
                    for (int i = 0; i < static_cast<int>(baseChars.size()); ++i) {
                        if (i >= startX && i < startX + static_cast<int>(impChars.size())) {
                            std::string cImpacto = impChars[i - startX];
                            if (cImpacto == " ") {
                                if (inWhite) { 
                                    linhaConstruida += isPiscarColorido ? corDestaque : Aparencia::cor(Cor::RESET); 
                                    inWhite = false; 
                                }
                                linhaConstruida += baseChars[i];
                            } else {
                                if (!inWhite) { linhaConstruida += Aparencia::cor(Cor::BRANCO); inWhite = true; }
                                linhaConstruida += cImpacto;
                            }
                        } else {
                            if (inWhite) { 
                                linhaConstruida += isPiscarColorido ? corDestaque : Aparencia::cor(Cor::RESET); 
                                inWhite = false; 
                            }
                            linhaConstruida += baseChars[i];
                        }
                    }
                    linhaConstruida += Aparencia::cor(Cor::RESET);
                    linhaAtual += linhaConstruida;
                } else {
                    if (isPiscarColorido) {
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

void TelaCombate::animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* alvoAnimacao, SistemaPersonagem* atacante, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& listaDeAliados, int danoAnimacao)
{
    Item* armaAtacante = (atacante != nullptr) ? atacante->obterArma() : nullptr;
    
    for (int frame = 1; frame <= 8; ++frame) {
        renderizarCenaPadrao(tituloCombate, listaDeInimigos, alvoAnimacao, frame, false, false, armaAtacante, jogadorAtual, listaDeAliados, nullptr, Cor::RESET, danoAnimacao);
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

void TelaCombate::animarMorteInimigo(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* inimigoMorto, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& listaDeAliados, const std::vector<std::string>& drops)
{
    if (listaDeInimigos.empty()) return;
    
    int totalLinhas = static_cast<int>(listaDeInimigos[0]->obterRaca()->obterAparenciaRaca().size());

    for (int frame = 1; frame <= totalLinhas; frame += 2) { // += 2 Para dar um efeito acelerado satisfatório
        renderizarCenaPadrao(tituloCombate, listaDeInimigos, inimigoMorto, frame, false, true, nullptr, jogadorAtual, listaDeAliados);
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
    
    // Frame final para garantir que toda a arte seja apagada e os DROPS apareçam centralizados no fantasma da imagem!
    renderizarCenaPadrao(tituloCombate, listaDeInimigos, inimigoMorto, totalLinhas, false, true, nullptr, jogadorAtual, listaDeAliados, nullptr, Cor::RESET, -1, drops);
}

void TelaCombate::animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* alvoAnimacao, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& listaDeAliados)
{
    for (int frame = 1; frame <= 4; ++frame) {
        renderizarCenaPadrao(tituloCombate, listaDeInimigos, alvoAnimacao, frame, true, false, nullptr, jogadorAtual, listaDeAliados);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    renderizarCenaPadrao(tituloCombate, listaDeInimigos, alvoAnimacao, 0, true, false, nullptr, jogadorAtual, listaDeAliados);
}

void TelaCombate::animarDanoNoJogador(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* alvoAnimacao, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& listaDeAliados, bool isParry, int danoAnimacao)
{
    Cor corDestaque = isParry ? Cor::CIANO : Cor::VERMELHO;

    for (int frame = 1; frame <= 12; ++frame) {
        Cor corAplicada = (frame % 2 == 1 && frame <= 6) ? corDestaque : Cor::RESET;
        renderizarCenaPadrao(tituloCombate, listaDeInimigos, nullptr, frame, false, false, nullptr, jogadorAtual, listaDeAliados, alvoAnimacao, corAplicada, danoAnimacao);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    renderizarCenaPadrao(tituloCombate, listaDeInimigos, nullptr, 0, false, false, nullptr, jogadorAtual, listaDeAliados);
}
