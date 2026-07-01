#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <thread>
#include <chrono>
#include <sstream>
#include <functional>
#include <unordered_map>

#include "TelaCombate.h"
#include "../../../Entidades/Personagem.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/FuncoesDialogo.h"
#include "../../../Entidades/Racas/RacaBase.h"
#include "../../../Entidades/Classes/ClasseBase.h"
#include "../../../Sistemas/Inventario/Item.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "TelaCombateLayout.h"
#include "../../../Sistemas/Combate/Combate3DRenderer.h"
#include "../../../Sistemas/Combate/Parry.h"
#include "../TelaCena3D.h"

extern Personagem* g_inimigoAtacanteParry;
extern int g_parryStatus;

namespace {
    struct PlayerHUDVisualState {
        double hpFantasma = -1.0;
        double hpAnterior = -1.0;
        int ouroAnterior = -1;
        int xpAnterior = -1;
        long long tempoUltimaRecompensaOuro = 0;
        long long tempoUltimaRecompensaXp = 0;
    };
    static std::unordered_map<Personagem*, PlayerHUDVisualState> hudStates;


    std::string gerarBarraDeXp(Personagem* jogadorAtual, const std::string& corXp, const std::string& corReset) {
        int tamanho = 8;
        double porcentagem = static_cast<double>(jogadorAtual->obterXpAtual()) / std::max(1, jogadorAtual->obterXpParaSubir());
        std::string barra = Aparencia::gerarBarraGradiente(porcentagem, tamanho, Cor::CIANO);
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
            case EfeitoID::Necrose: return {"Necrose", Cor::MAGENTA};
            case EfeitoID::MiraCerteira: return {"Mira Certeira", Cor::AMARELO};
            default: return {"", Cor::RESET};
        }
    }

    std::string gerarStringDeStatus(Personagem* jogadorAtual, int tempoMs) {
        std::vector<EfeitoID> efeitos;
        jogadorAtual->obterIDsEfeitosAtivos(efeitos);
        std::string status = "";
        bool piscaPerigo = (tempoMs / 300) % 2 == 0;
        for (EfeitoID id : efeitos) {
            auto disp = obterDisplayEfeito(id);
            if (!disp.nome.empty()) {
                std::string corUsada = Aparencia::cor(disp.cor);
                if ((id == EfeitoID::Sangramento || id == EfeitoID::Lentidao || id == EfeitoID::Fraqueza) && piscaPerigo) {
                    corUsada = "\033[38;2;255;100;100m"; // Rosa/Vermelho claro perigo
                }
                status += corUsada + "[" + disp.nome + "]" + Aparencia::cor(Cor::RESET) + " ";
            }
        }
        return status.empty() ? "Nenhum" : status;
    }

    std::vector<std::string> mensagensFixasCombate;
    std::string overlayLogoOnAnsiLine(const std::string& backgroundLine, const std::vector<std::string>& logoChars, int startX, const std::string& fgColor, int larguraTerminal);

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
        int larguraTerminal = Aparencia::obterLarguraTerminal();
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

        // Safety net de largura: trunca linhas que excedem a largura do terminal para evitar line wrap
        for (auto& linha : linhas) {
            int compVisual = Aparencia::obterComprimentoVisual(linha);
            if (compVisual > larguraTerminal) {
                // Trunca respeitando sequencias ANSI (nao corta no meio de uma)
                std::string resultado = "";
                int contadorVisual = 0;
                for (size_t i = 0; i < linha.length() && contadorVisual < larguraTerminal; ) {
                    if (linha[i] == '\033' && i + 1 < linha.length() && linha[i+1] == '[') {
                        // Copia a sequencia ANSI inteira sem contar como caractere visual
                        size_t inicioAnsi = i;
                        i += 2;
                        while (i < linha.length() && !(linha[i] >= 0x40 && linha[i] <= 0x7E)) i++;
                        if (i < linha.length()) i++; // Inclui o caractere final
                        resultado += linha.substr(inicioAnsi, i - inicioAnsi);
                    } else {
                        // Caractere normal (possivelmente multi-byte UTF-8)
                        unsigned char c = static_cast<unsigned char>(linha[i]);
                        int len = 1;
                        if ((c & 0x80) == 0) len = 1;
                        else if ((c & 0xE0) == 0xC0) len = 2;
                        else if ((c & 0xF0) == 0xE0) len = 3;
                        else if ((c & 0xF8) == 0xF0) len = 4;
                        resultado += linha.substr(i, len);
                        i += len;
                        contadorVisual++;
                    }
                }
                resultado += "\033[0m"; // Reseta cores no ponto de truncamento
                linha = resultado;
            }
        }

        std::string finalOutput = "";
        for (size_t i = 0; i < linhas.size(); ++i) {
            finalOutput += linhas[i] + "\033[K";
            if (i < linhas.size() - 1) finalOutput += "\n";
        }
        
        std::cout << "\033[?25l\033[H" << finalOutput << "\033[J\033[0m" << std::flush;
    }

    void renderizarCenaPadrao(const std::string& titulo, const std::vector<Personagem*>& inimigos, Personagem* alvoAnimacao, int frame, bool isCura, bool isMorte, Item* arma, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados, Personagem* alvoDanoJogador = nullptr, Cor corDanoJogador = Cor::RESET, int danoAnimacao = -1, const std::vector<std::string>& dropsAnimacao = {}, bool animarEntrada = false) {
        auto agora = std::chrono::steady_clock::now();
        int tempoMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(agora.time_since_epoch()).count());
        renderizarFrameBufferizado([&]() {
            (void)titulo;
            std::cout << "\n";

            int larguraTerminal = Aparencia::obterLarguraTerminal();

            // === Preparar dados do HUD (Comum para ambos os modos) ===
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
            const int LARGURA_PAINEL_DIREITO = 50;

            auto padLinhaDireita = [&](std::string& linha) {
                int visualLen = Aparencia::obterComprimentoVisual(linha);
                if (visualLen < LARGURA_PAINEL_DIREITO) {
                    linha += std::string(LARGURA_PAINEL_DIREITO - visualLen, ' ');
                }
            };

            if (TelaCombate::selecaoAcaoAtual != -1) {
                int blinkVal = (tempoMs / 400) % 2;
                if (blinkVal < 0) blinkVal += 2;
                std::string cursorIcon = (blinkVal == 0) ? ">  " : " > ";

                std::string tituloAcao = TelaCombate::isModo3D ? "═══ ESCOLHA UMA ACAO ═══" : "public void EscolhaAcao() {";
                padLinhaDireita(tituloAcao);
                painelDireito.push_back(tituloAcao);
                for (size_t i = 0; i < 3; ++i) {
                    std::string linhaDir = "";
                    for (size_t col = 0; col < 3; ++col) {
                        size_t idx = i + col * 3;
                        if (idx < TelaCombate::opcoesMenuAtual.size()) {
                            std::string op = "";
                            if (static_cast<int>(idx) == TelaCombate::selecaoAcaoAtual) {
                                op = Aparencia::cor(Cor::VERDE) + cursorIcon + TelaCombate::opcoesMenuAtual[idx] + Aparencia::cor(Cor::RESET);
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
                    padLinhaDireita(linhaDir);
                    painelDireito.push_back(linhaDir);
                }
                std::string emptyLine = TelaCombate::isModo3D ? "" : "}";
                padLinhaDireita(emptyLine);
                painelDireito.push_back(emptyLine);
            } else if (TelaCombate::selecaoAlvoAtual != -1) {
                std::vector<std::string> linhasAlvo;
                if (TelaCombate::isModo3D) {
                    linhasAlvo = {
                        "═══ ESCOLHA UM ALVO ═══",
                        "   < / > : Selecionar",
                        "   ENTER : Confirmar",
                        "   ESC   : Cancelar",
                        ""
                    };
                } else {
                    linhasAlvo = {
                        "public void EscolhaUmAlvo() {",
                        "    auto select = Input::EsquerdaDireita();",
                        "    auto conf   = Input::Enter();",
                        "    auto cancel = Input::Esc();",
                        "}"
                    };
                }
                for(auto& linha : linhasAlvo) {
                    padLinhaDireita(linha);
                    painelDireito.push_back(linha);
                }
            } else {
                for (int i = 0; i < 5; ++i) {
                    std::string emptyLine = "";
                    padLinhaDireita(emptyLine);
                    painelDireito.push_back(emptyLine);
                }
            }

            const size_t ALTURA_FIXA_HUD = 7;
            while (painelEsquerdo.size() < ALTURA_FIXA_HUD) {
                painelEsquerdo.push_back("");
            }
            while (painelDireito.size() < ALTURA_FIXA_HUD) {
                std::string emptyLine = "";
                padLinhaDireita(emptyLine);
                painelDireito.push_back(emptyLine);
            }

            if (TelaCombate::isModo3D) {
                int framesDanoJog = 0;
                if (alvoDanoJogador != nullptr && danoAnimacao > 0 && !isCura) {
                    framesDanoJog = frame;
                }

                std::vector<std::string> quadro3D = Combate3DRenderer::renderizarQuadro(
                    TelaCombate::tituloMapaAtual, 
                    jogadorAtual, 
                    inimigos,
                    alvoAnimacao,
                    frame,
                    framesDanoJog,
                    danoAnimacao,
                    isCura,
                    tempoMs,
                    isMorte,
                    dropsAnimacao
                );

                int altura3D = quadro3D.size();

                // Wrap HUD panels with white UTF-8 border and black background
                std::string bgPreto = "\033[48;2;0;0;0m";
                std::string corBordaBranca = "\033[38;2;255;255;255m";

                // 1. painelEsquerdo
                int maxEsqW = 0;
                for (const auto& l : painelEsquerdo) {
                    int len = Aparencia::obterComprimentoVisual(l);
                    if (len > maxEsqW) maxEsqW = len;
                }
                std::vector<std::string> painelEsquerdoBordado;
                
                std::string topo = bgPreto + corBordaBranca + "╔";
                for (int k = 0; k < maxEsqW - 1; ++k) topo += "═";
                topo += "╗\033[0m";
                painelEsquerdoBordado.push_back(topo);
                
                for (auto& l : painelEsquerdo) {
                    std::string linha = l;
                    size_t pos = 0;
                    while ((pos = linha.find("\033[0m", pos)) != std::string::npos) {
                        linha.replace(pos, 4, "\033[0m" + bgPreto);
                        pos += 4 + bgPreto.length();
                    }
                    
                    std::string linhaProcessada = bgPreto + linha;
                    size_t posV = linhaProcessada.find("║");
                    if (posV != std::string::npos) {
                        linhaProcessada.replace(posV, 3, corBordaBranca + "║\033[0m" + bgPreto);
                    }
                    
                    int len = Aparencia::obterComprimentoVisual(linhaProcessada);
                    int pad = maxEsqW - len;
                    if (pad > 0) linhaProcessada += std::string(pad, ' ');
                    linhaProcessada += corBordaBranca + "║\033[0m";
                    painelEsquerdoBordado.push_back(linhaProcessada);
                }
                
                std::string base = bgPreto + corBordaBranca + "╚";
                for (int k = 0; k < maxEsqW - 1; ++k) base += "═";
                base += "╝\033[0m";
                painelEsquerdoBordado.push_back(base);
                painelEsquerdo = std::move(painelEsquerdoBordado);

                // 2. painelDireito
                int maxDirW = LARGURA_PAINEL_DIREITO + 4;
                std::vector<std::string> painelDireitoBordado;
                
                std::string topoDir = bgPreto + corBordaBranca + "╔";
                for (int k = 0; k < maxDirW - 2; ++k) topoDir += "═";
                topoDir += "╗\033[0m";
                painelDireitoBordado.push_back(topoDir);
                
                for (auto& l : painelDireito) {
                    std::string linha = l;
                    size_t pos = 0;
                    while ((pos = linha.find("\033[0m", pos)) != std::string::npos) {
                        linha.replace(pos, 4, "\033[0m" + bgPreto);
                        pos += 4 + bgPreto.length();
                    }
                    
                    std::string linhaProcessada = bgPreto + corBordaBranca + "║\033[0m" + bgPreto + " " + linha;
                    int len = Aparencia::obterComprimentoVisual(linhaProcessada);
                    int pad = maxDirW - len - 1;
                    if (pad > 0) linhaProcessada += std::string(pad, ' ');
                    linhaProcessada += corBordaBranca + "║\033[0m";
                    painelDireitoBordado.push_back(linhaProcessada);
                }
                
                std::string baseDir = bgPreto + corBordaBranca + "╚";
                for (int k = 0; k < maxDirW - 2; ++k) baseDir += "═";
                baseDir += "╝\033[0m";
                painelDireitoBordado.push_back(baseDir);
                painelDireito = std::move(painelDireitoBordado);

                // Compile floating messages
                std::vector<std::string> painelMensagens;

                auto limparMsg = [](const std::string& msg) -> std::string {
                    std::string clean = msg;
                    while (!clean.empty() && (clean.back() == '\n' || clean.back() == '\r')) {
                        clean.pop_back();
                    }
                    size_t first = clean.find_first_not_of(" \t");
                    if (first != std::string::npos) {
                        clean = clean.substr(first);
                    } else {
                        return "";
                    }
                    size_t last = clean.find_last_not_of(" \t");
                    if (last != std::string::npos) {
                        clean = clean.substr(0, last + 1);
                    }
                    return clean;
                };

                if (TelaCombate::selecaoAlvoAtual != -1 && TelaCombate::selecaoAlvoAtual < static_cast<int>(inimigos.size())) {
                    std::string feedbackAlvo = "-> SELECIONADO: Inimigo (" + std::to_string(TelaCombate::selecaoAlvoAtual + 1) + ") - " + inimigos[TelaCombate::selecaoAlvoAtual]->obterNome() + " <-";
                    painelMensagens.push_back("\033[1;33m" + feedbackAlvo + "\033[0m");
                }

                std::string textoDoTurno = "TURNO " + std::to_string(TelaCombate::turnoAtualVisivel) + " │ VEZ DE " + TelaCombate::nomeTurnoVisivel;
                painelMensagens.push_back(textoDoTurno);

                if (!ControleDeInput::enterPromptText.empty()) {
                    std::string prompt = limparMsg(ControleDeInput::enterPromptText);
                    if (!prompt.empty()) painelMensagens.push_back("\033[5m" + prompt + "\033[0m");
                }

                if (!Parry::minigameMessage.empty()) {
                    std::string msg = limparMsg(Parry::minigameMessage);
                    if (!msg.empty()) painelMensagens.push_back(msg);
                }
                if (!Parry::minigameBar.empty()) {
                    std::string msg = limparMsg(Parry::minigameBar);
                    if (!msg.empty()) painelMensagens.push_back(msg);
                }

                if (alvoDanoJogador != nullptr && danoAnimacao > 0 && frame > 0 && !isCura) {
                    std::string nomeAtacante = "Desconhecido";
                    if (g_inimigoAtacanteParry != nullptr) {
                        for (size_t i = 0; i < inimigos.size(); ++i) {
                            if (inimigos[i] == g_inimigoAtacanteParry) {
                                nomeAtacante = inimigos[i]->obterNome() + " (" + std::to_string(i + 1) + ")";
                                break;
                            }
                        }
                        if (nomeAtacante == "Desconhecido") nomeAtacante = g_inimigoAtacanteParry->obterNome();
                    }
                    std::string mensagemDano = nomeAtacante + " causou " + std::to_string(danoAnimacao) + " de dano em " + alvoDanoJogador->obterNome() + "!";
                    painelMensagens.push_back(mensagemDano);
                }

                if (!mensagensFixasCombate.empty()) {
                    for (const auto& msg : mensagensFixasCombate) {
                        std::string msgLimpa = limparMsg(msg);
                        if (!msgLimpa.empty()) {
                            painelMensagens.push_back(msgLimpa);
                        }
                    }
                }

                std::vector<std::string> painelMensagensBordado;
                int maxMsgW = 0;
                if (!painelMensagens.empty()) {
                    for (const auto& l : painelMensagens) {
                        int len = Aparencia::obterComprimentoVisual(l);
                        if (len > maxMsgW) maxMsgW = len;
                    }
                    if (maxMsgW < 40) maxMsgW = 40;
                    int boxMsgW = maxMsgW + 6; // 6 para margens internas e bordas

                    std::string topoMsg = bgPreto + corBordaBranca + "╔";
                    for (int k = 0; k < boxMsgW - 2; ++k) topoMsg += "═";
                    topoMsg += "╗\033[0m";
                    painelMensagensBordado.push_back(topoMsg);

                    for (const auto& l : painelMensagens) {
                        std::string linha = l;
                        size_t pos = 0;
                        while ((pos = linha.find("\033[0m", pos)) != std::string::npos) {
                            linha.replace(pos, 4, "\033[0m" + bgPreto);
                            pos += 4 + bgPreto.length();
                        }

                        int visualLen = Aparencia::obterComprimentoVisual(linha);
                        int totalPadding = (boxMsgW - 2) - visualLen;
                        int padEsq = std::max(0, totalPadding / 2);
                        int padDir = std::max(0, totalPadding - padEsq);

                        std::string linhaProcessada = bgPreto + corBordaBranca + "║\033[0m" + bgPreto + std::string(padEsq, ' ') + linha + std::string(padDir, ' ') + corBordaBranca + "║\033[0m";
                        painelMensagensBordado.push_back(linhaProcessada);
                    }

                    std::string baseMsg = bgPreto + corBordaBranca + "╚";
                    for (int k = 0; k < boxMsgW - 2; ++k) baseMsg += "═";
                    baseMsg += "╝\033[0m";
                    painelMensagensBordado.push_back(baseMsg);
                }

                // Layout math for centered HUD panels at the bottom and message box directly above it
                int totalHUDWidth = maxEsqW + 6 + maxDirW;
                int startHUDX = (larguraTerminal - totalHUDWidth) / 2;
                if (startHUDX < 0) startHUDX = 0;

                int startEsqX = startHUDX;
                int startDirX = startHUDX + maxEsqW + 6;

                size_t maxLinhasHUD = std::max(painelEsquerdo.size(), painelDireito.size());
                int startHUDY = altura3D - maxLinhasHUD - 1;
                int startMsgY = startHUDY - painelMensagensBordado.size();

                for (size_t row = 0; row < maxLinhasHUD; ++row) {
                    int y = startHUDY + row;
                    if (y >= 0 && y < altura3D) {
                        if (row < painelEsquerdo.size()) {
                            quadro3D[y] = Aparencia::sobreporPainelNaLinhaAnsi(quadro3D[y], painelEsquerdo[row], startEsqX);
                        }
                        if (row < painelDireito.size()) {
                            quadro3D[y] = Aparencia::sobreporPainelNaLinhaAnsi(quadro3D[y], painelDireito[row], startDirX);
                        }
                    }
                }

                if (!painelMensagensBordado.empty()) {
                    int startMsgX = (larguraTerminal - (maxMsgW + 6)) / 2;
                    if (startMsgX < 0) startMsgX = 0;

                    for (size_t row = 0; row < painelMensagensBordado.size(); ++row) {
                        int y = startMsgY + row;
                        if (y >= 0 && y < altura3D) {
                            quadro3D[y] = Aparencia::sobreporPainelNaLinhaAnsi(quadro3D[y], painelMensagensBordado[row], startMsgX);
                        }
                    }
                }

                // Compile and overlay drops panel centered in the remaining 3D scene area above HUD/messages
                std::vector<std::string> dropsUsados = dropsAnimacao.empty() ? TelaCombate::g_dropsAtivos : dropsAnimacao;
                std::vector<std::string> painelDrops;
                if (!dropsUsados.empty()) {
                    int maxDropsW = 12; // "DERROTADO!" length is 10
                    for (const auto& d : dropsUsados) {
                        std::string cleanD = d;
                        size_t first = cleanD.find_first_not_of(" \t");
                        if (first != std::string::npos) cleanD = cleanD.substr(first);
                        size_t last = cleanD.find_last_not_of(" \t\r\n");
                        if (last != std::string::npos) cleanD = cleanD.substr(0, last + 1);
                        int len = Aparencia::obterComprimentoVisual(cleanD);
                        if (len > maxDropsW) maxDropsW = len;
                    }
                    int boxW = maxDropsW + 6; // margens e bordas
                    
                    std::string topoDrops = bgPreto + corBordaBranca + "╔";
                    for (int k = 0; k < boxW - 2; ++k) topoDrops += "═";
                    topoDrops += "╗\033[0m";
                    painelDrops.push_back(topoDrops);
                    
                    // Row for "DERROTADO!"
                    {
                        std::string label = "DERROTADO!";
                        std::string innerTxt = "\033[5;1;38;2;255;50;50m" + label + "\033[0m";
                        int txtLen = label.length();
                        int totalPadding = (boxW - 2) - txtLen;
                        int pEsq = totalPadding / 2;
                        int pDir = totalPadding - pEsq;
                        std::string line = bgPreto + corBordaBranca + "║\033[0m" + bgPreto + std::string(pEsq, ' ') + innerTxt + std::string(pDir, ' ') + corBordaBranca + "║\033[0m";
                        painelDrops.push_back(line);
                    }
                    
                    // Rows for drops
                    for (const auto& d : dropsUsados) {
                        std::string cleanD = d;
                        size_t first = cleanD.find_first_not_of(" \t");
                        if (first != std::string::npos) cleanD = cleanD.substr(first);
                        size_t last = cleanD.find_last_not_of(" \t\r\n");
                        if (last != std::string::npos) cleanD = cleanD.substr(0, last + 1);

                        std::string color = Aparencia::cor(Cor::BRANCO);
                        if (cleanD.find("XP") != std::string::npos) color = Aparencia::cor(Cor::CIANO);
                        else if (cleanD.find("G") != std::string::npos) color = Aparencia::cor(Cor::AMARELO);
                        
                        std::string innerTxt = color + cleanD + "\033[0m";
                        int txtLen = Aparencia::obterComprimentoVisual(cleanD);
                        int totalPadding = (boxW - 2) - txtLen;
                        int pEsq = totalPadding / 2;
                        int pDir = totalPadding - pEsq;
                        std::string line = bgPreto + corBordaBranca + "║\033[0m" + bgPreto + std::string(pEsq, ' ') + innerTxt + std::string(pDir, ' ') + corBordaBranca + "║\033[0m";
                        painelDrops.push_back(line);
                    }
                    
                    std::string baseDrops = bgPreto + corBordaBranca + "╚";
                    for (int k = 0; k < boxW - 2; ++k) baseDrops += "═";
                    baseDrops += "╝\033[0m";
                    painelDrops.push_back(baseDrops);

                    // Find index of dead enemy to position the drops box on its column
                    Personagem* deadEnemy = (alvoAnimacao != nullptr && isMorte) ? alvoAnimacao : TelaCombate::g_inimigoMortoComDrops;
                    int deadIdx = -1;
                    if (deadEnemy != nullptr) {
                        for (size_t k = 0; k < inimigos.size(); ++k) {
                            if (inimigos[k] == deadEnemy) {
                                deadIdx = static_cast<int>(k);
                                break;
                            }
                        }
                    }

                    int startDropsX = (larguraTerminal - boxW) / 2; // center screen fallback
                    if (deadIdx != -1 && !inimigos.empty()) {
                        int numInimigos = static_cast<int>(inimigos.size());
                        int larguraColuna = larguraTerminal / numInimigos;
                        int centroX = deadIdx * larguraColuna + larguraColuna / 2;
                        startDropsX = centroX - boxW / 2;
                    }
                    if (startDropsX < 0) startDropsX = 0;
                    if (startDropsX + boxW > larguraTerminal) startDropsX = larguraTerminal - boxW;
                    
                    int areaDisponivelY = startMsgY; // area above message box and HUD
                    int startDropsY = (areaDisponivelY - painelDrops.size()) / 2;
                    if (startDropsY < 0) startDropsY = 0;
                    
                    for (size_t row = 0; row < painelDrops.size(); ++row) {
                        int y = startDropsY + row;
                        if (y >= 0 && y < altura3D) {
                            quadro3D[y] = Aparencia::sobreporPainelNaLinhaAnsi(quadro3D[y], painelDrops[row], startDropsX);
                        }
                    }
                }

                for (const auto& linha : quadro3D) {
                    std::cout << linha << "\n";
                }
            } else {
                // Desenha a horda classicamente (que imprime no std::cout e será interceptada pelo buffer)
                TelaCombate::exibirHordaDeInimigosLadoALado(inimigos, alvoAnimacao, frame, isCura, animarEntrada, isMorte, arma, danoAnimacao, dropsAnimacao);

                Aparencia::imprimirLinhaDivisoria('=');
                std::cout << Aparencia::cor(Cor::RESET);

                Aparencia::imprimirLadoALado(painelEsquerdo, painelDireito, 0, 5, Cor::RESET, Cor::RESET, 0);

                std::string textoDoTurno = " TURNO " + std::to_string(TelaCombate::turnoAtualVisivel) + " | VEZ DE " + TelaCombate::nomeTurnoVisivel + " ";
                int comprimentoVisual = Aparencia::obterComprimentoVisual(textoDoTurno);
                int tracosEsq = (larguraTerminal - comprimentoVisual - 6) / 2;
                int tracosDir = larguraTerminal - tracosEsq - comprimentoVisual - 6;
                if (tracosEsq < 0) tracosEsq = 0;
                if (tracosDir < 0) tracosDir = 0;
                
                std::string linhaEsq = "/* ";
                for (int i = 0; i < tracosEsq; ++i) linhaEsq += "-";
                std::string linhaDir = "";
                for (int i = 0; i < tracosDir; ++i) linhaDir += "-";
                linhaDir += " */";
                
                Cor corDoTurno = (TelaCombate::nomeTurnoVisivel == "INIMIGOS") ? Cor::VERMELHO : Cor::VERDE;
                std::string corBordaEsqDir = "\033[38;2;96;139;78m"; // Cor de comentario verde do VSCode
                
                std::cout << "\n" << corBordaEsqDir << linhaEsq << Aparencia::cor(corDoTurno) << textoDoTurno << corBordaEsqDir << linhaDir << Aparencia::cor(Cor::RESET) << "\n";

                if (alvoDanoJogador != nullptr && danoAnimacao > 0 && frame > 0 && !isCura) {
                    int shakeX = (frame % 2 == 0) ? 4 : -4;
                    std::string padEsq = (shakeX > 0) ? std::string(shakeX, ' ') : "";
                    std::string padDir = (shakeX < 0) ? std::string(-shakeX, ' ') : "";

                    std::string nomeAtacante = "Desconhecido";
                    if (g_inimigoAtacanteParry != nullptr) {
                        for (size_t i = 0; i < inimigos.size(); ++i) {
                            if (inimigos[i] == g_inimigoAtacanteParry) {
                                nomeAtacante = inimigos[i]->obterNome() + " (" + std::to_string(i + 1) + ")";
                                break;
                            }
                        }
                        if (nomeAtacante == "Desconhecido") nomeAtacante = g_inimigoAtacanteParry->obterNome();
                    }

                    std::string mensagemDano = nomeAtacante + " causou " + std::to_string(danoAnimacao) + " de dano em " + alvoDanoJogador->obterNome() + "!";

                    std::cout << "\n";
                    if (frame % 2 == 0) std::cout << "\n";
                    
                    int espacosCentralizar = std::max(0, (larguraTerminal - Aparencia::obterComprimentoVisual(mensagemDano)) / 2);
                    std::cout << padEsq << std::string(espacosCentralizar, ' ') << "\033[1;38;2;255;50;50m" << mensagemDano << Aparencia::cor(Cor::RESET) << padDir << "\n";
                }

                if (!mensagensFixasCombate.empty()) {
                    std::cout << "\n";
                    for (const auto& msg : mensagensFixasCombate) {
                        std::cout << msg;
                    }
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

bool TelaCombate::isModo3D = false;
Personagem* TelaCombate::g_inimigoMortoComDrops = nullptr;
std::vector<std::string> TelaCombate::g_dropsAtivos = {};
std::vector<std::string> TelaCombate::matrizDoMapaAtual;
float TelaCombate::jogadorPosX = 0.0f;
float TelaCombate::jogadorPosY = 0.0f;
float TelaCombate::jogadorAngulo = 0.0f;
std::string TelaCombate::tituloMapaAtual = "";

void TelaCombate::configurarContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo) {
    isModo3D = modo3D;
    matrizDoMapaAtual = matriz;
    jogadorPosX = posX;
    jogadorPosY = posY;
    jogadorAngulo = angulo;
    tituloMapaAtual = titulo;
}

void TelaCombate::limparMensagensFixas() {
    mensagensFixasCombate.clear();
}

void TelaCombate::exibirLogoParaTelaDeCombate(const std::string& tituloDaTela, bool animar) 
{
    std::cout << "\033[?25l"; // Esconde o cursor
    
    Aparencia::exibirPainelArte(TelaCombateLayouts::obterLogoCombate(), 95, Cor::VERMELHO, tituloDaTela, animar);
}

void TelaCombate::animarIntroducaoCombate(const std::string& titulo, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual) {
    Aparencia::limparTela();
    Aparencia::ocultarCursor();

    if (isModo3D && jogadorAtual != nullptr) {
        int larguraTerminal = Aparencia::obterLarguraTerminal();
        int alturaTerminal = Aparencia::obterAlturaTerminal();
        if (larguraTerminal <= 0) larguraTerminal = 120;
        if (alturaTerminal <= 0) alturaTerminal = 40;
        int alturaHUD = 16;
        int altura3D = std::max(10, alturaTerminal - alturaHUD);

        // ═══════════════════════════════════════════════════════════════════
        //  Fase 1: Fundo 3D em Fade In (sem inimigos)
        // ═══════════════════════════════════════════════════════════════════
        std::vector<std::string> quadroBackground = Combate3DRenderer::renderizarQuadro(
            TelaCombate::tituloMapaAtual, 
            jogadorAtual, 
            {}, // Nenhum inimigo
            nullptr, 0, 0, -1, false, 0, false, {}, 1.0f
        );

        // Renderiza fundo completo uma vez e fade-in apenas das linhas do fundo (sem \n final para evitar scroll)
        for (size_t i = 0; i+1 < quadroBackground.size(); ++i) {
            std::cout << "\033[" << (i + 1) << ";1H" << quadroBackground[i] << "\033[K\n";
        }
        if (!quadroBackground.empty()) {
            int y = static_cast<int>(quadroBackground.size()) - 1;
            std::cout << "\033[" << (y + 1) << ";1H" << quadroBackground[y] << "\033[K";
        }
        
        Aparencia::animarFadeIn(15, 30, [&](int /*frame*/, int intensidade) {
            float ratio = intensidade / 255.0f;
            std::ostringstream buffer;
            for (size_t i = 0; i < quadroBackground.size(); ++i) {
                buffer << "\033[" << (i + 1) << ";1H" << Aparencia::fadarLinhaAnsi(quadroBackground[i], ratio) << "\033[K";
                if (i + 1 < quadroBackground.size()) buffer << "\n";
            }
            std::cout << buffer.str() << std::flush;
        });

        class CenaIntroducaoCombate3D : public TelaCena3D {
        private:
            Personagem* jogadorAtual;
            std::vector<Personagem*> inimigos;
            std::vector<std::string> quadroBackground;
            
        public:
            CenaIntroducaoCombate3D(Personagem* jogador, const std::vector<Personagem*>& inims, const std::vector<std::string>& fundoLivre)
                : jogadorAtual(jogador), inimigos(inims), quadroBackground(fundoLivre) {}
                
        protected:
            std::vector<std::string> obterLogo() const override {
                return TelaCombateLayouts::obterLogoCombate();
            }

            std::string obterCorLogo() const override {
                return "\033[1;31m";
            }

            std::vector<std::string> obterFundoInicial() const override {
                return quadroBackground;
            }

            std::vector<std::string> comporFundoComElementos(float opacity) const override {
                auto agora = std::chrono::steady_clock::now();
                int tempoMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(agora.time_since_epoch()).count());
                return Combate3DRenderer::renderizarQuadro(
                    TelaCombate::tituloMapaAtual, 
                    jogadorAtual, 
                    inimigos,
                    nullptr, 0, 0, -1, false, tempoMs, false, {}, opacity
                );
            }
        };

        CenaIntroducaoCombate3D cena(jogadorAtual, inimigos, quadroBackground);
        cena.executar();

    } else {
        // 2D: Titulo com fade padronizado (suave, Y=2)
        exibirLogoParaTelaDeCombate(titulo, true);

        // Captura o estado final dos inimigos (2D)
        std::ostringstream bufferInimigos;
        std::streambuf* oldCout = std::cout.rdbuf(bufferInimigos.rdbuf());
        exibirHordaDeInimigosLadoALado(inimigos, nullptr, 0, false, false);
        std::cout.rdbuf(oldCout);
        std::string inimigosFinalComCores = bufferInimigos.str();
        std::string inimigosFinalSemCores = Aparencia::removerCoresANSI(inimigosFinalComCores);

        // Loop de animacao de Fade-in dos Inimigos (2D)
        Aparencia::animarFadeIn(15, 100, [&](int /*frame*/, int intensidade) {
            std::string corRGB = Aparencia::obterCorRGBFade(Cor::BRANCO, intensidade);
            std::string conteudoFrame = corRGB + inimigosFinalSemCores + Aparencia::cor(Cor::RESET);
            std::ostringstream buffer;
            buffer << "\033[H";
            {
                std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());
                exibirLogoParaTelaDeCombate(titulo, false);
                std::cout << conteudoFrame;
                std::cout.rdbuf(oldCout);
            }
            std::cout << buffer.str() << std::flush;
        });
    }

    std::cout << "\n\033[0m";
    Aparencia::imprimirCentralizado("Prepare-se! O combate esta prestes a comecar...", Aparencia::cor(Cor::VERMELHO));
    ControleDeInput::aguardarEnter();
}

std::vector<std::string> TelaCombate::obterLinhasBarraDeStatusDoJogador(Personagem* jogadorAtual, Cor corDestaque, int danoAnimacao, int frameAnimacao, bool isCura) 
{
    if (jogadorAtual == nullptr) return {};
    std::string nomeDaArma = (jogadorAtual->obterArma()) ? jogadorAtual->obterArma()->obterNomeItem() + jogadorAtual->obterArma()->obterInfoStatus() : "Punhos";
    std::string nomeDoEscudo = (jogadorAtual->obterEscudo()) ? jogadorAtual->obterEscudo()->obterNomeItem() + jogadorAtual->obterEscudo()->obterInfoStatus() : "Nenhum";
    std::string nomeDaArmadura = (jogadorAtual->obterArmadura()) ? jogadorAtual->obterArmadura()->obterNomeItem() + jogadorAtual->obterArmadura()->obterInfoStatus() : "Trapos";
    
    Item* consumivelRapido = jogadorAtual->obterConsumivelRapido();
    std::string nomeCuraRapida = "Vazio";
    if (consumivelRapido) {
        nomeCuraRapida = consumivelRapido->obterNomeItem() + " (" + std::to_string(jogadorAtual->obterInventario()->contarItem(consumivelRapido->obterNomeItem())) + "x)";
    }

    auto agora = std::chrono::steady_clock::now();
    int tempoMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(agora.time_since_epoch()).count());

    double porcentagemDeVida = static_cast<double>(jogadorAtual->obterVida()) / jogadorAtual->obterVidaMaxima();
    std::string corLaranja = Aparencia::cor(Cor::AMARELO);
    std::string corCiano = Aparencia::cor(Cor::CIANO);
    std::string corReset = Aparencia::cor(Cor::RESET);
    
    std::string corVida = (porcentagemDeVida > 0.70) ? Aparencia::cor(Cor::VERDE) : (porcentagemDeVida > 0.30) ? corLaranja : Aparencia::cor(Cor::VERMELHO);

    int velocidadeBatida = (porcentagemDeVida <= 0.30) ? 300 : 1000;
    int modBatida = tempoMs % velocidadeBatida;
    if (modBatida < 0) modBatida += velocidadeBatida;
    int frameBatida = modBatida < 150 ? 1 : 0;

    std::string corCoracao = corVida;
    if (frameBatida == 1) {
        if (porcentagemDeVida > 0.70) corCoracao = "\033[38;2;150;255;150m"; // Verde mais claro
        else if (porcentagemDeVida > 0.30) corCoracao = "\033[38;2;255;255;150m"; // Amarelo mais claro
        else corCoracao = "\033[38;2;255;100;100m"; // Vermelho mais claro
    }

    std::vector<std::string> arteDoCoracao;
    if (TelaCombate::isModo3D) {
        if (porcentagemDeVida > 0.70) {
            if (frameBatida == 0) arteDoCoracao = { " ▄██▄ ▄██▄ ", "███████████", " ▀███████▀ ", "   ▀███▀   " };
            else                  arteDoCoracao = { "           ", "  ▄█████▄  ", "  ▀█████▀  ", "    ▀█▀    " };
        } else if (porcentagemDeVida > 0.30) {
            if (frameBatida == 0) arteDoCoracao = { " ▄██▄ ▄▄   ", "██████████ ", " ▀██████▀  ", "   ▀███▀   " };
            else                  arteDoCoracao = { "           ", "  ▄████▄   ", "  ▀████▀   ", "    ▀█▀    " };
        } else {
            if (frameBatida == 0) arteDoCoracao = { " ▄█▄   ▄█▄ ", "████   ████", " ▀███████▀ ", "   ▀███▀   " };
            else                  arteDoCoracao = { "           ", "  ▄█▄ ▄█▄  ", "  ▀█████▀  ", "    ▀█▀    " };
        }
    } else {
        if (porcentagemDeVida > 0.70) {
            if (frameBatida == 0) arteDoCoracao = { "   _   _   ", "  / \\_/ \\  ", "  \\     /  ", "   \\___/   " };
            else                  arteDoCoracao = { "   _   _   ", "  / \\_/ \\  ", "  \\ \\_/ /  ", "   \\___/   " };
        } else if (porcentagemDeVida > 0.30) {
            if (frameBatida == 0) arteDoCoracao = { "   _   _   ", "  / \\// \\  ", "  \\  \\ /   ", "   \\___/   " };
            else                  arteDoCoracao = { "   _   _   ", "  / \\// \\  ", "  \\ \\/ /   ", "   \\___/   " };
        } else {
            if (frameBatida == 0) arteDoCoracao = { "  _     _  ", " / \\   / \\ ", " \\     \\_/ ", "  \\___/    " };
            else                  arteDoCoracao = { "  _     _  ", " / \\   / \\ ", " \\ \\_  \\_/ ", "  \\___/    " };
        }
    }
    for (auto& linha : arteDoCoracao) linha = corCoracao + linha + corReset;

    PlayerHUDVisualState& estadoHUD = hudStates[jogadorAtual];
    double vidaAtual = jogadorAtual->obterVida();
    if (estadoHUD.hpAnterior == -1.0) estadoHUD.hpFantasma = vidaAtual;
    if (vidaAtual < estadoHUD.hpFantasma) {
        estadoHUD.hpFantasma -= std::max(0.5, (estadoHUD.hpFantasma - vidaAtual) * 0.15);
        if (estadoHUD.hpFantasma < vidaAtual) estadoHUD.hpFantasma = vidaAtual;
    } else {
        estadoHUD.hpFantasma = vidaAtual;
    }
    estadoHUD.hpAnterior = vidaAtual;

    double porcentagemFantasma = estadoHUD.hpFantasma / jogadorAtual->obterVidaMaxima();
    int tamanhoBarra = 8;
    int qtdReal = static_cast<int>(porcentagemDeVida * tamanhoBarra * 8);
    int qtdFantasma = static_cast<int>(porcentagemFantasma * tamanhoBarra * 8);
    std::string barraHP = "";
    std::string corFantasma = "\033[38;2;255;100;100m";
    std::string corFundoHP = Aparencia::cor(Cor::CINZA);
    Cor baseCorVida = (porcentagemDeVida > 0.70) ? Cor::VERDE : (porcentagemDeVida > 0.30) ? Cor::AMARELO : Cor::VERMELHO;
    for (int i = 0; i < tamanhoBarra; ++i) {
        int intensidade = 130 + (125 * i) / std::max(1, tamanhoBarra - 1);
        std::string corAtual = Aparencia::obterCorRGBFade(baseCorVida, intensidade);
        int charIdx = i * 8;
        if (qtdReal >= charIdx + 4) barraHP += corAtual + "█";
        else if (qtdFantasma >= charIdx + 4) barraHP += corFantasma + "█";
        else barraHP += corFundoHP + "░";
    }

    int ouroAtual = jogadorAtual->obterInventario()->obterOuro();
    int xpAtual = jogadorAtual->obterXpAtual();
    if (estadoHUD.ouroAnterior == -1) estadoHUD.ouroAnterior = ouroAtual;
    if (estadoHUD.xpAnterior == -1) estadoHUD.xpAnterior = xpAtual;
    if (ouroAtual > estadoHUD.ouroAnterior) estadoHUD.tempoUltimaRecompensaOuro = tempoMs;
    if (xpAtual > estadoHUD.xpAnterior) estadoHUD.tempoUltimaRecompensaXp = tempoMs;
    estadoHUD.ouroAnterior = ouroAtual;
    estadoHUD.xpAnterior = xpAtual;
    int modOuro = (tempoMs / 150) % 2;
    if (modOuro < 0) modOuro += 2;
    int modXp = (tempoMs / 150) % 2;
    if (modXp < 0) modXp += 2;
    bool piscarOuro = (tempoMs - estadoHUD.tempoUltimaRecompensaOuro < 1500) && (modOuro == 0);
    bool piscarXp = (tempoMs - estadoHUD.tempoUltimaRecompensaXp < 1500) && (modXp == 0);
    std::string corOuro = piscarOuro ? "\033[38;2;255;255;100m" : corLaranja;
    std::string corXpStr = piscarXp ? "\033[38;2;150;255;255m" : corCiano;

    std::string arteDeBarraDeXp = gerarBarraDeXp(jogadorAtual, corXpStr, corReset);
    std::string statusStr = gerarStringDeStatus(jogadorAtual, tempoMs);

    std::string fctPrint = "";
    if (danoAnimacao > 0 && frameAnimacao > 0 && isCura) {
        std::string corFCT;
        std::string textoFlutuante = "+" + std::to_string(danoAnimacao) + "!";
        if (frameAnimacao <= 3) corFCT = "\033[1;38;2;150;255;150m";
        else if (frameAnimacao <= 6) corFCT = "\033[1;38;2;50;255;50m";
        else if (frameAnimacao <= 9) corFCT = "\033[1;38;2;0;200;0m";
        else corFCT = "\033[1;38;2;0;150;0m";
        fctPrint = "  " + corFCT + textoFlutuante + "\033[0m";
    }
    std::string emptyPad(10, ' ');

    std::string parryPrint = "";
    if (g_parryStatus > 0 && frameAnimacao > 0 && frameAnimacao <= 12) {
        bool isPiscarColorido = (frameAnimacao <= 6 && frameAnimacao % 2 == 1) || (frameAnimacao > 6);
        std::string txtParry;
        Cor corParry = Cor::RESET;
        if (g_parryStatus == 1) { txtParry = "  [Parry Perfeito!]"; corParry = Cor::VERDE; }
        else if (g_parryStatus == 2) { txtParry = "  [Parry Efetivo!]"; corParry = Cor::AMARELO; }
        else if (g_parryStatus == 3) { txtParry = "  [Parry Falhou!]"; corParry = Cor::VERMELHO; }

        if (isPiscarColorido) parryPrint = Aparencia::cor(corParry) + txtParry + Aparencia::cor(Cor::RESET);
        else parryPrint = std::string(txtParry.length(), ' ');
    }

    std::string playerTag = (corDestaque != Cor::RESET) ? Aparencia::cor(corDestaque) + jogadorAtual->obterNome() + Aparencia::cor(Cor::RESET) : jogadorAtual->obterNome();

    std::vector<std::string> linhasParaImprimir = {
        "║ " + arteDoCoracao[0] + " ║ " + playerTag + " (" + jogadorAtual->obterRaca()->obterNomeRaca() + "/" + jogadorAtual->obterNomeClasse() + ") ║ HP: [" + barraHP + corReset + "] " + corVida + std::to_string(jogadorAtual->obterVida()) + corReset + "/" + std::to_string(jogadorAtual->obterVidaMaxima()) + parryPrint + fctPrint + emptyPad,
        "║ " + arteDoCoracao[1] + " ║ NIVEL: " + std::to_string(jogadorAtual->obterNivel()) + " ║ XP: " + arteDeBarraDeXp + " ║ OURO: " + corOuro + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G" + corReset + emptyPad,
        "║ " + arteDoCoracao[2] + " ║ ARMA: " + nomeDaArma + " ║ CURA RAP.: " + nomeCuraRapida + emptyPad,
        "║ " + arteDoCoracao[3] + " ║ ESC: " + nomeDoEscudo + " ║ ARM: " + nomeDaArmadura + emptyPad,
        "║ " + std::string(11, ' ') + " ║ STATUS: " + statusStr + emptyPad
    };

    return linhasParaImprimir;
}

void TelaCombate::exibirHordaDeInimigosLadoALado(const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, int frameAnimacao, bool isCura, bool animarSurgimento, bool isMorte, Item* /*armaAtacante*/, int danoAnimacao, const std::vector<std::string>& dropsAnimacao) 
{
    if (listaDeInimigos.empty()) return;
    int larguraTerminal = Aparencia::obterLarguraTerminal();
    int alturaTerminal = Aparencia::obterAlturaTerminal();
    const std::vector<std::string>& arteOriginalDoInimigo = listaDeInimigos[0]->obterRaca()->obterAparenciaRaca();
    
    std::vector<std::string> arteReduzidaLocal;
    const std::vector<std::string>* arteDoInimigoPtr = &arteOriginalDoInimigo;
    
    // Aplica o padrao global de compressao para inimigos (se a arte for maior que algumas linhas minimas)
    if (static_cast<int>(arteOriginalDoInimigo.size()) > 10) {
        arteReduzidaLocal = Aparencia::reduzirEscalaAscii(arteOriginalDoInimigo, Aparencia::FATOR_COMPRESSAO_GLOBAL, Aparencia::FATOR_COMPRESSAO_GLOBAL);
        arteDoInimigoPtr = &arteReduzidaLocal;
    }
    const std::vector<std::string>& arteDoInimigo = *arteDoInimigoPtr;
    
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

    auto agora = std::chrono::steady_clock::now();
    int tempoMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(agora.time_since_epoch()).count());
    
    std::vector<int> offsetsIdle;
    for (size_t idx = 0; idx < listaDeInimigos.size(); ++idx) {
        Personagem* ini = listaDeInimigos[idx];
        if (ini->obterVida() <= 0 || ini->obterMorteAnimada() || (ini == alvoAnimacao && frameAnimacao > 0 && !isMorte && !isCura)) {
            offsetsIdle.push_back(0); // Mantém estático durante animações de dano ou morte
        } else {
            // Ciclo de balanço: Move horizontalmente de forma suave (esquerda e direita)
            int cycle = ((tempoMs + idx * 500) / 200) % 8;
            if (cycle < 0) cycle += 8;
            int offset = 0;
            if (cycle == 1 || cycle == 3) offset = 1;
            else if (cycle == 2) offset = 2;
            else if (cycle == 5 || cycle == 7) offset = -1;
            else if (cycle == 6) offset = -2;
            offsetsIdle.push_back(offset);
        }
    }

    auto imprimirLinhaHorda = [&](const std::function<std::pair<std::string, std::string>(Personagem*, size_t)>& gerador) {
        for (size_t i = 0; i < listaDeInimigos.size(); ++i) {
            auto [textoVisual, textoPrint] = gerador(listaDeInimigos[i], i);
            int compVisual = Aparencia::obterComprimentoVisual(textoVisual);
            int espacosEsq = std::max(0, (larguraSeparadaParaCadaColuna - compVisual) / 2);
            espacosEsq += offsetsIdle[i];
            if (espacosEsq < 0) espacosEsq = 0;
            
            std::cout << std::string(espacosEsq, ' ') << textoPrint;
            if (i < listaDeInimigos.size() - 1) {
                int espacosDir = std::max(0, larguraSeparadaParaCadaColuna - espacosEsq - compVisual);
                std::cout << std::string(espacosDir, ' ');
            }
        }
        std::cout << "\n";
    };

    auto formatarFadeOut = [&](Personagem* inimigo, const std::string& textoVisual, const std::string& textoPrint) -> std::pair<std::string, std::string> {
        int compVisual = Aparencia::obterComprimentoVisual(textoVisual);
        if (inimigo->obterMorteAnimada()) {
            return std::make_pair(std::string(compVisual, ' '), std::string(compVisual, ' '));
        }

        if (isMorte && inimigo == alvoAnimacao && frameAnimacao > 0) {
            int maxFrames = static_cast<int>(inimigo->obterRaca()->obterAparenciaRaca().size());
            double progresso = std::min(1.0, static_cast<double>(frameAnimacao) / maxFrames);
            int intensidade = std::max(0, 255 - static_cast<int>(255.0 * progresso));
            
            if (intensidade < 30) {
                return std::make_pair(std::string(compVisual, ' '), std::string(compVisual, ' '));
            } else {
                // Retira as cores internas originais e aplica a escala de cinza de forma agressiva!
                std::string corFade = "\033[38;2;" + std::to_string(intensidade) + ";" + std::to_string(intensidade) + ";" + std::to_string(intensidade) + "m";
                return std::make_pair(textoVisual, corFade + textoVisual + "\033[0m");
            }
        }
        return std::make_pair(textoVisual, textoPrint);
    };

    for (int fctLine = 0; fctLine < 2; ++fctLine) {
        imprimirLinhaHorda([&](Personagem* inimigo, size_t /*i*/) {
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

    bool hordaTemDebuffs = false;
    for (auto* ini : listaDeInimigos) {
        std::vector<EfeitoID> effs; ini->obterIDsEfeitosAtivos(effs);
        if (!effs.empty()) { hordaTemDebuffs = true; break; }
    }

    if (hordaTemDebuffs) {
        imprimirLinhaHorda([&](Personagem* inimigo, size_t /*i*/) {
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

    imprimirLinhaHorda([&](Personagem* inimigo, size_t i) {
        std::string tag = inimigo->obterNome();
        if (listaDeInimigos.size() > 1) {
            tag += " (" + std::to_string(i + 1) + ")";
        }
        std::string printTag = tag;
        if (TelaCombate::selecaoAlvoAtual == static_cast<int>(i)) {
            tag = "> " + tag + " <";
            if (TelaCombate::piscarSelecao) {
                printTag = Aparencia::cor(Cor::AMARELO) + tag + Aparencia::cor(Cor::RESET);
            } else {
                printTag = Aparencia::cor(Cor::CINZA) + tag + Aparencia::cor(Cor::RESET);
            }
        } else if (inimigo == g_inimigoAtacanteParry) {
            printTag = "\033[38;2;255;140;0m" + tag + Aparencia::cor(Cor::RESET); // Laranja Escuro
        }
        return formatarFadeOut(inimigo, tag, printTag);
    });

    imprimirLinhaHorda([&](Personagem* inimigo, size_t /*i*/) {
        double pctVida = static_cast<double>(inimigo->obterVida()) / std::max(1, inimigo->obterVidaMaxima());
        Cor corVidaInimigo = (pctVida > 0.7) ? Cor::VERDE : (pctVida > 0.3) ? Cor::AMARELO : Cor::VERMELHO;
        std::string barraPrint = Aparencia::gerarBarraGradiente(pctVida, 8, corVidaInimigo);
        std::string barraVisual = Aparencia::removerCoresANSI(barraPrint);
        
        std::string hpStr = std::to_string(inimigo->obterVida()) + "/" + std::to_string(inimigo->obterVidaMaxima());
        std::string textoVisual = "HP: [" + barraVisual + "] " + hpStr;
        std::string printHp = "HP: [" + barraPrint + Aparencia::cor(Cor::RESET) + "] " + hpStr;
        
        if (inimigo == g_inimigoAtacanteParry) {
            printHp = "HP: [" + barraPrint + Aparencia::cor(Cor::RESET) + "] \033[38;2;255;140;0m" + hpStr + Aparencia::cor(Cor::RESET);
        }
        return formatarFadeOut(inimigo, textoVisual, printHp);
    });


    
    std::cout << "\n";
        
    std::vector<std::string> linhasDaArte;
    for (size_t indiceDaLinhaDaArte = 0; indiceDaLinhaDaArte < arteDoInimigo.size(); indiceDaLinhaDaArte++) 
    {
        std::string linhaAtual = "";
        for (size_t indiceDoInimigoParaDesenhar = 0; indiceDoInimigoParaDesenhar < listaDeInimigos.size(); indiceDoInimigoParaDesenhar++) 
        {
            Personagem* inimigoAtual = listaDeInimigos[indiceDoInimigoParaDesenhar];
            
            int offset = offsetsIdle[indiceDoInimigoParaDesenhar];
            int linhaReal = static_cast<int>(indiceDaLinhaDaArte);
            std::string linhaArte;
            int visivelLen;
            
            if (linhaReal >= 0 && linhaReal < static_cast<int>(arteDoInimigo.size())) {
                linhaArte = arteDoInimigo[linhaReal];
                auto caracteres = splitUTF8(linhaArte);
                visivelLen = static_cast<int>(caracteres.size());
            } else {
                visivelLen = static_cast<int>(splitUTF8(arteDoInimigo[0]).size()); // Margem de segurança de tamanho
                linhaArte = std::string(visivelLen, ' ');
            }
            
            int espacosParaCentralizarAArte = (larguraSeparadaParaCadaColuna - visivelLen) / 2;
            espacosParaCentralizarAArte += offset; // Aplica o deslocamento horizontal na margem da arte
            if (espacosParaCentralizarAArte < 0) espacosParaCentralizarAArte = 0; // Evita crashes com margens negativas
            std::string espacos(espacosParaCentralizarAArte, ' ');
            
            if (inimigoAtual->obterMorteAnimada()) {
                linhaArte = std::string(visivelLen, ' ');
            } else if (isMorte && inimigoAtual == alvoAnimacao) {
                int totalLinhasArte = static_cast<int>(arteDoInimigo.size());
                if (totalLinhasArte <= 0) totalLinhasArte = 1;
                
                if (frameAnimacao >= totalLinhasArte) {
                    linhaArte = std::string(visivelLen, ' ');
                    int maxTextLen = 10; // Tamanho de "DERROTADO!"
                    for (const auto& d : dropsAnimacao) {
                        int compVisual = Aparencia::obterComprimentoVisual(d);
                        if (compVisual > maxTextLen) maxTextLen = compVisual;
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
                            
                            int txtLen = Aparencia::obterComprimentoVisual(innerTxt);
                            if (txtLen > maxTextLen) {
                                innerTxt = Aparencia::removerCoresANSI(innerTxt).substr(0, maxTextLen);
                                txtLen = maxTextLen;
                            }
                            
                            int pEsq = (maxTextLen - txtLen) / 2;
                            int pDir = maxTextLen - txtLen - pEsq;
                            
                            std::string middle = corBorda + "║ " + std::string(pEsq, ' ') + corDrop + innerTxt + corReset + corBorda + std::string(pDir, ' ') + " ║" + corReset;
                            linhaArte = std::string(esp, ' ') + middle + std::string(rem, ' ');
                        }
                    }
                } else {
                    // Efeito de desintegração dithered (virando poeira) + fade de cor na visão top-down (2D)
                    double progresso = std::min(1.0, static_cast<double>(frameAnimacao) / totalLinhasArte);
                    int baseR = 200, baseG = 200, baseB = 200; // Cinza claro solicitado pelo usuário
                    double fade = 1.0 - progresso;
                    int rMod = static_cast<int>(baseR * fade);
                    int gMod = static_cast<int>(baseG * fade);
                    int bMod = static_cast<int>(baseB * fade);
                    
                    auto caracteres = splitUTF8(linhaArte);
                    std::string linhaComEfeito = "";
                    for (size_t k = 0; k < caracteres.size(); ++k) {
                        if (caracteres[k] == " " || caracteres[k] == "") {
                            linhaComEfeito += " ";
                        } else {
                            int hash = (static_cast<int>(k) * 37 + linhaReal * 57) % 100;
                            if (hash < progresso * 100) {
                                if (progresso < 0.8 && (hash % 3 == 0)) {
                                    int dustIntensity = static_cast<int>(100 * (1.0 - progresso));
                                    linhaComEfeito += "\033[38;2;" + std::to_string(dustIntensity) + ";" + std::to_string(dustIntensity) + ";" + std::to_string(dustIntensity) + "m.\033[0m";
                                } else {
                                    linhaComEfeito += " ";
                                }
                            } else {
                                    linhaComEfeito += "\033[38;2;" + std::to_string(rMod) + ";" + std::to_string(gMod) + ";" + std::to_string(bMod) + "m" + caracteres[k] + "\033[0m";
                            }
                        }
                    }
                    linhaArte = linhaComEfeito;
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

                if (inimigoAtual->obterMorteAnimada()) {
                    linhaAtual += linhaArte; // Adiciona espacos vazios
                } else if (isPiscarColorido) {
                    linhaAtual += corDestaque + baseLinha + Aparencia::cor(Cor::RESET);
                } else if (TelaCombate::selecaoAlvoAtual == static_cast<int>(indiceDoInimigoParaDesenhar)) {
                    linhaAtual += (TelaCombate::piscarSelecao ? Aparencia::cor(Cor::AMARELO) : Aparencia::cor(Cor::CINZA)) + baseLinha + Aparencia::cor(Cor::RESET);
                } else if (inimigoAtual == g_inimigoAtacanteParry) {
                    linhaAtual += "\033[38;2;255;140;0m" + baseLinha + Aparencia::cor(Cor::RESET);
                } else {
                    linhaAtual += baseLinha;
                }
            } else {
                if (inimigoAtual->obterMorteAnimada()) {
                    linhaAtual += linhaArte; // Adiciona espacos vazios
                } else if (TelaCombate::selecaoAlvoAtual == static_cast<int>(indiceDoInimigoParaDesenhar)) {
                    linhaAtual += (TelaCombate::piscarSelecao ? Aparencia::cor(Cor::AMARELO) : Aparencia::cor(Cor::CINZA)) + linhaArte + Aparencia::cor(Cor::RESET);
                } else if (inimigoAtual == g_inimigoAtacanteParry) {
                    linhaAtual += "\033[38;2;255;140;0m" + linhaArte + Aparencia::cor(Cor::RESET);
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
    
    auto construirOpcoes = [&]() {
        opcoesMenuAtual.clear();
        if (personagemHUD != nullptr && personagemHUD != personagemAgindo) {
            opcoesMenuAtual.push_back("Voltar a Acao");
            opcoesMenuAtual.push_back("Ver Aliados");
        } else {
            opcoesMenuAtual.push_back("Atacar");
            opcoesMenuAtual.push_back("Defender");
            
            std::string habOption = "Habilidade";
            if (personagemAgindo->obterTipoClasse() == TipoClasse::NECROMANTE) {
                size_t almas = personagemAgindo->obterNumeroDeAlmas();
                if (almas == 0) habOption += Aparencia::cor(Cor::VERMELHO) + " (0 Almas)" + Aparencia::cor(Cor::RESET);
                else habOption += " (" + std::to_string(almas) + " Alma" + (almas > 1 ? "s" : "") + ")";
            }
            opcoesMenuAtual.push_back(habOption);
            
            bool temInventario = (personagemAgindo->obterInventario() != nullptr && personagemAgindo->obterInventario()->obterTodosOsItens().size() > 0);
            if (temInventario || personagemAgindo == jogadorAtual) {
                opcoesMenuAtual.push_back("Inventario");
            }
            
            opcoesMenuAtual.push_back("Ficha");
            if (!aliados.empty()) opcoesMenuAtual.push_back("Ver Aliados");
            opcoesMenuAtual.push_back("Diario");
            opcoesMenuAtual.push_back("Log Batalha");
        }
    };

    construirOpcoes();
    auto ultimoUpdate = std::chrono::steady_clock::now();
    atualizarTelaEstatica("", inimigos, jogadorAtual, aliados);

    while (true) {
        auto agora = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(agora - ultimoUpdate).count() >= 150) {
            atualizarTelaEstatica("", inimigos, jogadorAtual, aliados);
            ultimoUpdate = agora;
        }

        if (ControleDeInput::teclaPressionada()) {
            unsigned char tecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            
            if (tecla == 224 || tecla == 0) {
                unsigned char proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
                if (proxTecla == 72 || proxTecla == 'A') tecla = 'w';
                else if (proxTecla == 80 || proxTecla == 'B') tecla = 's';
            } else if (tecla == '\033') {
                if (ControleDeInput::teclaPressionada()) {
                    unsigned char proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
                    if (proxTecla == '[') proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
                    if (proxTecla == 72 || proxTecla == 'A') tecla = 'w';
                    else if (proxTecla == 80 || proxTecla == 'B') tecla = 's';
                    else if (proxTecla == 27) tecla = '\033';
                }
            }

            int totalOpcoes = static_cast<int>(opcoesMenuAtual.size());

            if (tecla == 'w' || tecla == 'W') { 
                selecaoAcaoAtual--; 
                if (selecaoAcaoAtual < 0) selecaoAcaoAtual = totalOpcoes - 1; 
                ultimoUpdate = std::chrono::steady_clock::now();
                atualizarTelaEstatica("", inimigos, jogadorAtual, aliados);
            }
            else if (tecla == 's' || tecla == 'S') { 
                selecaoAcaoAtual++; 
                if (selecaoAcaoAtual >= totalOpcoes) selecaoAcaoAtual = 0; 
                ultimoUpdate = std::chrono::steady_clock::now();
                atualizarTelaEstatica("", inimigos, jogadorAtual, aliados);
            }
            else if (tecla == '\r' || tecla == '\n') { 
                std::string op = opcoesMenuAtual[selecaoAcaoAtual];
                if (op == "Ver Aliados") {
                    selecionarHUDDeAliado(jogadorAtual, aliados);
                    selecaoAcaoAtual = 0; 
                    construirOpcoes();
                    atualizarTelaEstatica("", inimigos, jogadorAtual, aliados);
                    continue;
                }
                if (op == "Voltar a Acao") {
                    personagemHUD = personagemAgindo;
                    selecaoAcaoAtual = 0;
                    construirOpcoes();
                    atualizarTelaEstatica("", inimigos, jogadorAtual, aliados);
                    continue;
                }

                selecaoAcaoAtual = -1; // Remove cursor para as animacoes manterem a tela limpa
                personagemHUD = nullptr; // Garante que o HUD volte a mostrar o personagem agindo de fato nas proximas animacoes
                atualizarTelaEstatica("", inimigos, jogadorAtual, aliados);
                
                if (op == "Atacar") return 1;
                if (op == "Defender") return 2;
                if (op.find("Habilidade") != std::string::npos) return 3;
                if (op == "Inventario") return 4;
                if (op == "Ficha") return 5;
                if (op == "Diario") return 6;
                if (op == "Log Batalha") return 7;
                return 0;
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
    auto ultimoUpdate = std::chrono::steady_clock::now();

    while (true) {
        auto agora = std::chrono::steady_clock::now();
        
        if (std::chrono::duration_cast<std::chrono::milliseconds>(agora - tempoBlink).count() >= 150) {
            piscarSelecao = !piscarSelecao;
            tempoBlink = agora;
        }

        if (std::chrono::duration_cast<std::chrono::milliseconds>(agora - ultimoUpdate).count() >= 33) {
            ultimoUpdate = agora;
            atualizarTelaEstatica(tituloCombate, inimigos, jogadorAtual, aliados);
        }

        if (ControleDeInput::teclaPressionada()) {
            unsigned char tecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            
            if (tecla == 224 || tecla == 0) {
                unsigned char proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
                if (proxTecla == 75 || proxTecla == 'D') tecla = 'a'; // Seta Esquerda
                else if (proxTecla == 77 || proxTecla == 'C') tecla = 'd'; // Seta Direita
            } else if (tecla == '\033') {
                if (ControleDeInput::teclaPressionada()) {
                    unsigned char proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
                    if (proxTecla == '[') proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
                    if (proxTecla == 75 || proxTecla == 'D') tecla = 'a'; // Seta Esquerda
                    else if (proxTecla == 77 || proxTecla == 'C') tecla = 'd'; // Seta Direita
                    else if (proxTecla == 27) tecla = '\033'; // Esc
                }
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
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
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
    std::cout << "\n" << Aparencia::margemCombate() << FuncoesDialogo::formatarMsgSistema("Uso do item cancelado. Ele retornou para a mochila.") << "\n";
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
    
    g_inimigoMortoComDrops = inimigoMorto;
    g_dropsAtivos = drops;
    
    int totalLinhas = 0;
    int stepAnim = 1;
    int intervaloMs = 12;

    if (isModo3D) {
        totalLinhas = 12; // 12 frames
        intervaloMs = 15; // 15ms por frame. Com o tempo de desenho de tela (~60-80ms), a animação dura cerca de 1,0 segundo no total.
    } else {
        totalLinhas = static_cast<int>(inimigoMorto->obterRaca()->obterAparenciaRaca().size());
        if (totalLinhas <= 0) totalLinhas = 6;
        intervaloMs = 25; // 25ms por frame
    }

    executarAnimacao(totalLinhas, intervaloMs, stepAnim, [&](int frame) {
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
