#include "TelaCombateRaycaster.h"
#include "../../EngineRaycaster/Raycaster.h"
#include "../../../TelasBase/Combate/TelaCombate.h"
#include "../../../../Sistemas/Combate/Combate.h"
#include "../../../../Entidades/Personagem.h"
#include "../../../../Entidades/Racas/RacaBase.h"
#include "../../../../Sistemas/Inventario/Item.h"
#include "../../../../Sistemas/Inventario/Inventario.h"
#include "../../../../Core/Utilidades/Aparencia.h"
#include <chrono>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <unordered_map>

struct PlayerHUDStateRaycaster {
    double hpFantasma = -1.0;
    double hpAnterior = -1.0;
};
static std::unordered_map<Personagem*, PlayerHUDStateRaycaster> hudStatesRaycaster;


std::vector<std::string> TelaCombateRaycaster::obterLinhasBarraDeStatusDoJogador(Personagem* jogadorAtual, Cor corDestaque, int danoAnimacao, int frameAnimacao, bool isCura) {
    if (!jogadorAtual) return {};

    auto agora = std::chrono::steady_clock::now();
    (void)agora; // caso precise no futuro

    std::string nomeDaArma = (jogadorAtual->obterArma()) ? jogadorAtual->obterArma()->obterNomeItem() + jogadorAtual->obterArma()->obterInfoStatus() : "Punhos";
    std::string nomeDoEscudo = (jogadorAtual->obterEscudo()) ? jogadorAtual->obterEscudo()->obterNomeItem() + jogadorAtual->obterEscudo()->obterInfoStatus() : "Nenhum";
    std::string nomeDaArmadura = (jogadorAtual->obterArmadura()) ? jogadorAtual->obterArmadura()->obterNomeItem() + jogadorAtual->obterArmadura()->obterInfoStatus() : "Trapos";
    
    Item* consumivel = jogadorAtual->obterConsumivelRapido();
    std::string nomeConsumivel = consumivel ? consumivel->obterNomeItem() + " (" + std::to_string(jogadorAtual->obterInventario()->contarItem(consumivel->obterNomeItem())) + "x)" : "Vazio";

    PlayerHUDStateRaycaster& estadoHUD = hudStatesRaycaster[jogadorAtual];
    double vidaAtual = jogadorAtual->obterVida();
    if (estadoHUD.hpAnterior == -1.0) estadoHUD.hpFantasma = vidaAtual;
    if (vidaAtual < estadoHUD.hpFantasma) {
        estadoHUD.hpFantasma -= std::max(0.5, (estadoHUD.hpFantasma - vidaAtual) * 0.10);
        if (estadoHUD.hpFantasma < vidaAtual) estadoHUD.hpFantasma = vidaAtual;
    } else {
        estadoHUD.hpFantasma = vidaAtual;
    }
    estadoHUD.hpAnterior = vidaAtual;

    double pctVida = vidaAtual / std::max(1, jogadorAtual->obterVidaMaxima());
    double pctFantasma = estadoHUD.hpFantasma / std::max(1, jogadorAtual->obterVidaMaxima());
    
    std::string corVidaBase = (pctVida > 0.70) ? "\033[38;2;100;255;100m" : (pctVida > 0.30) ? "\033[38;2;255;255;100m" : "\033[38;2;255;100;100m";
    std::string corHPFundo = "\033[38;2;60;60;60m";
    std::string corFantasma = "\033[38;2;200;100;100m";

    if (danoAnimacao > 0 && frameAnimacao > 0) {
        if (frameAnimacao % 2 == 1) {
            corVidaBase = isCura ? "\033[1;38;2;150;255;150m" : "\033[1;38;2;255;150;150m";
        }
    }

    std::string barraHP = "[";
    int tamanhoBarra = 5;
    int preenchido = static_cast<int>(pctVida * tamanhoBarra);
    int preenchidoFantasma = static_cast<int>(pctFantasma * tamanhoBarra);
    for (int i = 0; i < tamanhoBarra; ++i) {
        if (i < preenchido) barraHP += corVidaBase + "█";
        else if (i < preenchidoFantasma) barraHP += corFantasma + "█";
        else barraHP += corHPFundo + "░";
    }
    barraHP += "\033[0m]" + corVidaBase + std::to_string(jogadorAtual->obterVida()) + "/" + std::to_string(jogadorAtual->obterVidaMaxima()) + "\033[0m";

    std::string corOuro = "\033[38;2;255;255;255m"; // Branco
    std::string corXP = "\033[38;2;0;200;255m"; // Ciano
    std::string corTexto = "\033[38;2;200;200;200m"; // Cinza claro
    std::string corDestaqueGeral = "\033[1;38;2;255;255;255m";
    std::string corLabel = "\033[38;2;150;150;150m";

    // Formatacao da XP
    int xpAtual = jogadorAtual->obterXpAtual();
    int xpBaseAnterior = 0; // Aproximacao simples, o ideal seria ter xpBaseDoNivel
    int deltaTotalXP = jogadorAtual->obterXpParaSubir() - xpBaseAnterior;
    double pctXp = static_cast<double>(xpAtual - xpBaseAnterior) / std::max(1, deltaTotalXP);
    if (pctXp > 1.0) pctXp = 1.0;
    
    std::string barraXP = "[";
    int tamanhoBarraXP = 5;
    int xpPreenchido = static_cast<int>(pctXp * tamanhoBarraXP);
    for (int i = 0; i < tamanhoBarraXP; ++i) {
        if (i < xpPreenchido) barraXP += corXP + "█";
        else barraXP += corHPFundo + "░";
    }
    barraXP += "\033[0m]";

    std::string prefixHP = corLabel + " | HP: ";
    if (danoAnimacao > 0 && frameAnimacao > 0) {
        if (frameAnimacao % 2 == 1) {
            prefixHP = isCura ? "\033[1;38;2;150;255;150m | HP: " : "\033[1;38;2;255;150;150m | HP: ";
        }
    }

    std::vector<std::string> linhas;
    std::string infoRaca = jogadorAtual->obterRaca() ? jogadorAtual->obterRaca()->obterNomeRaca() : "Desconhecido";
    std::string infoClasse = jogadorAtual->obterNomeClasse();

    std::string linha1 = corDestaqueGeral + " " + jogadorAtual->obterNome() + " (Nv " + std::to_string(jogadorAtual->obterNivel()) + ") - " 
                       + infoRaca + " " + infoClasse + "\033[0m"
                       + prefixHP + barraHP
                       + corLabel + " | XP: " + barraXP
                       + corLabel + " | " + corOuro + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "g\033[0m"
                       + corLabel + " | Pocao: " + corTexto + nomeConsumivel + "\033[0m";
    
    std::string linha2 = corLabel + " Arma: " + corTexto + nomeDaArma + "\033[0m"
                       + corLabel + " | Escudo: " + corTexto + nomeDoEscudo + "\033[0m";

    std::string linha3 = corLabel + " Traje: " + corTexto + nomeDaArmadura + "\033[0m";

    std::vector<EfeitoID> efeitos;
    jogadorAtual->obterIDsEfeitosAtivos(efeitos);
    std::string strStatusHUD = "";
    for (auto id : efeitos) {
        const EfeitoStatus* ef = jogadorAtual->encontrarEfeito(id);
        if (ef) {
            strStatusHUD += "\033[38;2;255;150;50m[" + ef->obterNome() + "]\033[0m ";
        }
    }
    if (!strStatusHUD.empty()) {
        linha3 += corLabel + " | Status: " + strStatusHUD;
    }

    // Retrato / Mugshot
    std::vector<std::string> mugshot = { " /_\\ ", "(o_o)", " \\_/ " }; // Base humana
    if (jogadorAtual->obterRaca()) {
        TipoRaca t = jogadorAtual->obterRaca()->obterTipoRaca();
        if (t == TipoRaca::Elfo) mugshot = { " /\\/\\ ", "(o_o)", " \\__/ " };
        else if (t == TipoRaca::Ork || t == TipoRaca::OrkExilado) mugshot = { " [__] ", "(>_<)", " \\##/ " };
        else if (t == TipoRaca::Dwarf) mugshot = { " _██_ ", "(o_o)", " {##} " };
    }
    
    if (pctVida <= 0.30) { // Hurt mugshot
        mugshot[1] = "(x_x)";
        if (danoAnimacao > 0 && !isCura) mugshot[1] = "(>O<)"; // Tomando porrada
    } else if (pctVida > 0.70 && danoAnimacao > 0 && isCura) { // Curando e feliz
        mugshot[1] = "(^_^)";
    }

    std::string corMugshot = "\033[38;2;255;220;180m";
    if (pctVida <= 0.30) corMugshot = "\033[38;2;255;100;100m"; // Sangrando

    linha1 = corMugshot + mugshot[0] + "\033[0m|" + linha1;
    linha2 = corMugshot + mugshot[1] + "\033[0m|" + linha2;
    linha3 = corMugshot + mugshot[2] + "\033[0m|" + linha3;

    linhas.push_back(linha1);
    linhas.push_back(linha2);
    linhas.push_back(linha3);
    
    return linhas;
}

#include "../../../../Sistemas/Combate/Combate.h"
#include "../../EngineRaycaster/RaycasterRenderizadorCombate.h"
#include "../../EngineRaycaster/RaycasterHUD.h"
#include "../../TelasRaycaster/Menu/TelaParryRaycaster.h"
#include "../../../../Core/Utilidades/ControleDeInput.h"
#include "../../../../Sistemas/Combate/Parry.h"
#include "../../../TelasBase/Menu/TelaBaseMenu.h"
#include <thread>
#include <iostream>

// Estado Estático do Contexto
static bool s_isContexto3D = false;
static std::vector<std::string> s_contextoMapa;
static float s_contextoPosX = 0.0f;
static float s_contextoPosY = 0.0f;
static float s_contextoAngulo = 0.0f;
static std::string s_contextoTituloMapa = "";
static std::string s_tituloTurnoHUD = "";

struct MsgLogRaycaster {
    std::string texto;
    std::chrono::time_point<std::chrono::steady_clock> timestamp;
};
static std::vector<MsgLogRaycaster> logBatalha;

// ---- Início das Funções Implementadas ----

void TelaCombateRaycaster::configurarContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo) {
    s_isContexto3D = modo3D;
    s_contextoMapa = matriz;
    s_contextoPosX = posX;
    s_contextoPosY = posY;
    s_contextoAngulo = angulo;
    s_contextoTituloMapa = titulo;
}

void TelaCombateRaycaster::definirTurnoVisivel(int turno, const std::string& nome) {
    (void)turno;
    std::string corTurno = (nome == "INIMIGOS") ? "\033[1;38;2;255;100;100m" : "\033[1;38;2;100;255;100m";
    s_tituloTurnoHUD = corTurno + "[ TURNO DE " + nome + " ]";
}

void TelaCombateRaycaster::adicionarMensagemFixa(const std::string& msg) {
    MsgLogRaycaster log;
    log.texto = msg;
    log.timestamp = std::chrono::steady_clock::now();
    logBatalha.push_back(log);
    if (logBatalha.size() > 5) {
        logBatalha.erase(logBatalha.begin());
    }
}

void TelaCombateRaycaster::limparMensagensFixas() {
    logBatalha.clear();
}

void TelaCombateRaycaster::atualizarTelaEstatica(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, bool animarEntrada, std::function<void(std::vector<std::string>&)> callbackOverlay) {
    int larguraTerminal = Aparencia::obterLarguraTerminal();
    int alturaTerminal = Aparencia::obterAlturaTerminal();
    (void)animarEntrada;
    (void)listaDeAliados;
    (void)tituloCombate;
    
    auto agora = std::chrono::steady_clock::now();
    int tempoMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(agora.time_since_epoch()).count());

    std::vector<std::string> tela3D = RaycasterRenderizadorCombate::renderizarQuadro(
        s_contextoTituloMapa, jogadorAtual, listaDeInimigos, 
        nullptr, 0, 0, 0, false, tempoMs, false, {}, 1.0f
    );
    
    // Desenho dos logs de batalha
    std::vector<MsgLogRaycaster> mensagensAtivas;
    for (auto it = logBatalha.begin(); it != logBatalha.end(); ) {
        int elapsedMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(agora - it->timestamp).count());
        if (elapsedMs > 5000) {
            it = logBatalha.erase(it);
        } else {
            mensagensAtivas.push_back(*it);
            ++it;
        }
    }

    if (!mensagensAtivas.empty()) {
        int maxCompVis = 0;
        for (const auto& msg : mensagensAtivas) {
            int compVis = Aparencia::obterComprimentoVisual(msg.texto);
            if (compVis > maxCompVis) maxCompVis = compVis;
        }

        if (maxCompVis > larguraTerminal - 6) maxCompVis = larguraTerminal - 6;

        int boxW = maxCompVis + 4; // 1 space padding each side + borders
        int boxX = larguraTerminal - boxW - 2;
        if (boxX < 0) boxX = 0;
        int boxY = 2; // Canto superior direito

        std::string bg = "\033[48;2;25;25;25m";
        std::string br = "\033[38;2;255;255;255m";
        std::string tracos = "";
        for (int i = 0; i < boxW - 2; ++i) tracos += "═";

        if (boxY >= 0 && boxY < (int)tela3D.size()) {
            tela3D[boxY] = Aparencia::sobreporPainelNaLinhaAnsi(tela3D[boxY], bg + br + "╔" + tracos + "╗\033[0m", boxX);
        }

        for (size_t i = 0; i < mensagensAtivas.size(); ++i) {
            int yAtual = boxY + 1 + i;
            if (yAtual >= 0 && yAtual < (int)tela3D.size()) {
                std::string texto = mensagensAtivas[i].texto;
                int compTexto = Aparencia::obterComprimentoVisual(texto);
                int pad = maxCompVis - compTexto;
                if (pad < 0) pad = 0;
                std::string linhaStr = bg + br + "║ " + bg + texto + "\033[0m" + bg + std::string(pad, ' ') + br + " ║\033[0m";
                tela3D[yAtual] = Aparencia::sobreporPainelNaLinhaAnsi(tela3D[yAtual], linhaStr, boxX);
            }
        }

        int yFim = boxY + 1 + mensagensAtivas.size();
        if (yFim >= 0 && yFim < (int)tela3D.size()) {
            tela3D[yFim] = Aparencia::sobreporPainelNaLinhaAnsi(tela3D[yFim], bg + br + "╚" + tracos + "╝\033[0m", boxX);
        }
    }

    // Minigame de Parry Popup Overwrite
    if (!Parry::minigameBar.empty() || !Parry::minigameMessage.empty()) {
        int compBar = Aparencia::obterComprimentoVisual(Parry::minigameBar);
        int compMsg = Aparencia::obterComprimentoVisual(Parry::minigameMessage);
        int boxW = std::max(compBar, compMsg) + 10;
        if (boxW > larguraTerminal - 2) boxW = larguraTerminal - 2;
        int boxH = 7;
        int boxX = (larguraTerminal - boxW) / 2;
        if (boxX < 0) boxX = 0;
        int boxY = (tela3D.size() - boxH) / 2;
        
        std::string bg = "\033[48;2;25;25;25m";
        std::string br = "\033[38;2;255;255;255m";
        std::string tracos = "";
        for (int i = 0; i < boxW - 2; ++i) tracos += "═";
        
        if (boxY >= 0 && boxY < (int)tela3D.size()) {
            tela3D[boxY] = Aparencia::sobreporPainelNaLinhaAnsi(tela3D[boxY], bg + br + "╔" + tracos + "╗\033[0m", boxX);
        }
        
        for (int row = 1; row < boxH - 1; ++row) {
            int ty = boxY + row;
            if (ty >= 0 && ty < (int)tela3D.size()) {
                std::string emptyLine = std::string(boxW - 2, ' ');
                tela3D[ty] = Aparencia::sobreporPainelNaLinhaAnsi(tela3D[ty], bg + br + "║" + bg + emptyLine + br + "║\033[0m", boxX);
            }
        }
        
        if (boxY + boxH - 1 >= 0 && boxY + boxH - 1 < (int)tela3D.size()) {
            tela3D[boxY + boxH - 1] = Aparencia::sobreporPainelNaLinhaAnsi(tela3D[boxY + boxH - 1], bg + br + "╚" + tracos + "╝\033[0m", boxX);
        }
        
        if (!Parry::minigameMessage.empty()) {
            RaycasterRenderizadorCombate::pintarTextoNoBuffer(tela3D, larguraTerminal, tela3D.size(), boxX + (boxW - compMsg)/2, boxY + 2, Parry::minigameMessage, "\033[38;2;255;255;255m", "\033[48;2;25;25;25m");
        }
        if (!Parry::minigameBar.empty()) {
            RaycasterRenderizadorCombate::pintarTextoNoBuffer(tela3D, larguraTerminal, tela3D.size(), boxX + (boxW - compBar)/2, boxY + 4, Parry::minigameBar, "\033[38;2;255;255;255m", "\033[48;2;25;25;25m");
        }
    }

    // Desenha o HUD
    if (jogadorAtual) {
        RaycasterHUD::desenharBarraStatus(tela3D, larguraTerminal, alturaTerminal, jogadorAtual, s_contextoAngulo, s_tituloTurnoHUD);
    }

    if (callbackOverlay) {
        callbackOverlay(tela3D);
    }

    // Joga na tela
    std::string out = "\033[?25l\033[H";
    for (size_t i = 0; i < tela3D.size(); ++i) {
        out += tela3D[i];
        if (i < tela3D.size() - 1) out += "\n";
    }
    Raycaster::s_ultimoQuadroRenderizado = out;
    std::cout << out << std::flush;
}

void TelaCombateRaycaster::exibirHordaDeInimigosLadoALado(const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, int frameAnimacao, bool isCura, bool animarSurgimento, bool isMorte, Item* armaAtacante, int danoAnimacao, const std::vector<std::string>& dropsAnimacao) {
    int larguraTerminal = Aparencia::obterLarguraTerminal();
    int alturaTerminal = Aparencia::obterAlturaTerminal();
    (void)animarSurgimento;
    (void)armaAtacante;
    
    auto agora = std::chrono::steady_clock::now();
    int tempoMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(agora.time_since_epoch()).count());

    std::vector<std::string> tela3D = RaycasterRenderizadorCombate::renderizarQuadro(
        s_contextoTituloMapa, nullptr, listaDeInimigos, 
        alvoAnimacao, frameAnimacao, 0, danoAnimacao, isCura, tempoMs, isMorte, dropsAnimacao, 1.0f
    );

    // Joga na tela
    std::string out = "\033[?25l\033[H";
    for (size_t i = 0; i < tela3D.size(); ++i) {
        out += tela3D[i];
        if (i < tela3D.size() - 1) out += "\n";
    }
    Raycaster::s_ultimoQuadroRenderizado = out;
    std::cout << out << std::flush;
}

static void rodarLoopAnimacao(int framesTotais, int intervaloMs, int step, const std::function<void(int)>& renderFrame) {
    for (int frame = 1; frame <= framesTotais; frame += step) {
        renderFrame(frame);
        std::this_thread::sleep_for(std::chrono::milliseconds(intervaloMs));
    }
}

void TelaCombateRaycaster::animarIntroducaoCombate(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* jogadorAtual) {
    (void)tituloCombate;
    (void)listaDeInimigos;
    (void)jogadorAtual;
    // O usuario solicitou a remocao da animacao de introducao do combate
}

void TelaCombateRaycaster::exibirLogoParaTelaDeCombate(const std::string& tituloDaTela, bool animar) {
    (void)tituloDaTela;
    (void)animar;
}

void TelaCombateRaycaster::animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* atacante, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int danoAnimacao) {
    (void)tituloCombate;
    (void)listaDeAliados;
    rodarLoopAnimacao(10, 50, 1, [&](int frame) {
        auto agora = std::chrono::steady_clock::now();
        int tempoMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(agora.time_since_epoch()).count());
        std::vector<std::string> tela = RaycasterRenderizadorCombate::renderizarQuadro(s_contextoTituloMapa, jogadorAtual, listaDeInimigos, alvoAnimacao, frame, 0, danoAnimacao, false, tempoMs, false, {}, 1.0f);
        if (jogadorAtual) RaycasterHUD::desenharBarraStatus(tela, Aparencia::obterLarguraTerminal(), Aparencia::obterAlturaTerminal(), jogadorAtual, s_contextoAngulo, s_tituloTurnoHUD);
        
        std::string out = "\033[?25l\033[H";
        for (size_t i = 0; i < tela.size(); ++i) {
            out += tela[i];
            if (i < tela.size() - 1) out += "\n";
        }
        Raycaster::s_ultimoQuadroRenderizado = out;
        std::cout << out << std::flush;
    });
}

void TelaCombateRaycaster::animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int curaAnimacao) {
    (void)tituloCombate;
    (void)listaDeAliados;
    if (alvoAnimacao && curaAnimacao > 0) {
        adicionarMensagemFixa(Aparencia::cor(Cor::VERDE) + alvoAnimacao->obterNome() + " curou " + std::to_string(curaAnimacao) + " HP!" + Aparencia::cor(Cor::RESET));
    }
    rodarLoopAnimacao(10, 50, 1, [&](int frame) {
        auto agora = std::chrono::steady_clock::now();
        int tempoMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(agora.time_since_epoch()).count());
        std::vector<std::string> tela = RaycasterRenderizadorCombate::renderizarQuadro(s_contextoTituloMapa, jogadorAtual, listaDeInimigos, alvoAnimacao, frame, 0, curaAnimacao, true, tempoMs, false, {}, 1.0f);
        if (jogadorAtual) RaycasterHUD::desenharBarraStatus(tela, Aparencia::obterLarguraTerminal(), Aparencia::obterAlturaTerminal(), jogadorAtual, s_contextoAngulo, s_tituloTurnoHUD);
        
        std::string out = "\033[?25l\033[H";
        for (size_t i = 0; i < tela.size(); ++i) {
            out += tela[i];
            if (i < tela.size() - 1) out += "\n";
        }
        Raycaster::s_ultimoQuadroRenderizado = out;
        std::cout << out << std::flush;
    });
}

extern Personagem* g_inimigoAtacanteParry;

void TelaCombateRaycaster::animarDanoNoJogador(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, bool isParry, int danoAnimacao) {
    (void)tituloCombate;
    (void)listaDeAliados;
    if (alvoAnimacao && danoAnimacao >= 0 && !isParry) {
        std::string atacante = g_inimigoAtacanteParry ? g_inimigoAtacanteParry->obterNome() : "Inimigo";
        adicionarMensagemFixa(Aparencia::cor(Cor::LARANJA) + atacante + " causou " + std::to_string(danoAnimacao) + " dano no " + alvoAnimacao->obterNome() + "!" + Aparencia::cor(Cor::RESET));
    }
    rodarLoopAnimacao(6, 60, 1, [&](int frame) {
        int largura = Aparencia::obterLarguraTerminal();
        int altura = Aparencia::obterAlturaTerminal();
        auto agora = std::chrono::steady_clock::now();
        int tempoMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(agora.time_since_epoch()).count());
        std::vector<std::string> tela3D = RaycasterRenderizadorCombate::renderizarQuadro(s_contextoTituloMapa, jogadorAtual, listaDeInimigos, nullptr, 0, frame, 0, false, tempoMs, false, {}, 1.0f);
        
        RaycasterHUD::desenharBarraStatus(tela3D, largura, altura, jogadorAtual, s_contextoAngulo, s_tituloTurnoHUD, frame, danoAnimacao);
        
        std::string out = "\033[?25l\033[H";
        for (size_t i = 0; i < tela3D.size(); ++i) {
            out += tela3D[i];
            if (i < tela3D.size() - 1) out += "\n";
        }
        Raycaster::s_ultimoQuadroRenderizado = out;
        std::cout << out << std::flush;
    });
}

void TelaCombateRaycaster::animarCuraNoJogador(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int curaAnimacao) {
    (void)tituloCombate;
    (void)listaDeAliados;
    if (alvoAnimacao && curaAnimacao > 0) {
        adicionarMensagemFixa(Aparencia::cor(Cor::VERDE) + alvoAnimacao->obterNome() + " curou " + std::to_string(curaAnimacao) + " HP!" + Aparencia::cor(Cor::RESET));
    }
    rodarLoopAnimacao(6, 60, 1, [&](int frame) {
        int largura = Aparencia::obterLarguraTerminal();
        int altura = Aparencia::obterAlturaTerminal();
        auto agora = std::chrono::steady_clock::now();
        int tempoMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(agora.time_since_epoch()).count());
        std::vector<std::string> tela3D = RaycasterRenderizadorCombate::renderizarQuadro(s_contextoTituloMapa, jogadorAtual, listaDeInimigos, nullptr, 0, 0, 0, false, tempoMs, false, {}, 1.0f);
        
        RaycasterHUD::desenharBarraStatus(tela3D, largura, altura, jogadorAtual, s_contextoAngulo, s_tituloTurnoHUD, frame, curaAnimacao, true);
        
        std::string out = "\033[?25l\033[H";
        for (size_t i = 0; i < tela3D.size(); ++i) {
            out += tela3D[i];
            if (i < tela3D.size() - 1) out += "\n";
        }
        Raycaster::s_ultimoQuadroRenderizado = out;
        std::cout << out << std::flush;
    });
}
void TelaCombateRaycaster::animarMorteInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* inimigoMorto, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, const std::vector<std::string>& drops) {
    (void)tituloCombate;
    (void)listaDeAliados;
    rodarLoopAnimacao(10, 60, 1, [&](int frame) {
        int largura = Aparencia::obterLarguraTerminal();
        int altura = Aparencia::obterAlturaTerminal();
        auto agora = std::chrono::steady_clock::now();
        int tempoMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(agora.time_since_epoch()).count());
        std::vector<std::string> tela3D = RaycasterRenderizadorCombate::renderizarQuadro(s_contextoTituloMapa, jogadorAtual, listaDeInimigos, inimigoMorto, frame, 0, 0, false, tempoMs, true, drops, 1.0f);
        RaycasterHUD::desenharBarraStatus(tela3D, largura, altura, jogadorAtual, s_contextoAngulo, s_tituloTurnoHUD);
        std::string out = "\033[?25l\033[H";
        for (size_t i = 0; i < tela3D.size(); ++i) {
            out += tela3D[i];
            if (i < tela3D.size() - 1) out += "\n";
        }
        Raycaster::s_ultimoQuadroRenderizado = out;
        std::cout << out << std::flush;
    });

    // Drops removidos daqui conforme solicitado; eles já aparecem no log.
}

// Menus de Input
int TelaCombateRaycaster::obterAcaoDoJogador(int turnoAtual, Personagem* personagemAgindo, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) {
    (void)turnoAtual;
    (void)personagemAgindo;
    ControleDeInput::limparBuffer();
    TelaCombate::contexto.selecaoAlvoAtual = -1; // Garante que nenhum alvo esta selecionado ao escolher acao
    std::vector<std::string> acoes = {"Atacar", "Defender", "Itens", "Fugir"};
    int selecionado = 0;
    while(true) {
        int largura = Aparencia::obterLarguraTerminal();
        int altura = Aparencia::obterAlturaTerminal();
        
        int boxY = altura - 10; 
        
        std::string bg = "\033[48;2;25;25;25m";
        std::string linhaAcoes = "";
        for(size_t i=0; i<acoes.size(); i++) {
            if (i == (size_t)selecionado) {
                linhaAcoes += bg + "\033[38;2;255;255;0m> " + acoes[i] + " <\033[0m";
            } else {
                linhaAcoes += bg + "\033[38;2;150;150;150m  " + acoes[i] + "  \033[0m";
            }
            if (i < acoes.size() - 1) linhaAcoes += bg + "   \033[0m";
        }
        
        int compVisual = 0;
        for (auto& a : acoes) compVisual += a.length() + 4;
        compVisual += (acoes.size() - 1) * 3;
        
        int boxW = compVisual + 4;
        int boxX = (largura - boxW) / 2;
        
        auto overlayMenu = [&](std::vector<std::string>& tela) {
            std::string br = "\033[38;2;255;255;255m";
            std::string tracos = "";
            for (int i = 0; i < boxW - 2; ++i) tracos += "═";
            
            if (boxY >= 0 && boxY < (int)tela.size()) {
                tela[boxY] = Aparencia::sobreporPainelNaLinhaAnsi(tela[boxY], bg + br + "╔" + tracos + "╗\033[0m", boxX);
            }
            if (boxY+1 >= 0 && boxY+1 < (int)tela.size()) {
                tela[boxY+1] = Aparencia::sobreporPainelNaLinhaAnsi(tela[boxY+1], bg + br + "║ " + linhaAcoes + bg + br + " ║\033[0m", boxX);
            }
            if (boxY+2 >= 0 && boxY+2 < (int)tela.size()) {
                tela[boxY+2] = Aparencia::sobreporPainelNaLinhaAnsi(tela[boxY+2], bg + br + "╚" + tracos + "╝\033[0m", boxX);
            }
        };

        atualizarTelaEstatica("", inimigos, jogadorAtual, aliados, false, overlayMenu);
        
        while(!ControleDeInput::teclaPressionada()) { 
            std::this_thread::sleep_for(std::chrono::milliseconds(30)); 
            atualizarTelaEstatica("", inimigos, jogadorAtual, aliados, false, overlayMenu);
        }
        char c = ControleDeInput::lerTecla();
        if(c == 'a' || c == 'A' || c == 75) selecionado = (selecionado - 1 + acoes.size()) % acoes.size();
        if(c == 'd' || c == 'D' || c == 77) selecionado = (selecionado + 1) % acoes.size();
        if(c == '\r' || c == '\n') {
            if (selecionado == 0) return 1; // Atacar
            if (selecionado == 1) return 2; // Defender
            if (selecionado == 2) return 4; // Inventario
            if (selecionado == 3) return 6; // Fugir
        }
    }
}

int TelaCombateRaycaster::obterAlvoAtaque(const std::string& tituloCombate, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) {
    ControleDeInput::limparBuffer();
    int selecionado = 0;
    while(true) {
        TelaCombate::contexto.selecaoAlvoAtual = selecionado;
        atualizarTelaEstatica(tituloCombate, inimigos, jogadorAtual, aliados, false);
        
        while(!ControleDeInput::teclaPressionada()) { 
            std::this_thread::sleep_for(std::chrono::milliseconds(30)); 
            atualizarTelaEstatica(tituloCombate, inimigos, jogadorAtual, aliados, false);
        }
        
        char c = ControleDeInput::lerTecla();
        if(c == 'a' || c == 'A') selecionado = (selecionado - 1 + inimigos.size()) % inimigos.size();
        if(c == 'd' || c == 'D') selecionado = (selecionado + 1) % inimigos.size();
        if(c == '\r' || c == '\n') {
            TelaCombate::contexto.selecaoAlvoAtual = -1;
            return selecionado;
        }
        if(c == 27) {
            TelaCombate::contexto.selecaoAlvoAtual = -1;
            return -1;
        }
    }
}

int TelaCombateRaycaster::obterAlvoItem(const std::string& tituloCombate, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) { return -1; }
int TelaCombateRaycaster::obterEscolhaDeEscudo(const std::string& nomePersonagem, const std::vector<Item*>& listaDeEscudos) {
    if (listaDeEscudos.empty()) return 0;
    std::vector<std::string> nomes;
    for (auto* esc : listaDeEscudos) nomes.push_back(esc->obterNomeItem());
    int sel = ControleDeInput::lerSelecaoMenuEmPopup("ESCOLHA DE ESCUDO", {"Qual escudo deseja equipar?"}, nomes, Cor::AMARELO);
    return sel + 1;
}
void TelaCombateRaycaster::selecionarHUDDeAliado(Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) {}
void TelaCombateRaycaster::notificarInimigosMaisAgeis() { adicionarMensagemFixa("Inimigos sao mais ageis e atacam primeiro!"); }
void TelaCombateRaycaster::notificarTurnoExtra(int, int) { adicionarMensagemFixa("Velocidade superior: Turno Extra!"); }
void TelaCombateRaycaster::notificarDesprevencaoInventario() { adicionarMensagemFixa("Sem item rapido equipado!"); }
void TelaCombateRaycaster::notificarSemEscudos(const std::string& nome) { adicionarMensagemFixa(nome + " tentou defender mas nao tem escudos!"); }
void TelaCombateRaycaster::notificarDesequilibrioDefesa(const std::string& nome) { adicionarMensagemFixa(nome + " teve sua defesa quebrada!"); }
void TelaCombateRaycaster::notificarPosturaDefensiva(const std::string& nome, const std::string& escudo) { 
    if (escudo.empty()) adicionarMensagemFixa(nome + " assumiu postura defensiva!");
    else adicionarMensagemFixa(nome + " ergueu o " + escudo + " para defender!");
}
void TelaCombateRaycaster::notificarAcaoInvalida() { adicionarMensagemFixa("Acao Invalida!"); }
void TelaCombateRaycaster::notificarCancelamentoItem() { adicionarMensagemFixa("Uso de item cancelado."); }
void TelaCombateRaycaster::notificarRequisitoNaoAtendido(const std::string& m) { adicionarMensagemFixa(m); }

