#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <thread>
#include <chrono>
#include <sstream>
#include <functional>

#include "TelaCombate.h"
#include "../../../Entidades/Personagem.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/FuncoesDialogo.h"
#include "../../../Entidades/Racas/RacaBase.h"
#include "../../../Entidades/Classes/ClasseBase.h"
#include "../../../Sistemas/Inventario/Item.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "TelaCombateLayout.h"

extern Personagem* g_inimigoAtacanteParry;
extern int g_parryStatus;

namespace {
    std::string gerarBarraDeXp(Personagem* jogadorAtual, const std::string& corXp, const std::string& corReset) {
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
            case EfeitoID::RodaAdaptacao: return {"Adaptacao", Cor::AMARELO};
            default: return {"", Cor::RESET};
        }
    }

    std::string gerarStringDeStatus(Personagem* jogadorAtual) {
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
            
            // Encontra a linha divisoria do HUD ("═════")
            int indiceDivisoria = -1;
            for (int i = static_cast<int>(linhas.size()) - 1; i >= 0; --i) {
                if (linhas[i].find("═════") != std::string::npos) {
                    indiceDivisoria = i;
                    break;
                }
            }

            if (indiceDivisoria != -1) {
                // Calcula quantas linhas os espacamentos ocupam antes da divisoria
                int nL = 1; // Para o \n vazio que exibirHordaDeInimigosLadoALado sempre deixa no final
                
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
        
        std::cout << "\033[?25l\033[H" << finalOutput << "\033[J" << std::flush;
    }

    void renderizarCenaPadrao(const std::string& titulo, const std::vector<Personagem*>& inimigos, Personagem* alvoAnimacao, int frame, bool isCura, bool isMorte, Item* arma, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados, Personagem* alvoDanoJogador = nullptr, Cor corDanoJogador = Cor::RESET, int danoAnimacao = -1, const std::vector<std::string>& dropsAnimacao = {}, bool animarEntrada = false) {
        renderizarFrameBufferizado([&]() {
            (void)titulo;
            std::cout << "\n";

            TelaCombate::exibirHordaDeInimigosLadoALado(inimigos, alvoAnimacao, frame, isCura, animarEntrada, isMorte, arma, danoAnimacao, dropsAnimacao);
            
            std::cout << Aparencia::cor(Cor::BRANCO);
            Aparencia::imprimirLinhaDivisoria('=');
            std::cout << Aparencia::cor(Cor::RESET);

            std::vector<std::string> painelEsquerdo;
            
            Personagem* destaque = jogadorAtual;
            if (alvoDanoJogador != nullptr) {
                destaque = alvoDanoJogador;
            } else if (TelaCombate::personagemHUD != nullptr) {
                destaque = TelaCombate::personagemHUD;
            } else if (TelaCombate::nomeTurnoVisivel != "" && TelaCombate::nomeTurnoVisivel != "INIMIGOS") {
                if (jogadorAtual->obterNome() == TelaCombate::nomeTurnoVisivel) destaque = jogadorAtual;
                for (auto* aliado : aliados) {
                    if (aliado->obterNome() == TelaCombate::nomeTurnoVisivel) destaque = aliado;
                }
            }

            auto linhasDestaque = TelaCombate::obterLinhasBarraDeStatusDoJogador(destaque, (alvoDanoJogador == destaque) ? corDanoJogador : Cor::RESET, (alvoDanoJogador == destaque) ? danoAnimacao : -1, (alvoDanoJogador == destaque) ? frame : 0, (alvoDanoJogador == destaque) ? isCura : false);
            painelEsquerdo.insert(painelEsquerdo.end(), linhasDestaque.begin(), linhasDestaque.end());

            std::vector<std::string> painelDireito;
            if (TelaCombate::selecaoAcaoAtual != -1) {
                painelDireito.push_back("═══ ESCOLHA UMA ACAO ═══");
                for (size_t i = 0; i < 3; ++i) {
                    std::string linhaDir = "";
                    for (size_t col = 0; col < 3; ++col) {
                        size_t idx = i + col * 3;
                        if (idx < TelaCombate::opcoesMenuAtual.size()) {
                            std::string op = "";
                            if (static_cast<int>(idx) == TelaCombate::selecaoAcaoAtual) {
                                op = Aparencia::cor(Cor::VERDE) + " > " + TelaCombate::opcoesMenuAtual[idx] + Aparencia::cor(Cor::RESET);
                            } else {
                                op = "   " + TelaCombate::opcoesMenuAtual[idx];
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
            
            Cor corDoTurno = (TelaCombate::nomeTurnoVisivel == "INIMIGOS") ? Cor::VERMELHO : Cor::VERDE;
            std::cout << "\n" << Aparencia::cor(Cor::BRANCO) << linhaEsq << Aparencia::cor(corDoTurno) << textoDoTurno << Aparencia::cor(Cor::BRANCO) << linhaDir << Aparencia::cor(Cor::RESET) << "\n";

            if (!mensagensFixasCombate.empty()) {
                std::cout << "\n";
                for (const auto& msg : mensagensFixasCombate) {
                    std::cout << msg;
                }
            }
        });
    }

    void executarAnimacao(int framesTotais, int intervaloMs, int step, const std::function<void(int)>& renderFrame, const std::function<void()>& renderFim) {
        for (int frame = 1; frame <= framesTotais; frame += step) {
            renderFrame(frame);
            std::this_thread::sleep_for(std::chrono::milliseconds(intervaloMs));
        }
        renderFim();
    }
}

int TelaCombate::turnoAtualVisivel = 1;
std::string TelaCombate::nomeTurnoVisivel = "";
int TelaCombate::selecaoAcaoAtual = -1;
int TelaCombate::selecaoAlvoAtual = -1;
bool TelaCombate::piscarSelecao = true;
std::vector<std::string> TelaCombate::opcoesMenuAtual = {};
Personagem* TelaCombate::personagemHUD = nullptr;

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

void TelaCombate::exibirLogoParaTelaDeCombate(const std::string& tituloDaTela, bool animar) 
{
    std::cout << "\033[?25l"; // Esconde o cursor
    int larguraConsole = Aparencia::obterLarguraTerminal();
    
    Aparencia::exibirPainelArte(TelaCombateLayouts::obterLogoCombate(), 95, Cor::VERMELHO, tituloDaTela, animar);
}

void TelaCombate::animarIntroducaoCombate(const std::string& titulo, const std::vector<Personagem*>& inimigos) {
    Aparencia::limparTela();
    Aparencia::ocultarCursor();

    // Captura o estado final do titulo
    std::ostringstream bufferTitulo;
    std::streambuf* oldCout = std::cout.rdbuf(bufferTitulo.rdbuf());
    exibirLogoParaTelaDeCombate(titulo, false);
    std::cout.rdbuf(oldCout);
    std::string tituloFinalComCores = bufferTitulo.str();
    std::string tituloFinalSemCores = Aparencia::removerCoresANSI(tituloFinalComCores);

    // Captura o estado final dos inimigos
    std::ostringstream bufferInimigos;
    oldCout = std::cout.rdbuf(bufferInimigos.rdbuf());
    exibirHordaDeInimigosLadoALado(inimigos, nullptr, 0, false, false);
    std::cout.rdbuf(oldCout);
    std::string inimigosFinalComCores = bufferInimigos.str();
    std::string inimigosFinalSemCores = Aparencia::removerCoresANSI(inimigosFinalComCores);

    // 1. Loop de animacao de Fade-in do Titulo
    Aparencia::animarFadeIn(15, 100, [&](int frame, int intensidade) {
        std::string conteudoFrame;
        if (frame < 15) {
            std::string corRGB = Aparencia::obterCorRGBFade(Cor::BRANCO, intensidade);
            conteudoFrame = corRGB + tituloFinalSemCores + Aparencia::cor(Cor::RESET);
        } else {
            conteudoFrame = tituloFinalComCores;
        }

        renderizarFrameBufferizado([&](){
            std::cout << conteudoFrame;
        });
    });

    // 2. Loop de animacao de Fade-in dos Inimigos
    Aparencia::animarFadeIn(15, 100, [&](int frame, int intensidade) {
        std::string conteudoFrame = tituloFinalComCores;
        if (frame < 15) {
            std::string corRGB = Aparencia::obterCorRGBFade(Cor::BRANCO, intensidade);
            conteudoFrame += corRGB + inimigosFinalSemCores + Aparencia::cor(Cor::RESET);
        } else {
            conteudoFrame += inimigosFinalComCores;
        }

        renderizarFrameBufferizado([&](){
            std::cout << conteudoFrame;
        });
    });

    std::cout << "\n";
    Aparencia::imprimirCentralizado("Prepare-se! O combate esta prestes a comecar...", Aparencia::cor(Cor::VERMELHO));
    ControleDeInput::aguardarEnter();
}

std::vector<std::string> TelaCombate::obterLinhasBarraDeStatusDoJogador(Personagem* jogadorAtual, Cor corDestaque, int danoAnimacao, int frameAnimacao, bool isCura) 
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

    std::string parryPrint = "";
    if (g_parryStatus > 0 && frameAnimacao > 0 && frameAnimacao <= 12) {
        bool isPiscarColorido = (frameAnimacao <= 6 && frameAnimacao % 2 == 1) || (frameAnimacao > 6);
        std::string txtParry;
        Cor corParry;
        if (g_parryStatus == 1) { txtParry = "  [Parry Perfeito!]"; corParry = Cor::VERDE; }
        else if (g_parryStatus == 2) { txtParry = "  [Parry Efetivo!]"; corParry = Cor::AMARELO; }
        else if (g_parryStatus == 3) { txtParry = "  [Parry Falhou!]"; corParry = Cor::VERMELHO; }

        if (isPiscarColorido) parryPrint = Aparencia::cor(corParry) + txtParry + Aparencia::cor(Cor::RESET);
        else parryPrint = std::string(txtParry.length(), ' ');
    }

    std::string playerTag = (corDestaque != Cor::RESET) ? Aparencia::cor(corDestaque) + jogadorAtual->obterNome() + Aparencia::cor(Cor::RESET) : jogadorAtual->obterNome();

    std::vector<std::string> linhasParaImprimir = 
    {
        "║ " + arteDoCoracao[0] + " ║ " + playerTag + " (" + jogadorAtual->obterRaca()->obterNomeRaca() + "/" + jogadorAtual->obterNomeClasse() + ") ║ HP: [" + barraHP + corReset + "] " + corVida + std::to_string(jogadorAtual->obterVida()) + corReset + "/" + std::to_string(jogadorAtual->obterVidaMaxima()) + parryPrint + fctPrint + emptyPad,
        "║ " + arteDoCoracao[1] + " ║ NIVEL: " + std::to_string(jogadorAtual->obterNivel()) + " ║ XP: " + arteDeBarraDeXp + " ║ OURO: " + corLaranja + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G" + corReset + emptyPad,
        "║ " + arteDoCoracao[2] + " ║ ARMA: " + nomeDaArma + " ║ ESC: " + nomeDoEscudo + " ║ ARM: " + nomeDaArmadura + emptyPad,
        "║ " + arteDoCoracao[3] + " ║ STATUS: " + statusStr + emptyPad
    };

    return linhasParaImprimir;
}

void TelaCombate::exibirHordaDeInimigosLadoALado(const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, int frameAnimacao, bool isCura, bool animarSurgimento, bool isMorte, Item* armaAtacante, int danoAnimacao, const std::vector<std::string>& dropsAnimacao) 
{
    if (listaDeInimigos.empty()) return;
    int larguraTerminal = Aparencia::obterLarguraTerminal();
    const std::vector<std::string>& arteDoInimigo = listaDeInimigos[0]->obterRaca()->obterAparenciaCombate();
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

    auto imprimirLinhaHorda = [&](const std::function<std::pair<std::string, std::string>(Personagem*, size_t)>& gerador) {
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

    auto formatarFadeOut = [&](Personagem* inimigo, const std::string& textoVisual, const std::string& textoPrint) -> std::pair<std::string, std::string> {
        if (isMorte && inimigo == alvoAnimacao && frameAnimacao > 0) {
            int maxFrames = static_cast<int>(inimigo->obterRaca()->obterAparenciaCombate().size());
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

    imprimirLinhaHorda([&](Personagem* inimigo, size_t i) {
        std::string tag = inimigo->obterNome();
        std::string printTag = tag;
        if (TelaCombate::selecaoAlvoAtual == static_cast<int>(i)) {
            tag = "> " + tag + " <";
            if (TelaCombate::piscarSelecao) {
                printTag = Aparencia::cor(Cor::AMARELO) + tag + Aparencia::cor(Cor::RESET);
            } else {
                printTag = Aparencia::cor(Cor::CINZA) + tag + Aparencia::cor(Cor::RESET);
            }
        }
        return formatarFadeOut(inimigo, tag, printTag);
    });

    imprimirLinhaHorda([&](Personagem* inimigo, size_t i) {
        std::string hp = "HP: " + std::to_string(inimigo->obterVida()) + "/" + std::to_string(inimigo->obterVidaMaxima());
        return formatarFadeOut(inimigo, hp, hp);
    });

    bool hordaTemDebuffs = false;
    for (auto* ini : listaDeInimigos) {
        std::vector<EfeitoID> effs; ini->obterIDsEfeitosAtivos(effs);
        if (!effs.empty()) { hordaTemDebuffs = true; break; }
    }

    if (hordaTemDebuffs) {
        imprimirLinhaHorda([&](Personagem* inimigo, size_t i) {
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
        imprimirLinhaHorda([&](Personagem* inimigo, size_t i) {
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
            
            Personagem* inimigoAtual = listaDeInimigos[indiceDoInimigoParaDesenhar];
            std::string linhaArte = arteDoInimigo[indiceDaLinhaDaArte];
            
            if (isMorte && inimigoAtual == alvoAnimacao) {
                int totalLinhasArte = static_cast<int>(arteDoInimigo.size());
                if (static_cast<int>(indiceDaLinhaDaArte) >= totalLinhasArte - frameAnimacao) {
                    linhaArte = std::string(visivelLen, ' ');
                    
                    if (frameAnimacao >= totalLinhasArte) {
                        int maxTextLen = 10; // Tamanho de "DERROTADO!"
                        for (const auto& d : dropsAnimacao) {
                            if (static_cast<int>(d.length()) > maxTextLen) maxTextLen = static_cast<int>(d.length());
                        }
                        if (maxTextLen > visivelLen - 4) maxTextLen = visivelLen - 4;
                        if (maxTextLen < 0) maxTextLen = 0;

                        int totalBoxLines = static_cast<int>(dropsAnimacao.size()) + 3; // +1 texto base, +2 bordas
                        int startDropLine = (totalLinhasArte - totalBoxLines) / 2;
                        if (startDropLine < 0) startDropLine = 0;
                        
                        int currentLineIndex = static_cast<int>(indiceDaLinhaDaArte) - startDropLine;
                        if (currentLineIndex >= 0 && currentLineIndex < totalBoxLines) {
                            std::string corBorda = Aparencia::cor(Cor::CINZA);
                            std::string corReset = Aparencia::cor(Cor::RESET);
                            
                            int esp = (visivelLen - (maxTextLen + 4)) / 2;
                            if (esp < 0) esp = 0;
                            int rem = visivelLen - esp - (maxTextLen + 4);
                            if (rem < 0) rem = 0;
                            
                            if (currentLineIndex == 0) {
                                std::string b = "╔"; for(int k=0; k<maxTextLen+2; ++k) b += "═"; b += "╗";
                                linhaArte = std::string(esp, ' ') + corBorda + b + corReset + std::string(rem, ' ');
                            } else if (currentLineIndex == totalBoxLines - 1) {
                                std::string b = "╚"; for(int k=0; k<maxTextLen+2; ++k) b += "═"; b += "╝";
                                linhaArte = std::string(esp, ' ') + corBorda + b + corReset + std::string(rem, ' ');
                            } else {
                                int textIdx = currentLineIndex - 1;
                                std::string innerTxt = (textIdx == 0) ? "DERROTADO!" : dropsAnimacao[textIdx - 1];
                                
                                std::string corDrop = Aparencia::cor(Cor::BRANCO);
                                if (textIdx == 0) corDrop = "\033[5m" + Aparencia::cor(Cor::VERMELHO); // Piscar Vermelho
                                else if (innerTxt.find("XP") != std::string::npos) corDrop = Aparencia::cor(Cor::CIANO);
                                else if (innerTxt.find("G") != std::string::npos) corDrop = Aparencia::cor(Cor::AMARELO);
                                
                                int txtLen = static_cast<int>(innerTxt.length());
                                if (txtLen > maxTextLen) {
                                    innerTxt = innerTxt.substr(0, maxTextLen);
                                    txtLen = maxTextLen;
                                }
                                
                                int pEsq = (maxTextLen - txtLen) / 2;
                                int pDir = maxTextLen - txtLen - pEsq;
                                
                                std::string middle = corBorda + "║ " + std::string(pEsq, ' ') + corDrop + innerTxt + corReset + corBorda + std::string(pDir, ' ') + " ║" + corReset;
                                linhaArte = std::string(esp, ' ') + middle + std::string(rem, ' ');
                            }
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
                        linhaAtual += (TelaCombate::piscarSelecao ? Aparencia::cor(Cor::AMARELO) : Aparencia::cor(Cor::CINZA)) + baseLinha + Aparencia::cor(Cor::RESET);
                    } else {
                        linhaAtual += baseLinha;
                    }
                }
            } else {
                if (TelaCombate::selecaoAlvoAtual == static_cast<int>(indiceDoInimigoParaDesenhar)) {
                    linhaAtual += (TelaCombate::piscarSelecao ? Aparencia::cor(Cor::AMARELO) : Aparencia::cor(Cor::CINZA)) + linhaArte + Aparencia::cor(Cor::RESET);
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

void TelaCombate::animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* atacante, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int danoAnimacao)
{
    Item* armaAtacante = (atacante != nullptr) ? atacante->obterArma() : nullptr;
    
    executarAnimacao(8, 100, 1, [&](int frame) {
        renderizarCenaPadrao(tituloCombate, listaDeInimigos, alvoAnimacao, frame, false, false, armaAtacante, jogadorAtual, listaDeAliados, nullptr, Cor::RESET, danoAnimacao);
    }, [&]() {
        renderizarCenaPadrao(tituloCombate, listaDeInimigos, alvoAnimacao, 0, false, false, nullptr, jogadorAtual, listaDeAliados);
    });
}

void TelaCombate::animarCuraNoJogador(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int curaAnimacao)
{
    executarAnimacao(12, 100, 1, [&](int frame) {
        Cor corAplicada = (frame % 2 == 1 && frame <= 6) ? Cor::VERDE : Cor::RESET;
        renderizarCenaPadrao(tituloCombate, listaDeInimigos, nullptr, frame, true, false, nullptr, jogadorAtual, listaDeAliados, alvoAnimacao, corAplicada, curaAnimacao);
    }, [&]() {
        renderizarCenaPadrao(tituloCombate, listaDeInimigos, nullptr, 0, false, false, nullptr, jogadorAtual, listaDeAliados);
    });
}

int TelaCombate::obterAcaoDoJogador(int turnoAtual, Personagem* personagemAgindo, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) {
    definirTurnoVisivel(turnoAtual, personagemAgindo->obterNome());
    personagemHUD = personagemAgindo;
    selecaoAcaoAtual = 0;
    

    while (true) {
        opcoesMenuAtual.clear();
        
        // Se o HUD estiver focado em alguem que NAO e o dono do turno, restringe as acoes
        if (personagemHUD != nullptr && personagemHUD != personagemAgindo) {
            opcoesMenuAtual.push_back("Voltar a Acao");
            opcoesMenuAtual.push_back("Ver Aliados");
        } else {
            opcoesMenuAtual.push_back("Atacar");
            opcoesMenuAtual.push_back("Defender");
            opcoesMenuAtual.push_back("Habilidade");
            
            bool temInventario = (personagemAgindo->obterInventario() != nullptr && personagemAgindo->obterInventario()->obterTodosOsItens().size() > 0);
            if (temInventario || personagemAgindo == jogadorAtual) {
                opcoesMenuAtual.push_back("Inventario");
            }
            
            opcoesMenuAtual.push_back("Ficha");
            if (!aliados.empty()) opcoesMenuAtual.push_back("Ver Aliados");
            opcoesMenuAtual.push_back("Diario");
            opcoesMenuAtual.push_back("Log Batalha");
        }

        int totalOpcoes = static_cast<int>(opcoesMenuAtual.size());

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
            std::string op = opcoesMenuAtual[selecaoAcaoAtual];
            if (op == "Ver Aliados") {
                selecionarHUDDeAliado(jogadorAtual, aliados);
                selecaoAcaoAtual = 0; // Reseta selecao para evitar falhas ao recarregar a lista
                continue;
            }
            if (op == "Voltar a Acao") {
                personagemHUD = personagemAgindo;
                selecaoAcaoAtual = 0;
                continue;
            }

            int escolha = selecaoAcaoAtual;
            selecaoAcaoAtual = -1; // Remove cursor para as animacoes manterem a tela limpa
            personagemHUD = nullptr; // Garante que o HUD volte a mostrar o personagem agindo de fato nas proximas animacoes
            atualizarTelaEstatica("", inimigos, jogadorAtual, aliados);
            
            if (op == "Atacar") return 1;
            if (op == "Defender") return 2;
            if (op == "Habilidade") return 3;
            if (op == "Inventario") return 4;
            if (op == "Ficha") return 5;
            if (op == "Diario") return 6;
            if (op == "Log Batalha") return 7;
            return 0;
        }
    }
}

int TelaCombate::obterAlvoAtaque(const std::string& tituloCombate, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) {
    selecaoAlvoAtual = 0;
    int totalInimigos = static_cast<int>(inimigos.size());
    int oldAcao = selecaoAcaoAtual;
    selecaoAcaoAtual = -1; // Esconde o cursor do menu de acoes

    piscarSelecao = true;
    atualizarTelaEstatica(tituloCombate, inimigos, jogadorAtual, aliados);
    auto tempoBlink = std::chrono::steady_clock::now();

    while (true) {
        auto agora = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(agora - tempoBlink).count() >= 150) {
            piscarSelecao = !piscarSelecao;
            tempoBlink = agora;
            atualizarTelaEstatica(tituloCombate, inimigos, jogadorAtual, aliados);
        }

        if (ControleDeInput::teclaPressionada()) {
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
                piscarSelecao = true;
                tempoBlink = std::chrono::steady_clock::now();
                atualizarTelaEstatica(tituloCombate, inimigos, jogadorAtual, aliados);
            }
            else if (tecla == 'd' || tecla == 'D') { 
                selecaoAlvoAtual++; 
                if (selecaoAlvoAtual >= totalInimigos) selecaoAlvoAtual = 0; 
                piscarSelecao = true;
                tempoBlink = std::chrono::steady_clock::now();
                atualizarTelaEstatica(tituloCombate, inimigos, jogadorAtual, aliados);
            }
            else if (tecla == '\r' || tecla == '\n') { 
                int escolha = selecaoAlvoAtual;
                selecaoAlvoAtual = -1; 
                selecaoAcaoAtual = oldAcao;
                piscarSelecao = true;
                atualizarTelaEstatica(tituloCombate, inimigos, jogadorAtual, aliados);
                return escolha; 
            }
            else if (tecla == '\033' || tecla == 'x' || tecla == 'X' || tecla == '0' || tecla == '\b') {
                selecaoAlvoAtual = -1;
                selecaoAcaoAtual = oldAcao;
                piscarSelecao = true;
                atualizarTelaEstatica(tituloCombate, inimigos, jogadorAtual, aliados);
                return -1;
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

void TelaCombate::selecionarHUDDeAliado(Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) {
    std::vector<std::string> opcoesHUD;
    opcoesHUD.push_back(jogadorAtual->obterNome());
    for (auto* aliado : aliados) {
        opcoesHUD.push_back(aliado->obterNome());
    }
    opcoesHUD.push_back("Voltar");

    std::cout << "\n" << Aparencia::margemCombate() << "═══ SELECIONE O PERSONAGEM PARA VER INFO ═══\n";
    int escolha = ControleDeInput::lerSelecaoMenuComSetas(opcoesHUD, false, Aparencia::margemCombate());
    
    if (escolha >= 0 && escolha < static_cast<int>(opcoesHUD.size()) - 1) {
        if (escolha == 0) personagemHUD = jogadorAtual;
        else personagemHUD = aliados[escolha - 1];
    }
}

int TelaCombate::obterAlvoItem(const std::string& tituloCombate, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) {
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
    std::string msg = FuncoesDialogo::formatarMsgSistema("Os inimigos sao mais ageis e atacam primeiro!", Cor::VERMELHO);
    std::cout << "\n" << Aparencia::margemCombate() << msg << "\n";
    Aparencia::registrarLogBatalha(msg);
    ControleDeInput::aguardarEnter();
}

void TelaCombate::notificarTurnoExtra(int destrezaJogador, int maxDestrezaInimigos) {
    std::string msg = FuncoesDialogo::formatarMsgSistema("Sua agilidade extrema (" + std::to_string(destrezaJogador) + " VS " + std::to_string(maxDestrezaInimigos) + ") permite que voce aja novamente!", Cor::CIANO);
    std::cout << "\n" << Aparencia::margemCombate() << msg << "\n";
    Aparencia::registrarLogBatalha(msg);
    ControleDeInput::aguardarEnter();
}

void TelaCombate::notificarDesprevencaoInventario() {
    std::string msg = FuncoesDialogo::formatarMsgSistema("O inimigo te pegou desprevinido enquanto voce usava o inventario!");
    std::cout << "\n" << Aparencia::margemCombate() << msg << "\n";
    Aparencia::registrarLogBatalha(msg);
}

void TelaCombate::notificarSemEscudos(const std::string& nomePersonagem) {
    std::cout << "\n" << Aparencia::margemCombate() << FuncoesDialogo::formatarMsgSistema(nomePersonagem + " nao possui escudos no inventario para usar!", Cor::VERMELHO) << "\n";
}

void TelaCombate::notificarDesequilibrioDefesa(const std::string& nomePersonagem) {
    std::string msg = FuncoesDialogo::formatarMsgSistema(nomePersonagem + " se desequilibrou e precisa de 1 turno para poder defender novamente!", Cor::VERMELHO);
    std::cout << "\n" << Aparencia::margemCombate() << msg << "\n";
    Aparencia::registrarLogBatalha(msg);
    ControleDeInput::aguardarEnter();
}

void TelaCombate::notificarPosturaDefensiva(const std::string& nomePersonagem, const std::string& nomeEscudo) {
    std::string msg = FuncoesDialogo::formatarMsgSistema(nomePersonagem + " assumiu uma postura defensiva com " + nomeEscudo + "!", Cor::BRANCO);
    std::cout << "\n" << Aparencia::margemCombate() << msg << "\n";
    Aparencia::registrarLogBatalha(msg);
    ControleDeInput::aguardarEnter();
}

void TelaCombate::notificarAcaoInvalida() {
    std::cout << "\n" << Aparencia::margemCombate() << FuncoesDialogo::formatarMsgSistema("Acao invalida!", Cor::VERMELHO) << "\n";
    ControleDeInput::aguardarEnter();
}

void TelaCombate::notificarCancelamentoItem() {
    std::cout << "\n" << Aparencia::margemCombate() << FuncoesDialogo::formatarMsgSistema("Uso do frasco cancelado. O item voltou para a mochila.") << "\n";
}

void TelaCombate::notificarRequisitoNaoAtendido(const std::string& mensagemRequisito) {
    if (mensagemRequisito.substr(0, 1) == "\n") {
        std::cout << "\n" << Aparencia::margemCombate() << mensagemRequisito.substr(1);
    } else {
        std::cout << Aparencia::margemCombate() << mensagemRequisito;
    }
    ControleDeInput::aguardarEnter();
}

void TelaCombate::atualizarTelaEstatica(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, bool animarEntrada)
{
    renderizarCenaPadrao(tituloCombate, listaDeInimigos, nullptr, 0, false, false, nullptr, jogadorAtual, listaDeAliados, nullptr, Cor::RESET, -1, {}, animarEntrada);
}

void TelaCombate::animarMorteInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* inimigoMorto, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, const std::vector<std::string>& drops)
{
    if (listaDeInimigos.empty()) return;
    
    int totalLinhas = static_cast<int>(listaDeInimigos[0]->obterRaca()->obterAparenciaCombate().size());

    executarAnimacao(totalLinhas, 25, 2, [&](int frame) { // step = 2 Para dar um efeito acelerado satisfatório
        renderizarCenaPadrao(tituloCombate, listaDeInimigos, inimigoMorto, frame, false, true, nullptr, jogadorAtual, listaDeAliados);
    }, [&]() {
        // Frame final para garantir que toda a arte seja apagada e os DROPS apareçam centralizados no fantasma da imagem!
        renderizarCenaPadrao(tituloCombate, listaDeInimigos, inimigoMorto, totalLinhas, false, true, nullptr, jogadorAtual, listaDeAliados, nullptr, Cor::RESET, -1, drops);
    });
}

void TelaCombate::animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int curaAnimacao)
{
    executarAnimacao(4, 100, 1, [&](int frame) {
        renderizarCenaPadrao(tituloCombate, listaDeInimigos, alvoAnimacao, frame, true, false, nullptr, jogadorAtual, listaDeAliados, nullptr, Cor::RESET, curaAnimacao);
    }, [&]() {
        renderizarCenaPadrao(tituloCombate, listaDeInimigos, alvoAnimacao, 0, true, false, nullptr, jogadorAtual, listaDeAliados);
    });
}

void TelaCombate::animarDanoNoJogador(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, bool isParry, int danoAnimacao)
{
    Cor corDestaque = isParry ? Cor::CIANO : Cor::VERMELHO;

    executarAnimacao(12, 100, 1, [&](int frame) {
        Cor corAplicada = (frame % 2 == 1 && frame <= 6) ? corDestaque : Cor::RESET;
        renderizarCenaPadrao(tituloCombate, listaDeInimigos, nullptr, frame, false, false, nullptr, jogadorAtual, listaDeAliados, alvoAnimacao, corAplicada, danoAnimacao);
    }, [&]() {
        renderizarCenaPadrao(tituloCombate, listaDeInimigos, nullptr, 0, false, false, nullptr, jogadorAtual, listaDeAliados);
    });
}
