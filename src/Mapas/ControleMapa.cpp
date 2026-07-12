#include "ControleMapa.h"
#include "../Sistemas/Inventario/InventarioCombate.h"
#include "../Perspectiva/TelasBase/Atributos/TelaAtributos.h"
#include "../Perspectiva/TelasBase/Diario/TelaDiario.h"
#include "../Perspectiva/TelasBase/Menu/TelaMenu.h"
#include "../Perspectiva/TelasBase/Pause/TelaPause.h"
#include "../Core/Utilidades/Aparencia.h"
#include "../Core/Utilidades/InputDispatcher.h"

#include "../Perspectiva/TelasBase/MapaMundial/TelaMapaMundo.h"
#include "../Sistemas/Combate/Combate.h"
#include "../Sistemas/Combate/CombateRaycasterUIImpl.h"
#include "../Sistemas/Progresso/Progressao.h"
#include "../Core/Controladores/Debug.h"
#include "../Core/Utilidades/ControleDeInput.h"
#include "../Core/Utilidades/GeradorAleatorio.h"
#include "Sistemas/FisicaMapa.h"
#include "../Perspectiva/PerspectivaRaycaster/EngineRaycaster/Raycaster.h"
#include "../Perspectiva/PerspectivaRaycaster/EngineRaycaster/RaycasterMundo.h"
#include "../Perspectiva/GerenciadorPerspectiva.h"
#include "../Sistemas/Minigames/HackConsole.h"
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <sstream>
#include <cmath>

#include "Sistemas/RenderizadorMapa.h"
#include "Sistemas/ControladorInputMapa.h"

namespace {
    std::string extrairCorBaseDoRaycaster(char celula, const std::string& tituloDoMapa, bool isFloresta) {
        // Sampleia a textura no "meio" do bloco (tx=33, ty=33) para evitar as linhas escuras de rejunte/sombra
        std::vector<std::tuple<int, int, int>> luzesVazias;
        Pixel3D px = RaycasterMundo::obterPixelParedeInternal(tituloDoMapa, isFloresta, 0.0f, 10.0f, celula, 33, 0, 64, 33.0f/64.0f, 0.0f, luzesVazias, 0.0f, 0.0f);
        return "\033[38;2;" + std::to_string(px.r) + ";" + std::to_string(px.g) + ";" + std::to_string(px.b) + "m";
    }
}

static bool s_recemTrocouDeMapa = false;
static float s_posCamera3DX = -1.0f;
static float s_posCamera3DY = -1.0f;
static float s_anguloCamera3D = 0.0f;
static std::string s_tituloMapaAtual = "";
static std::vector<std::string> s_matrizDoMapaAtual;

void ControleMapa::sinalizarTrocaDeMapa3D() { s_recemTrocouDeMapa = true; }
bool ControleMapa::isExploracao3DAtiva() { return GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva(); }
float ControleMapa::obterPosCamera3DX() { return s_posCamera3DX; }
float ControleMapa::obterPosCamera3DY() { return s_posCamera3DY; }
float ControleMapa::obterAnguloCamera3D() { return s_anguloCamera3D; }
std::string ControleMapa::obterTituloMapaAtual() { return s_tituloMapaAtual; }
std::vector<std::string> ControleMapa::obterMatrizDoMapaAtual() { return s_matrizDoMapaAtual; }

// processarInputEComandos movido para ControladorInputMapa.cpp

// aplicarLimitesDeMapa foi movido para FisicaMapa
void ControleMapa::processarCombate(
    Personagem* jogadorAtual, std::vector<std::string>& matrizDoMapaAtual, 
    int& posicaoXDoJogador, int& posicaoYDoJogador, bool& exploracaoEstaAtiva,
    const std::string& tituloDoCombate, const std::string& mensagemDeAviso, std::vector<std::unique_ptr<Personagem>> inimigosParaBatalha, 
    int posicaoXAposCombate, int posicaoYAposCombate, int posicaoXInicialDoInimigo, int quantidadeDeCelulasOcupadas, int /*larguraDoTerminal*/, const std::function<void()>& restaurarTela)
{
    Aparencia::iniciarInteracaoPopup();
    std::vector<std::string> texto = { 
        Aparencia::cor(Cor::AMARELO) + "[!] " + mensagemDeAviso + Aparencia::cor(Cor::RESET) 
    };
    std::vector<std::string> opcoesCombate = { "Nao, recuar", "Sim, batalha!" };
    int opcaoEscolhidaPeloJogador = ControleDeInput::lerSelecaoMenuEmPopup(tituloDoCombate, texto, opcoesCombate, Cor::VERMELHO);

    if (opcaoEscolhidaPeloJogador == 1) {
        std::unique_ptr<ICombateUI> ui = nullptr;
        if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
            ui = std::make_unique<CombateRaycasterUIImpl>();
        }
        
        Combate combate(jogadorAtual, std::move(inimigosParaBatalha), std::move(ui));
        if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
            combate.setContexto3D(true, matrizDoMapaAtual, s_posCamera3DX, s_posCamera3DY, s_anguloCamera3D, s_tituloMapaAtual);
        }
        combate.iniciarCombate();

        if (jogadorAtual->obterVida() > 0) {
            for (int i = 0; i < quantidadeDeCelulasOcupadas; ++i) matrizDoMapaAtual[posicaoYAposCombate][posicaoXInicialDoInimigo + i] = '.';
            posicaoXDoJogador = posicaoXAposCombate;
            posicaoYDoJogador = posicaoYAposCombate;
        }
    }

    if (exploracaoEstaAtiva && !GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) restaurarTela();
}

// animarIntroducaoMapa movido para AnimadorMapa.cpp}


// animarFlashbang movido para AnimadorMapa.cpp

// Funções da câmera e renderização abstraídas para RenderizadorMapa.cpp

std::string ControleMapa::formatarCelula(char celula, int x, int y, const std::string& tituloDoMapa, const std::vector<std::string>& matrizDoMapa, bool isMinimapa) {
    thread_local std::string ultimoTitulo = "";
    thread_local std::string tituloUpper = "";
    thread_local bool isReino = false, isInterior = false, isFloresta = false, isVila = false, isSpawn = false;

    if (ultimoTitulo != tituloDoMapa) {
        ultimoTitulo = tituloDoMapa;
        tituloUpper = tituloDoMapa;
        for (char& ch : tituloUpper) ch = std::toupper(static_cast<unsigned char>(ch));
        
        isReino = (tituloUpper.find("Reino") != std::string::npos || tituloUpper.find("REINO") != std::string::npos);
        isInterior = (tituloUpper.find("LABIRINTO") != std::string::npos || tituloUpper.find("CHEFE") != std::string::npos || tituloUpper.find("CORACAO") != std::string::npos || tituloUpper.find("CAVERNA") != std::string::npos);
        isFloresta = (tituloUpper.find("FLORESTA") != std::string::npos);
        isVila = (tituloUpper.find("VILA") != std::string::npos);
        isSpawn = (tituloUpper.find("INICIO") != std::string::npos);
    }
    
    // --- ESTÉTICA ENGINE IDE (VISÃO TERMINAL) ---
    bool isEngineIDE = !isMinimapa && !GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva();
    if (isEngineIDE) {
        std::string npcs = "GOBFPMSTRCH";
        if (npcs.find(celula) == std::string::npos && celula != ' ' && !RaycasterMundo::isMapLabel(x, y, matrizDoMapa)) {
            
            if (celula == '.' && (!isInterior || tituloUpper.find("CHEFE") != std::string::npos || tituloUpper.find("CORACAO") != std::string::npos)) {
                return "\033[38;2;40;40;40m·\033[0m"; // Floor trace for IDE
            }

            const char syntaxChars[] = "{};/*<>&|!=";
            int idx = (x * 7 + y * 13) % (sizeof(syntaxChars) - 1);
            char ideChar = syntaxChars[idx];
            
            // Syntax Colors (VSCode Dark+ Theme)
            const char* colors[] = {
                "\033[38;2;86;156;214m",   // Blue
                "\033[38;2;197;134;192m",  // Purple
                "\033[38;2;220;220;170m",  // Yellow
                "\033[38;2;78;201;176m",   // Teal
                "\033[38;2;214;157;133m"   // Orange
            };
            int cIdx = (x * 3 + y * 11) % 5;
            
            if (isFloresta || celula == '*' || celula == '#') {
                return "\033[38;2;96;139;78m" + std::string(1, ideChar) + "\033[0m"; // Comentario Verde
            }
            if (celula == '~') {
                return "\033[38;2;86;156;214m" + std::string(1, '~') + "\033[0m"; // Agua continua azul mas com string base
            }
            if (celula == '^') {
                return "\033[38;2;197;134;192m" + std::string(1, '^') + "\033[0m"; // Teleporte roxo IDE
            }
            
            return std::string(colors[cIdx]) + std::string(1, ideChar) + "\033[0m";
        }
    }

    // Teleporte
    if (celula == '^') return Aparencia::cor(Cor::NEGRITO, Cor::TELEPORTE) + "^" + Aparencia::cor(Cor::RESET);
    
    // Água
    if (celula == '~') return Aparencia::corRGB(50, 150, 255) + "≈" + Aparencia::cor(Cor::RESET);
    
    // Árvores
    if (celula == '*') {
        bool isTrunk = false;
        if (y > 0 && matrizDoMapa[y-1][x] == '*') {
            int countHorizontal = 0;
            if (x > 0 && matrizDoMapa[y][x-1] == '*') countHorizontal++;
            if (x + 1 < static_cast<int>(matrizDoMapa[y].length()) && matrizDoMapa[y][x+1] == '*') countHorizontal++;
            if (countHorizontal <= 1) isTrunk = true;
        }
        if (isTrunk) return Aparencia::cor(Cor::MADEIRA) + "█" + Aparencia::cor(Cor::RESET);
        return Aparencia::cor(Cor::VERDE) + "▲" + Aparencia::cor(Cor::RESET);
    }
    
    // Verifica se é uma letra de placa de chão (Label) ANTES de checar as entidades
    if (RaycasterMundo::isMapLabel(x, y, matrizDoMapa)) {
        return Aparencia::cor(Cor::CINZA) + std::string(1, celula) + Aparencia::cor(Cor::RESET);
    }
    
    // Entidades
    if (isVila || isSpawn) {
        if (celula == 'G' || celula == 'O') return Aparencia::cor(Cor::NEGRITO, Cor::VERMELHO) + std::string(1, celula) + Aparencia::cor(Cor::RESET); // Inimigos Vermelhos
        if (celula == 'B') return Aparencia::cor(Cor::NEGRITO, Cor::CIANO) + "B" + Aparencia::cor(Cor::RESET); // Bjorn Ciano
        if (celula == 'F' && x > 0 && matrizDoMapa[y][x-1] == '{') return Aparencia::cor(Cor::NEGRITO, Cor::AMARELO) + "F" + Aparencia::cor(Cor::RESET); // Franchesco Amarelo
        if (celula == 'P') return Aparencia::cor(Cor::NEGRITO, Cor::MARROM) + "P" + Aparencia::cor(Cor::RESET); // Placa Marrom
    } else if (isFloresta) {
        if (celula == 'S' && (!isInterior || tituloUpper.find("CHEFE") != std::string::npos)) return Aparencia::cor(Cor::NEGRITO, Cor::VERMELHO) + "S" + Aparencia::cor(Cor::RESET);
        if (celula == 'F' || celula == 'A') return Aparencia::cor(Cor::NEGRITO, Cor::VERMELHO) + std::string(1, celula) + Aparencia::cor(Cor::RESET);
        if (celula == 'M') return Aparencia::cor(Cor::NEGRITO, Cor::MAGENTA) + "M" + Aparencia::cor(Cor::RESET);
        if (celula == 'B') return Aparencia::cor(Cor::NEGRITO, Cor::DOURADO) + "B" + Aparencia::cor(Cor::RESET);
    } else if (isReino) {
        if (celula == 'T') return Aparencia::cor(Cor::NEGRITO, Cor::VERMELHO) + "T" + Aparencia::cor(Cor::RESET);
        if (celula == 'G') return Aparencia::cor(Cor::NEGRITO, Cor::AMARELO) + "G" + Aparencia::cor(Cor::RESET);
        if (celula == 'C') return Aparencia::cor(Cor::NEGRITO, Cor::PRATA) + "C" + Aparencia::cor(Cor::RESET);
    }
    
    if (tituloUpper == "SALA DO CHEFE" && (celula == 'M' || celula == 'A' || celula == 'H' || celula == 'O' || celula == 'R' || celula == 'G')) {
        return Aparencia::cor(Cor::NEGRITO, Cor::BRANCO) + std::string(1, celula) + Aparencia::cor(Cor::RESET);
    }
    
    // Casas e Estruturas
    if (!isInterior && !isReino) {
        std::string corEstrutura = extrairCorBaseDoRaycaster('|', tituloUpper, isFloresta);
        
        if (celula == '_') return corEstrutura + "▄" + Aparencia::cor(Cor::RESET);
        if (celula == '|' || celula == '[' || celula == ']') return corEstrutura + "█" + Aparencia::cor(Cor::RESET);
        std::string estruturas = "{}/\\<>;=-:+";
        if (estruturas.find(celula) != std::string::npos) return corEstrutura + std::string(1, celula) + Aparencia::cor(Cor::RESET);
        
        if (celula == '#') {
            if (isFloresta) return Aparencia::cor(Cor::VERDE) + "█" + Aparencia::cor(Cor::RESET); // Arvores continuam verdes
            std::string corMuro = extrairCorBaseDoRaycaster('#', tituloUpper, isFloresta);
            return corMuro + "█" + Aparencia::cor(Cor::RESET);
        }
    }
    
    // Reino
    if (isReino) {
        if (celula == '|') return Aparencia::cor(Cor::MADEIRA) + "█" + Aparencia::cor(Cor::RESET); // Portao de madeira
        std::string estruturas = "_[]{}/\\<>;=-+#";
        if (estruturas.find(celula) != std::string::npos) {
            std::string corReino = extrairCorBaseDoRaycaster(celula, tituloUpper, isFloresta);
            return corReino + "█" + Aparencia::cor(Cor::RESET);
        }
    }
    
    // Labirinto
    if (isInterior) {
        if (tituloUpper.find("LABIRINTO") != std::string::npos) {
            std::string corLabirinto = extrairCorBaseDoRaycaster('|', tituloUpper, isFloresta);
            auto isHWall = [](char c) { return c == '=' || c == '.' || c == '\''; };
            auto isVWall = [](char c) { return c == '|' || c == '+' || c == 'S' || c == 'E'; };

            if (celula == '=') return corLabirinto + "─" + Aparencia::cor(Cor::RESET);
            if (celula == '|') {
                bool right = (x + 1 < static_cast<int>(matrizDoMapa[y].length()) && isHWall(matrizDoMapa[y][x+1]));
                bool left = (x > 0 && isHWall(matrizDoMapa[y][x-1]));
                if (right && left) return corLabirinto + "┼" + Aparencia::cor(Cor::RESET);
                if (right) return corLabirinto + "├" + Aparencia::cor(Cor::RESET);
                if (left) return corLabirinto + "┤" + Aparencia::cor(Cor::RESET);
                return corLabirinto + "│" + Aparencia::cor(Cor::RESET);
            }
            if (celula == '.') {
                bool right = (x + 1 < static_cast<int>(matrizDoMapa[y].length()) && isHWall(matrizDoMapa[y][x+1]));
                bool left = (x > 0 && isHWall(matrizDoMapa[y][x-1]));
                bool down = (y + 1 < static_cast<int>(matrizDoMapa.size()) && isVWall(matrizDoMapa[y+1][x]));
                
                if (left && right && down) return corLabirinto + "┬" + Aparencia::cor(Cor::RESET);
                if (right && down) return corLabirinto + "┌" + Aparencia::cor(Cor::RESET);
                if (left && down) return corLabirinto + "┐" + Aparencia::cor(Cor::RESET);
                if (left && right) return corLabirinto + "─" + Aparencia::cor(Cor::RESET);
                return corLabirinto + "█" + Aparencia::cor(Cor::RESET);
            }
            if (celula == '\'') {
                bool right = (x + 1 < static_cast<int>(matrizDoMapa[y].length()) && isHWall(matrizDoMapa[y][x+1]));
                bool left = (x > 0 && isHWall(matrizDoMapa[y][x-1]));
                bool up = (y > 0 && isVWall(matrizDoMapa[y-1][x]));
                
                if (left && right && up) return corLabirinto + "┴" + Aparencia::cor(Cor::RESET);
                if (right && up) return corLabirinto + "└" + Aparencia::cor(Cor::RESET);
                if (left && up) return corLabirinto + "┘" + Aparencia::cor(Cor::RESET);
                if (left && right) return corLabirinto + "─" + Aparencia::cor(Cor::RESET);
                return corLabirinto + "█" + Aparencia::cor(Cor::RESET);
            }
            if (celula == '+') {
                bool right = (x + 1 < static_cast<int>(matrizDoMapa[y].length()) && isHWall(matrizDoMapa[y][x+1]));
                bool left = (x > 0 && isHWall(matrizDoMapa[y][x-1]));
                bool down = (y + 1 < static_cast<int>(matrizDoMapa.size()) && isVWall(matrizDoMapa[y+1][x]));
                bool up = (y > 0 && isVWall(matrizDoMapa[y-1][x]));
                
                if (left && right && down && up) return corLabirinto + "┼" + Aparencia::cor(Cor::RESET);
                if (left && right && down) return corLabirinto + "┬" + Aparencia::cor(Cor::RESET);
                if (left && right && up) return corLabirinto + "┴" + Aparencia::cor(Cor::RESET);
                if (up && down && left) return corLabirinto + "┤" + Aparencia::cor(Cor::RESET);
                if (up && down && right) return corLabirinto + "├" + Aparencia::cor(Cor::RESET);
                return corLabirinto + "┼" + Aparencia::cor(Cor::RESET);
            }
        }
        else if (tituloUpper.find("CAVERNA") != std::string::npos) {
            std::string corCaverna = extrairCorBaseDoRaycaster('#', tituloUpper, isFloresta);
            if (celula == '#') return corCaverna + "█" + Aparencia::cor(Cor::RESET);
            if (celula == '.') {
                if (isMinimapa) return "\033[38;2;50;50;50m.\033[0m";
                return "\033[38;2;40;40;40m·\033[0m";
            }
        }
    }
    
    // Chão / Labels
    if (celula == '.' && (!isInterior || tituloUpper.find("CHEFE") != std::string::npos || tituloUpper.find("CORACAO") != std::string::npos)) {
        if (isMinimapa) return "\033[38;2;50;50;50m.\033[0m";
        return "\033[38;2;40;40;40m·\033[0m";
    }
    
    if (std::isalpha(celula) && celula != ' ' && celula != 'S' && celula != 'F' && celula != 'A' && celula != 'M' && celula != 'B' && celula != 'T' && celula != 'G' && celula != 'C') {
        return Aparencia::cor(Cor::CINZA) + std::string(1, celula) + Aparencia::cor(Cor::RESET);
    }
    
    return std::string(1, celula);
}

// renderizarMapa abstraído para RenderizadorMapa.cpp

ProximaTransicaoMapa ControleMapa::executarLoopDeExploracao(
    Personagem* jogadorAtual,
    std::vector<std::string>& matrizDoMapaAtual,
    int& posicaoXDoJogador,
    int& posicaoYDoJogador,
    bool& exploracaoEstaAtiva,
    const std::string& tituloDoMapaAtual,
    const std::function<std::string()>& obterSimbolosInimigos,
    const std::function<std::vector<std::string>()>& obterLayoutOriginal,
    const std::function<void(int, int, int)>& processarInteracao,
    const std::function<std::string(char, int, int)>& formatador,
    const std::function<void()>& restaurarTela,
    int& linhaInicialParaDesenharOMapa,
    bool& precisaRenderizar
) {
    auto ultimoMovimentoInimigos = std::chrono::steady_clock::now();
    ProximaTransicaoMapa destinoViagemRapida = ProximaTransicaoMapa::Nenhuma;
    s_anguloCamera3D = 0.0f;
    s_posCamera3DX = -1.0f;
    s_posCamera3DY = -1.0f;
    s_tituloMapaAtual = tituloDoMapaAtual;
    s_matrizDoMapaAtual = matrizDoMapaAtual;

    while (exploracaoEstaAtiva && jogadorAtual->obterVida() > 0)
    {
        s_tituloMapaAtual = tituloDoMapaAtual;
        
        auto agora = std::chrono::steady_clock::now();
        bool tempoDeMoverInimigos = std::chrono::duration_cast<std::chrono::milliseconds>(agora - ultimoMovimentoInimigos).count() >= 800;

        if (tempoDeMoverInimigos) {
            FisicaMapa::moverInimigosAleatoriamente(matrizDoMapaAtual, obterLayoutOriginal(), obterSimbolosInimigos(), posicaoXDoJogador, posicaoYDoJogador);
            ultimoMovimentoInimigos = std::chrono::steady_clock::now();
            precisaRenderizar = true;
        }

        int larguraDoTerminal = Aparencia::obterLarguraTerminal();
        
        if (precisaRenderizar && !GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
            int alturaDoTerminal = Aparencia::obterAlturaTerminal();

            RenderizadorMapa::renderizarMapa(matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, larguraDoTerminal, alturaDoTerminal, linhaInicialParaDesenharOMapa, formatador);

            precisaRenderizar = false;
        }

        char teclaPressionadaPeloJogador = '\0';
        bool processarInput = false;

            if (!GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva() && ControleDeInput::teclaPressionada()) {
            teclaPressionadaPeloJogador = ControleDeInput::lerTecla();
            processarInput = true;
        }

        if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva() || (processarInput && (teclaPressionadaPeloJogador == 'v' || teclaPressionadaPeloJogador == 'V'))) {
            static std::string tituloAnterior = "";
            int tipoAnimacao = 0;
            
            bool trocandoDePerspectiva = !GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva();
            if (trocandoDePerspectiva) {
                tipoAnimacao = 1;
            }
            if (s_recemTrocouDeMapa && !trocandoDePerspectiva) {
                tipoAnimacao = 2;
            }
            
            if (tituloAnterior != tituloDoMapaAtual) {
                if (tituloAnterior != "" && !trocandoDePerspectiva) {
                    tipoAnimacao = 2;
                }
                tituloAnterior = tituloDoMapaAtual;
            }

            s_recemTrocouDeMapa = false;
            if (!GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
                GerenciadorPerspectiva::obterInstancia().alternarVisao();
            }

                if (s_posCamera3DX == -1.0f || static_cast<int>(s_posCamera3DX) != posicaoXDoJogador || static_cast<int>(s_posCamera3DY) != posicaoYDoJogador) {
                    s_posCamera3DX = static_cast<float>(posicaoXDoJogador) + 0.5f;
                    s_posCamera3DY = static_cast<float>(posicaoYDoJogador) + 0.5f;
                }
                
                int hitX = -1, hitY = -1;
                char acaoPendente = Raycaster::iniciarExploracao3D(matrizDoMapaAtual, s_posCamera3DX, s_posCamera3DY, s_anguloCamera3D, tituloDoMapaAtual, jogadorAtual, hitX, hitY, tipoAnimacao);
                
                posicaoXDoJogador = static_cast<int>(s_posCamera3DX);
                posicaoYDoJogador = static_cast<int>(s_posCamera3DY);
                
                bool isTrigger = false;
                if (hitX != -1 && hitY != -1) {
                    FisicaMapa::aplicarLimitesDeMapa(hitX, hitY, matrizDoMapaAtual);
                    
                    char cell = matrizDoMapaAtual[hitY][hitX];
                    // Verifica se o jogador parou em um trigger (Inimigos ou Teleportes ou Terminal)
                    std::string triggers = "^GOBFSAMTHRPCIQ@";
                    if (triggers.find(cell) != std::string::npos) {
                        isTrigger = true;
                    }
                    
                    int posXantes = posicaoXDoJogador;
                    int posYantes = posicaoYDoJogador;
                    
                    if (cell == '@') {
                        if (HackConsole::iniciarHack(jogadorAtual)) {
                            matrizDoMapaAtual[hitY][hitX] = '.'; // Remove o terminal após hackeado
                            for(int dy = -5; dy <= 5; dy++) {
                                for(int dx = -5; dx <= 5; dx++) {
                                    if(hitY+dy >= 0 && hitY+dy < static_cast<int>(matrizDoMapaAtual.size()) && hitX+dx >= 0 && hitX+dx < static_cast<int>(matrizDoMapaAtual[0].size())) {
                                        if (matrizDoMapaAtual[hitY+dy][hitX+dx] == '!' || matrizDoMapaAtual[hitY+dy][hitX+dx] == '%') {
                                            matrizDoMapaAtual[hitY+dy][hitX+dx] = '.';
                                        }
                                    }
                                }
                            }
                            RaycasterMundo::atualizarMapHash(matrizDoMapaAtual);
                        }
                    } else {
                        processarInteracao(hitX, hitY, larguraDoTerminal); // Aciona o combate/NPC caso o jogador tenha parado em cima de um
                    }
                    
                    // So empurra o jogador para tras se a posicao nao mudou (evita sobrescrever teleportes)
                    if (isTrigger && GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva() && posicaoXDoJogador == posXantes && posicaoYDoJogador == posYantes) {
                        // Empurra o jogador para tras em 1 casa para evitar que ele fique preso no NPC
                        // E previne loops onde ele volta pro jogo ja interagindo
                        s_posCamera3DX = static_cast<float>(hitX) + 0.5f - cos(s_anguloCamera3D) * 1.5f;
                        s_posCamera3DY = static_cast<float>(hitY) + 0.5f - sin(s_anguloCamera3D) * 1.5f;
                        posicaoXDoJogador = static_cast<int>(s_posCamera3DX);
                        posicaoYDoJogador = static_cast<int>(s_posCamera3DY);
                    }
                }
                
                if (acaoPendente == 'M') {
                    teclaPressionadaPeloJogador = 'M';
                    processarInput = true;
                } else if (!isTrigger) {
                    restaurarTela();
                    precisaRenderizar = true;
                    continue;
                } else {
                    continue;
                }
        }

        if (processarInput) {
            if (teclaPressionadaPeloJogador == 'v' || teclaPressionadaPeloJogador == 'V') {
                continue;
            }

            if (teclaPressionadaPeloJogador == 'm' || teclaPressionadaPeloJogador == 'M') {
                LocalizacaoMapa loc = LocalizacaoMapa::VilaInicial;
                std::string tituloUpper = tituloDoMapaAtual;
                std::transform(tituloUpper.begin(), tituloUpper.end(), tituloUpper.begin(), ::toupper);
                
                if (tituloUpper.find("FLORESTA") != std::string::npos || 
                    tituloUpper.find("BOSQUE") != std::string::npos ||
                    tituloUpper.find("LABIRINTO") != std::string::npos ||
                    tituloUpper.find("CHEFE") != std::string::npos ||
                    tituloUpper.find("ARVORE") != std::string::npos) {
                    loc = LocalizacaoMapa::Floresta;
                } else if (tituloUpper.find("Reino") != std::string::npos) {
                    loc = LocalizacaoMapa::Reino;
                } else if (tituloUpper.find("REINO") != std::string::npos) {
                    loc = LocalizacaoMapa::Reino;
                }
                
                int progressoVila = Progressao::instancia().obterProgressoVila(jogadorAtual);
                int progressoFloresta = Progressao::instancia().obterProgressoFloresta(jogadorAtual);
                int progressoPonteReino = Progressao::instancia().obterProgressoPonteReino(jogadorAtual);
                int progressoReino = Progressao::instancia().obterProgressoReino(jogadorAtual);

                ProximaTransicaoMapa destino = TelaMapaMundo::exibir(jogadorAtual, loc, progressoVila, progressoFloresta, progressoPonteReino, progressoReino);

                if (destino != ProximaTransicaoMapa::Nenhuma) {
                    destinoViagemRapida = destino;
                    exploracaoEstaAtiva = false; // Sinaliza para sair do loop e processar a viagem
                    break;
                }
                // Se nenhum destino foi escolhido, apenas restaura a tela e continua a exploração.
                if (!GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
                    restaurarTela();
                    precisaRenderizar = true;
                }
                continue;
            }

            int proximaPosicaoX = posicaoXDoJogador;
            int proximaPosicaoY = posicaoYDoJogador;

            if (ControladorInputMapa::processarInputEComandos(teclaPressionadaPeloJogador, jogadorAtual, proximaPosicaoX, proximaPosicaoY, restaurarTela)) continue;
            
            if (jogadorAtual->obterVoltarProMenu()) break;

            FisicaMapa::aplicarLimitesDeMapa(proximaPosicaoX, proximaPosicaoY, matrizDoMapaAtual);
            processarInteracao(proximaPosicaoX, proximaPosicaoY, larguraDoTerminal);
            
            precisaRenderizar = true;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }

    return destinoViagemRapida;
}
