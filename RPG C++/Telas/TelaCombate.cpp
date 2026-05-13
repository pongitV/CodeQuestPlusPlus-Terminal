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
    std::string gerarBarraDeXp(SistemaPersonagem* jogadorAtual, const std::string& corXp, const std::string& corReset) {
        int tamanho = 8;
        double porcentagem = static_cast<double>(jogadorAtual->obterXpAtual()) / std::max(1, jogadorAtual->obterXpParaSubir());
        std::string barra = Aparencia::gerarBarraSuave(porcentagem, tamanho, corXp, Aparencia::cor(Cor::CINZA));
        return "[" + barra + corReset + "] " + corXp + std::to_string(jogadorAtual->obterXpAtual()) + corReset + "/" + std::to_string(jogadorAtual->obterXpParaSubir());
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
        
        std::vector<std::string> linhas;
        size_t start = 0, end = output.find('\n');
        while (end != std::string::npos) {
            linhas.push_back(output.substr(start, end - start));
            start = end + 1;
            end = output.find('\n', start);
        }
        if (start < output.length()) linhas.push_back(output.substr(start));

        int alturaTerminal = Aparencia::obterAlturaTerminal();
        int maxLinhas = (alturaTerminal > 2) ? alturaTerminal - 1 : 24; 
        
        if (static_cast<int>(linhas.size()) > maxLinhas) {
            int linhasParaRemover = static_cast<int>(linhas.size()) - maxLinhas;
            
            // Encontra a linha divisoria do HUD ("=====")
            int indiceDivisoria = -1;
            for (int i = static_cast<int>(linhas.size()) - 1; i >= 0; --i) {
                if (linhas[i].find("=====") != std::string::npos) {
                    indiceDivisoria = i;
                    break;
                }
            }

            if (indiceDivisoria != -1) {
                // Calcula quantas linhas as mensagens e espacamentos ocupam antes da divisoria
                int nL = 1; // Para o \n vazio que exibirHordaDeInimigosLadoALado sempre deixa no final
                for (const auto& msg : mensagensFixasCombate) {
                    for (char c : msg) {
                        if (c == '\n') nL++;
                    }
                }
                if (!mensagensFixasCombate.empty()) nL++; // Do if (!mensagensFixasCombate.empty()) std::cout << "\n";
                
                int indiceFimCorte = indiceDivisoria - nL; 
                int indiceInicioCorte = indiceFimCorte - linhasParaRemover;
                
                // Protege o Cabecalho (Nomes, HP, FCTs) + O Topo da arte do inimigo
                int linhasProtegidas = 8; 
                if (indiceInicioCorte < linhasProtegidas) {
                    indiceInicioCorte = linhasProtegidas;
                    linhasParaRemover = indiceFimCorte - linhasProtegidas;
                }
                
                if (linhasParaRemover > 0 && indiceInicioCorte >= 0 && indiceInicioCorte + linhasParaRemover <= static_cast<int>(linhas.size())) {
                    // Remove do fundo da arte para manter o cabecalho (nome/vida) e o topo do monstro intactos
                    linhas.erase(linhas.begin() + indiceInicioCorte, linhas.begin() + indiceInicioCorte + linhasParaRemover);
                } else if (linhasParaRemover > 0) {
                    // Fallback de seguranca
                    linhas.erase(linhas.begin(), linhas.begin() + linhasParaRemover);
                }
            } else {
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

    void renderizarCenaPadrao(const std::string& titulo, const std::vector<SistemaPersonagem*>& inimigos, SistemaPersonagem* alvoAnimacao, int frame, bool isCura, bool isMorte, Item* arma, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& aliados, SistemaPersonagem* alvoDanoJogador = nullptr, Cor corDanoJogador = Cor::RESET, int danoAnimacao = -1, const std::vector<std::string>& dropsAnimacao = {}, bool animarEntrada = false) {
        renderizarFrameBufferizado([&]() {
            (void)titulo;
            std::cout << "\n";

            TelaCombate::exibirHordaDeInimigosLadoALado(inimigos, alvoAnimacao, frame, isCura, animarEntrada, isMorte, arma, danoAnimacao, dropsAnimacao);
            
            for (const auto& msg : mensagensFixasCombate) {
                std::cout << msg;
            }
            if (!mensagensFixasCombate.empty()) std::cout << "\n";
            
            std::cout << Aparencia::cor(Cor::BRANCO);
            Aparencia::imprimirLinhaDivisoria('=');
            std::cout << Aparencia::cor(Cor::RESET);

            std::vector<std::string> painelEsquerdo;
            auto linhasJogador = TelaCombate::obterLinhasBarraDeStatusDoJogador(jogadorAtual, (alvoDanoJogador == jogadorAtual) ? corDanoJogador : Cor::RESET, (alvoDanoJogador == jogadorAtual) ? danoAnimacao : -1, (alvoDanoJogador == jogadorAtual) ? frame : 0, (alvoDanoJogador == jogadorAtual) ? isCura : false);
            painelEsquerdo.insert(painelEsquerdo.end(), linhasJogador.begin(), linhasJogador.end());

            for (auto* aliado : aliados) {
                auto linhasAliado = TelaCombate::obterLinhasBarraDeStatusDoJogador(aliado, (alvoDanoJogador == aliado) ? corDanoJogador : Cor::RESET, (alvoDanoJogador == aliado) ? danoAnimacao : -1, (alvoDanoJogador == aliado) ? frame : 0, (alvoDanoJogador == aliado) ? isCura : false);
                painelEsquerdo.insert(painelEsquerdo.end(), linhasAliado.begin(), linhasAliado.end());
            }

            std::vector<std::string> painelDireito;
            if (TelaCombate::selecaoAcaoAtual != -1) {
                std::vector<std::string> opcoes = { "Atacar", "Defender", "Habilidade", "Inventario", "Sua Ficha", "Bestiario", "Log de Batalha" };
                painelDireito.push_back("═══ ESCOLHA UMA ACAO ═══");
                for (size_t i = 0; i < 3; ++i) {
                    std::string linhaDir = "";
                    for (size_t col = 0; col < 3; ++col) {
                        size_t idx = i + col * 3;
                        if (idx < opcoes.size()) {
                            std::string op = "";
                            if (static_cast<int>(idx) == TelaCombate::selecaoAcaoAtual) {
                                op = Aparencia::cor(Cor::VERDE) + " > " + opcoes[idx] + Aparencia::cor(Cor::RESET);
                            } else {
                                op = "   " + opcoes[idx];
                            }
                            if (col < 2) {
                                int pad = 18 - Aparencia::obterComprimentoVisual(op);
                                linhaDir += op + std::string(std::max(0, pad), ' ');
                            } else {
                                linhaDir += op;
                            }
                        }
                    }
                    painelDireito.push_back(linhaDir);
                }
            } else if (TelaCombate::selecaoAlvoAtual != -1) {
                painelDireito.push_back("═══ ESCOLHA UM ALVO ═══");
                painelDireito.push_back("   < / > : Selecionar");
                painelDireito.push_back("   ENTER : Confirmar");
                painelDireito.push_back("   ESC   : Cancelar");
            } else {
                for (int i = 0; i < 4; ++i) painelDireito.push_back(std::string(37, ' '));
            }
            
            Aparencia::imprimirLadoALado(painelEsquerdo, painelDireito, 0, 5, Cor::RESET, Cor::RESET, 0);

            int larguraTerminal = Aparencia::obterLarguraTerminal();
            std::string textoDoTurno = " ╣ TURNO " + std::to_string(TelaCombate::turnoAtualVisivel) + " │ VEZ DE " + TelaCombate::nomeTurnoVisivel + " ╠ ";
            int comprimentoVisual = Aparencia::obterComprimentoVisual(textoDoTurno);
            int tracosEsq = (larguraTerminal - comprimentoVisual) / 2;
            int tracosDir = larguraTerminal - tracosEsq - comprimentoVisual;
            
            std::string linhaEsq = "";
            for (int i = 0; i < tracosEsq; ++i) linhaEsq += "═";
            std::string linhaDir = "";
            for (int i = 0; i < tracosDir; ++i) linhaDir += "═";
            
            std::cout << "\n" << Aparencia::cor(Cor::BRANCO) << linhaEsq << textoDoTurno << linhaDir << Aparencia::cor(Cor::RESET) << "\n";
        });
    }
}

int TelaCombate::turnoAtualVisivel = 1;
std::string TelaCombate::nomeTurnoVisivel = "";
int TelaCombate::selecaoAcaoAtual = -1;
int TelaCombate::selecaoAlvoAtual = -1;

void TelaCombate::definirTurnoVisivel(int turno, const std::string& nome) {
    turnoAtualVisivel = turno;
    nomeTurnoVisivel = nome;
}

void TelaCombate::adicionarMensagemFixa(const std::string& msg) {
    mensagensFixasCombate.push_back(msg);
    if (mensagensFixasCombate.size() > 6) { // Limita o numero de logs para nao empurrar o HUD para baixo demais
        mensagensFixasCombate.erase(mensagensFixasCombate.begin());
    }
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

std::vector<std::string> TelaCombate::obterLinhasBarraDeStatusDoJogador(SistemaPersonagem* jogadorAtual, Cor corDestaque, int danoAnimacao, int frameAnimacao, bool isCura) 
{
    if (jogadorAtual == nullptr) return {};
    std::string nomeDaArma = (jogadorAtual->obterArma()) ? jogadorAtual->obterArma()->obterNomeItem() + jogadorAtual->obterArma()->obterInfoStatus() : "Punhos";
    std::string nomeDoEscudo = (jogadorAtual->obterEscudo()) ? jogadorAtual->obterEscudo()->obterNomeItem() + jogadorAtual->obterEscudo()->obterInfoStatus() : "Nenhum";
    std::string nomeDaArmadura = (jogadorAtual->obterArmadura()) ? jogadorAtual->obterArmadura()->obterNomeItem() + jogadorAtual->obterArmadura()->obterInfoStatus() : "Trapos";
    
    double porcentagemDeVida = static_cast<double>(jogadorAtual->obterVida()) / jogadorAtual->obterVidaMaxima();
    std::string corLaranja = Aparencia::cor(Cor::AMARELO);
    std::string corCiano = Aparencia::cor(Cor::CIANO);
    std::string corReset = Aparencia::cor(Cor::RESET);
    
    std::string corVida = (porcentagemDeVida > 0.70) ? Aparencia::cor(Cor::VERDE) : (porcentagemDeVida > 0.30) ? corLaranja : Aparencia::cor(Cor::VERMELHO);

    std::vector<std::string> arteDoCoracao;
    if (porcentagemDeVida > 0.70)      arteDoCoracao = { "   _   _   ", "  / \\_/ \\  ", "  \\     /  ", "   \\___/   " };
    else if (porcentagemDeVida > 0.30) arteDoCoracao = { "   _   _   ", "  / \\// \\  ", "  \\  \\ /   ", "   \\___/   " };
    else                               arteDoCoracao = { "  _     _  ", " / \\   / \\ ", " \\     \\_/ ", "  \\___/    " };
    for (auto& linha : arteDoCoracao) linha = corVida + linha + corReset;

    std::string barraHP = Aparencia::gerarBarraSuave(porcentagemDeVida, 8, corVida, Aparencia::cor(Cor::CINZA));
    std::string arteDeBarraDeXp = gerarBarraDeXp(jogadorAtual, corCiano, corReset);
    std::string statusStr = gerarStringDeStatus(jogadorAtual);

    std::string fctPrint = "";
    if (danoAnimacao > 0 && frameAnimacao > 0) {
        std::string corFCT;
        std::string textoFlutuante;
        if (isCura) {
            textoFlutuante = "+" + std::to_string(danoAnimacao) + "!";
            if (frameAnimacao <= 3) corFCT = "\033[1;38;2;150;255;150m";
            else if (frameAnimacao <= 6) corFCT = "\033[1;38;2;50;255;50m";
            else if (frameAnimacao <= 9) corFCT = "\033[1;38;2;0;200;0m";
            else corFCT = "\033[1;38;2;0;150;0m";
        } else {
            textoFlutuante = "-" + std::to_string(danoAnimacao) + "!";
            if (frameAnimacao <= 3) corFCT = "\033[1;38;2;255;200;0m";
            else if (frameAnimacao <= 6) corFCT = "\033[1;38;2;255;100;0m";
            else if (frameAnimacao <= 9) corFCT = "\033[1;38;2;255;0;0m";
            else corFCT = "\033[1;38;2;150;0;0m";
        }
        fctPrint = "  " + corFCT + textoFlutuante + "\033[0m";
    }
    std::string emptyPad(10, ' ');

    std::string playerTag = (corDestaque != Cor::RESET) ? Aparencia::cor(corDestaque) + jogadorAtual->obterNome() + Aparencia::cor(Cor::RESET) : jogadorAtual->obterNome();

    std::vector<std::string> linhasParaImprimir = 
    {
        "║ " + arteDoCoracao[0] + " ║ " + playerTag + " (" + jogadorAtual->obterRaca()->obterNomeRaca() + "/" + jogadorAtual->obterNomeClasse() + ") ║ HP: [" + barraHP + corReset + "] " + corVida + std::to_string(jogadorAtual->obterVida()) + corReset + "/" + std::to_string(jogadorAtual->obterVidaMaxima()) + fctPrint + emptyPad,
        "║ " + arteDoCoracao[1] + " ║ NIVEL: " + std::to_string(jogadorAtual->obterNivel()) + " ║ XP: " + arteDeBarraDeXp + " ║ OURO: " + corLaranja + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G" + corReset + emptyPad,
        "║ " + arteDoCoracao[2] + " ║ ARMA: " + nomeDaArma + " ║ ESC: " + nomeDoEscudo + " ║ ARM: " + nomeDaArmadura + emptyPad,
        "║ " + arteDoCoracao[3] + " ║ STATUS: " + statusStr + emptyPad
    };

    return linhasParaImprimir;
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

    auto formatarFadeOut = [&](SistemaPersonagem* inimigo, const std::string& textoVisual, const std::string& textoPrint) -> std::pair<std::string, std::string> {
        if (isMorte && inimigo == alvoAnimacao && frameAnimacao > 0) {
            int maxFrames = static_cast<int>(inimigo->obterRaca()->obterAparenciaRaca().size());
            double progresso = std::min(1.0, static_cast<double>(frameAnimacao) / maxFrames);
            int intensidade = std::max(0, 255 - static_cast<int>(255.0 * progresso));
            
            if (intensidade < 30) {
                return std::make_pair(std::string(textoVisual.length(), ' '), std::string(textoVisual.length(), ' '));
            } else {
                // Retira as cores internas originais e aplica a escala de cinza de forma agressiva!
                std::string corFade = "\033[38;2;" + std::to_string(intensidade) + ";" + std::to_string(intensidade) + ";" + std::to_string(intensidade) + "m";
                return std::make_pair(textoVisual, corFade + textoVisual + "\033[0m");
            }
        }
        return std::make_pair(textoVisual, textoPrint);
    };

    imprimirLinhaHorda([&](SistemaPersonagem* inimigo, size_t i) {
        std::string tag = inimigo->obterNome();
        std::string printTag = tag;
        if (TelaCombate::selecaoAlvoAtual == static_cast<int>(i)) {
            tag = "> " + tag + " <";
            printTag = "\033[5m" + Aparencia::cor(Cor::CINZA) + tag + "\033[0m";
        }
        return formatarFadeOut(inimigo, tag, printTag);
    });

    imprimirLinhaHorda([&](SistemaPersonagem* inimigo, size_t i) {
        std::string hp = "HP: " + std::to_string(inimigo->obterVida()) + "/" + std::to_string(inimigo->obterVidaMaxima());
        return formatarFadeOut(inimigo, hp, hp);
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
            return formatarFadeOut(inimigo, visualStr, printStr);
        });
    }
    
    for (int fctLine = 0; fctLine < 2; ++fctLine) {
        imprimirLinhaHorda([&](SistemaPersonagem* inimigo, size_t i) {
            std::string visualStr = "", printStr = "";
            if (inimigo == alvoAnimacao && danoAnimacao > 0 && !isMorte && frameAnimacao > 0) {
                int targetLine = (frameAnimacao < 4) ? 1 : 0;
                if (fctLine == targetLine) {
                    std::string corFCT;
                    std::string textoFlutuante;
                    if (isCura) {
                        textoFlutuante = "+" + std::to_string(danoAnimacao) + "!";
                        if (frameAnimacao <= 2) corFCT = "\033[1;38;2;150;255;150m";
                        else if (frameAnimacao <= 4) corFCT = "\033[1;38;2;50;255;50m";
                        else corFCT = "\033[1;38;2;0;200;0m";
                    } else {
                        textoFlutuante = "-" + std::to_string(danoAnimacao) + "!";
                        if (frameAnimacao <= 2) corFCT = "\033[1;38;2;255;200;0m";
                        else if (frameAnimacao <= 4) corFCT = "\033[1;38;2;255;100;0m";
                        else if (frameAnimacao <= 6) corFCT = "\033[1;38;2;255;0;0m";
                        else corFCT = "\033[1;38;2;150;0;0m";
                    }
                    visualStr = textoFlutuante;
                    printStr = corFCT + textoFlutuante + "\033[0m";
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
                    } else if (TelaCombate::selecaoAlvoAtual == static_cast<int>(indiceDoInimigoParaDesenhar)) {
                        linhaAtual += "\033[5m" + Aparencia::cor(Cor::CINZA) + baseLinha + "\033[0m";
                    } else {
                        linhaAtual += baseLinha;
                    }
                }
            } else {
                if (TelaCombate::selecaoAlvoAtual == static_cast<int>(indiceDoInimigoParaDesenhar)) {
                    linhaAtual += "\033[5m" + Aparencia::cor(Cor::CINZA) + linhaArte + "\033[0m";
                } else {
                    linhaAtual += linhaArte;
                }
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

void TelaCombate::animarCuraNoJogador(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* alvoAnimacao, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& listaDeAliados, int curaAnimacao)
{
    for (int frame = 1; frame <= 12; ++frame) {
        Cor corAplicada = (frame % 2 == 1 && frame <= 6) ? Cor::VERDE : Cor::RESET;
        renderizarCenaPadrao(tituloCombate, listaDeInimigos, nullptr, frame, true, false, nullptr, jogadorAtual, listaDeAliados, alvoAnimacao, corAplicada, curaAnimacao);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    renderizarCenaPadrao(tituloCombate, listaDeInimigos, nullptr, 0, false, false, nullptr, jogadorAtual, listaDeAliados);
}

int TelaCombate::obterAcaoDoJogador(int turnoAtual, const std::string& nomePersonagem, const std::vector<SistemaPersonagem*>& inimigos, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& aliados) {
    definirTurnoVisivel(turnoAtual, nomePersonagem);
    selecaoAcaoAtual = 0;
    int totalOpcoes = 7;

    while (true) {
        atualizarTelaEstatica("", inimigos, jogadorAtual, aliados);
        
        unsigned char tecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
        
        if (tecla == 224 || tecla == 0 || tecla == '\033') {
            unsigned char proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (proxTecla == '[') proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (proxTecla == 72 || proxTecla == 'A') tecla = 'w';
            else if (proxTecla == 80 || proxTecla == 'B') tecla = 's';
        }

        if (tecla == 'w' || tecla == 'W') { 
            selecaoAcaoAtual--; 
            if (selecaoAcaoAtual < 0) selecaoAcaoAtual = totalOpcoes - 1; 
        }
        else if (tecla == 's' || tecla == 'S') { 
            selecaoAcaoAtual++; 
            if (selecaoAcaoAtual >= totalOpcoes) selecaoAcaoAtual = 0; 
        }
        else if (tecla == '\r' || tecla == '\n') { 
            int escolha = selecaoAcaoAtual;
            selecaoAcaoAtual = -1; // Remove cursor para as animacoes manterem a tela limpa
            atualizarTelaEstatica("", inimigos, jogadorAtual, aliados);
            return escolha + 1; 
        }
    }
}

int TelaCombate::obterAlvoAtaque(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& inimigos, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& aliados) {
    selecaoAlvoAtual = 0;
    int totalInimigos = static_cast<int>(inimigos.size());
    int oldAcao = selecaoAcaoAtual;
    selecaoAcaoAtual = -1; // Esconde o cursor do menu de acoes

    while (true) {
        atualizarTelaEstatica(tituloCombate, inimigos, jogadorAtual, aliados);
        
        unsigned char tecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
        
        if (tecla == 224 || tecla == 0 || tecla == '\033') {
            unsigned char proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (proxTecla == '[') proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (proxTecla == 75 || proxTecla == 'D') tecla = 'a'; // Seta Esquerda
            else if (proxTecla == 77 || proxTecla == 'C') tecla = 'd'; // Seta Direita
            else if (proxTecla == 27) tecla = '\033'; // Esc
        }

        if (tecla == 'a' || tecla == 'A') { 
            selecaoAlvoAtual--; 
            if (selecaoAlvoAtual < 0) selecaoAlvoAtual = totalInimigos - 1; 
        }
        else if (tecla == 'd' || tecla == 'D') { 
            selecaoAlvoAtual++; 
            if (selecaoAlvoAtual >= totalInimigos) selecaoAlvoAtual = 0; 
        }
        else if (tecla == '\r' || tecla == '\n') { 
            int escolha = selecaoAlvoAtual;
            selecaoAlvoAtual = -1; 
            selecaoAcaoAtual = oldAcao;
            atualizarTelaEstatica(tituloCombate, inimigos, jogadorAtual, aliados);
            return escolha; 
        }
        else if (tecla == '\033' || tecla == 'x' || tecla == 'X' || tecla == '0' || tecla == '\b') {
            selecaoAlvoAtual = -1;
            selecaoAcaoAtual = oldAcao;
            atualizarTelaEstatica(tituloCombate, inimigos, jogadorAtual, aliados);
            return -1;
        }
    }
}

int TelaCombate::obterAlvoItem(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& inimigos, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& aliados) {
    return obterAlvoAtaque(tituloCombate, inimigos, jogadorAtual, aliados);
}

int TelaCombate::obterEscolhaDeEscudo(const std::string& nomePersonagem, const std::vector<Item*>& listaDeEscudos) {
    std::cout << "\n" << Aparencia::margemCombate() << "═══ SELECIONE UM ESCUDO PARA " << nomePersonagem << " ═══\n";
    std::vector<std::string> opcoes;
    for (size_t indice = 0; indice < listaDeEscudos.size(); indice++) {
        opcoes.push_back(listaDeEscudos[indice]->obterNomeItem() + listaDeEscudos[indice]->obterInfoStatus());
    }
    opcoes.push_back("Cancelar");
    int escolha = ControleDeInput::lerSelecaoMenuComSetas(opcoes, false, Aparencia::margemCombate());
    if (escolha == static_cast<int>(opcoes.size()) - 1) return 0;
    return escolha + 1;
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

void TelaCombate::atualizarTelaEstatica(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& listaDeAliados, bool animarEntrada)
{
    renderizarCenaPadrao(tituloCombate, listaDeInimigos, nullptr, 0, false, false, nullptr, jogadorAtual, listaDeAliados, nullptr, Cor::RESET, -1, {}, animarEntrada);
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

void TelaCombate::animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* alvoAnimacao, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& listaDeAliados, int curaAnimacao)
{
    for (int frame = 1; frame <= 4; ++frame) {
        renderizarCenaPadrao(tituloCombate, listaDeInimigos, alvoAnimacao, frame, true, false, nullptr, jogadorAtual, listaDeAliados, nullptr, Cor::RESET, curaAnimacao);
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
