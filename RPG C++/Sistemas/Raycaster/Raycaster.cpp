#include "Raycaster.h"
#include "../../Core/Utilidades/Aparencia.h"
#include "../../Core/Utilidades/ControleDeInput.h"
#include "../../Interface/Telas/Combate/TelaCombate.h"
#include "../Inventario/InventarioCombate.h"
#include "../../Interface/Telas/Atributos/TelaAtributos.h"
#include "../../Interface/Telas/Diario/TelaDiario.h"
#include "../../Interface/Telas/Pause/TelaPause.h"
#include "RaycasterSprites.h"
#include "RaycasterInimigos.h"
#include "RaycasterNPCs.h"
#include "RaycasterMundo.h"
#include "RaycasterHUD.h"
#include "RaycasterRenderer.h"
#include <map>
#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>

#ifdef _WIN32
    #include <windows.h>
#endif

using namespace std;

char Raycaster::iniciarExploracao3D(const vector<string>& matrizDoMapa, float& jogadorX, float& jogadorY, float& anguloVisao, const string& tituloMapa, Personagem* jogador, int& outHitX, int& outHitY, bool animarEntrada) {
    outHitX = -1;
    outHitY = -1;
    if (matrizDoMapa.empty() || !jogador) return 0;

    int larguraMapa = matrizDoMapa[0].size();
    int alturaMapa = matrizDoMapa.size();

    bool temaFloresta = RaycasterMundo::isTemaFloresta(tituloMapa);
    int temaCeu = RaycasterMundo::obterTemaCeu(tituloMapa);

    // Obtem a resolucao dinamica do terminal para preencher a tela inteira
    int LARGURA_TELA = Aparencia::obterLarguraTerminal();
    int ALTURA_TELA = Aparencia::obterAlturaTerminal();

    // Fallback de seguranca caso nao consiga ler a resolucao
    if (LARGURA_TELA <= 0) LARGURA_TELA = 120;
    if (ALTURA_TELA <= 0) ALTURA_TELA = 40;
    
    map<char, SpriteCache> cacheSprites;
    RaycasterInimigos::inicializarSprites(cacheSprites);
    RaycasterNPCs::inicializarSprites(cacheSprites);

    float profundidadeMaxima = 150.0f;  // Profundidade infinita/maxima do mapa
    float velocidadeMovimento = 5.0f;
    float velocidadeRotacao = 2.0f;

    Aparencia::limparTela();
    cout << "\033[?25l"; // Oculta o cursor piscante

    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();
    auto tempoInicio = chrono::system_clock::now();
    float bobbingTime = 0.0f;
    float bobbingAmplitude = 0.0f;

    vector<string> tela(LARGURA_TELA * ALTURA_TELA, " ");

    auto animarOlho = [&](bool abrindo, const vector<string>& frameBase) {
        int maxPassos = 8; 
        for (int passo = 0; passo <= maxPassos; passo++) {
            int p = abrindo ? passo : (maxPassos - passo);
            float aberturaPercent = (float)p / maxPassos;
            
            string bufferFrame = "\033[?25l\033[H";
            bufferFrame.reserve(LARGURA_TELA * ALTURA_TELA * 15);
            int centroY = ALTURA_TELA / 2;
            int centroX = LARGURA_TELA / 2;
            
            for (int y = 0; y < ALTURA_TELA; y++) {
                for (int x = 0; x < LARGURA_TELA; x++) {
                    if (y == ALTURA_TELA - 1 && x == LARGURA_TELA - 1) break;
                    
                    float dx = std::abs(x - centroX);
                    float rx = dx / (float)centroX; 
                    
                    float heightAtX = -1.0f;
                    if (aberturaPercent > 0.0f) {
                        heightAtX = (ALTURA_TELA * 0.6f * (1.0f - rx*rx) + ALTURA_TELA * 0.5f) * aberturaPercent; 
                    }
                    
                    if (std::abs(y - centroY) > heightAtX) {
                        bufferFrame += "\033[40m \033[0m";
                    } else {
                        bufferFrame += frameBase[y * LARGURA_TELA + x];
                    }
                }
                if (y < ALTURA_TELA - 1) bufferFrame += "\n";
            }
            cout << bufferFrame << flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    };

    RaycasterRenderer::renderizar3D(tela, LARGURA_TELA, ALTURA_TELA, jogadorX, jogadorY, anguloVisao, (ALTURA_TELA / 2.0f), 0, profundidadeMaxima, 0.0f, matrizDoMapa, tituloMapa, temaFloresta, temaCeu, cacheSprites);
    RaycasterHUD::desenhar(tela, LARGURA_TELA, ALTURA_TELA, jogadorX, jogadorY, anguloVisao, matrizDoMapa, tituloMapa, temaFloresta, jogador);
    if (animarEntrada) {
        animarOlho(true, tela);
    }

#ifdef _WIN32
    // Aguarda o jogador soltar a tecla 'V' antes de iniciar o loop para nao fechar no mesmo instante
    while (GetAsyncKeyState('V') & 0x8000) std::this_thread::sleep_for(std::chrono::milliseconds(10));
#endif

    bool primeiraRenderizacao = animarEntrada;
    bool rodando = true;
    while (rodando) {
        tp2 = chrono::system_clock::now();
        chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float tempoDelta = elapsedTime.count();
        
        chrono::duration<float> diffInicio = tp2 - tempoInicio;
        float tempoAbsoluto = diffInicio.count();

        // Limitador de delta para nao "pular" paredes ou quebrar o mapa se a thread travar
        if (tempoDelta > 0.1f) tempoDelta = 0.1f;

        float oldPlayerX = jogadorX;
        float oldPlayerY = jogadorY;
        int oldCellX = (int)jogadorX;
        int oldCellY = (int)jogadorY;
        bool isMoving = false;

#ifdef _WIN32
        // --- CONTROLES ASSINCRONOS (TEMPO REAL) ---
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            TelaPause::exibir(jogador);
            Aparencia::limparTela();
            tp1 = chrono::system_clock::now();
        }

        if (GetAsyncKeyState('V') & 0x8000) {
            rodando = false;
            while (GetAsyncKeyState('V') & 0x8000) std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        if (GetAsyncKeyState('I') & 0x8000) {
            InventarioCombate::gerenciarInventario(jogador);
            Aparencia::limparTela();
            tp1 = chrono::system_clock::now();
        }
        if (GetAsyncKeyState('C') & 0x8000) {
            TelaAtributos::gerenciarFichaDoJogador(jogador);
            Aparencia::limparTela();
            tp1 = chrono::system_clock::now();
        }
        if (GetAsyncKeyState('B') & 0x8000) {
            TelaDiario::exibir(jogador);
            Aparencia::limparTela();
            tp1 = chrono::system_clock::now();
        }
        if (GetAsyncKeyState('M') & 0x8000) {
            while (GetAsyncKeyState('M') & 0x8000) std::this_thread::sleep_for(std::chrono::milliseconds(10));
            ControleDeInput::limparBuffer();
            animarOlho(false, tela);
            Aparencia::limparTela();
            return 'M';
        }

        // Rotacao
        if (GetAsyncKeyState('A') & 0x8000) anguloVisao -= velocidadeRotacao * tempoDelta;
        if (GetAsyncKeyState('D') & 0x8000) anguloVisao += velocidadeRotacao * tempoDelta;

        // Movimento (Com sistema de Sliding - permite deslizar na parede ao andar em diagonal)
        if (GetAsyncKeyState('W') & 0x8000) {
            isMoving = true;
            float novoX = jogadorX + cosf(anguloVisao) * velocidadeMovimento * tempoDelta;
            float novoY = jogadorY + sinf(anguloVisao) * velocidadeMovimento * tempoDelta;
            
            if (novoY >= 0 && novoY < alturaMapa && jogadorX >= 0 && jogadorX < larguraMapa) {
                if (RaycasterMundo::isWalkable((int)jogadorX, (int)novoY, matrizDoMapa)) jogadorY = novoY;
            }
            if (jogadorY >= 0 && jogadorY < alturaMapa && novoX >= 0 && novoX < larguraMapa) {
                if (RaycasterMundo::isWalkable((int)novoX, (int)jogadorY, matrizDoMapa)) jogadorX = novoX;
            }
        }
        if (GetAsyncKeyState('S') & 0x8000) {
            isMoving = true;
            float novoX = jogadorX - cosf(anguloVisao) * velocidadeMovimento * tempoDelta;
            float novoY = jogadorY - sinf(anguloVisao) * velocidadeMovimento * tempoDelta;
            
            if (novoY >= 0 && novoY < alturaMapa && jogadorX >= 0 && jogadorX < larguraMapa) {
                if (RaycasterMundo::isWalkable((int)jogadorX, (int)novoY, matrizDoMapa)) jogadorY = novoY;
            }
            if (jogadorY >= 0 && jogadorY < alturaMapa && novoX >= 0 && novoX < larguraMapa) {
                if (RaycasterMundo::isWalkable((int)novoX, (int)jogadorY, matrizDoMapa)) jogadorX = novoX;
            }
        }

        // Efeito de Head Bobbing (Balanco da Camera)
        if (isMoving) {
            bobbingTime += tempoDelta * 12.0f;
            bobbingAmplitude += tempoDelta * 5.0f; // Aumenta a forca do passo
            if (bobbingAmplitude > 1.0f) bobbingAmplitude = 1.0f;
        } else {
            bobbingAmplitude -= tempoDelta * 5.0f; // Suaviza a parada em 0.2 segundos
            if (bobbingAmplitude < 0.0f) {
                bobbingAmplitude = 0.0f;
                bobbingTime = 0.0f;
            } else {
                bobbingTime += tempoDelta * 12.0f;
            }
        }
        int bobbingOffset = (int)(sinf(bobbingTime) * bobbingAmplitude * (ALTURA_TELA * 0.02f));
        float horizonte = (ALTURA_TELA / 2.0f) + bobbingOffset;

        // Verifica se o jogador pisou em um trigger (Inimigo ou Teleporte) para acionar a transicao de mapa/combate
        int newCellX = (int)jogadorX;
        int newCellY = (int)jogadorY;
        if (newCellX != oldCellX || newCellY != oldCellY) {
            char cell = matrizDoMapa[newCellY][newCellX];
            bool isLabel = RaycasterMundo::isMapLabel(newCellX, newCellY, matrizDoMapa);
            if (!isLabel && (RaycasterMundo::isTeleport(cell) || RaycasterMundo::isEntity(cell))) {
                outHitX = newCellX;
                outHitY = newCellY;
                jogadorX = oldPlayerX; // Retorna para a exata posicao anterior flutuante
                jogadorY = oldPlayerY;
                rodando = false; // Sai do loop 3D e devolve o controle pro mapa top-down processar o evento!
            }
        }
#else
        // Fallback previne loop infinito caso compilado fora do windows
        rodando = false; 
#endif

        // --- RENDERIZACAO RAYCASTING (3D) ---
        RaycasterRenderer::renderizar3D(tela, LARGURA_TELA, ALTURA_TELA, jogadorX, jogadorY, anguloVisao, horizonte, bobbingOffset, profundidadeMaxima, tempoAbsoluto, matrizDoMapa, tituloMapa, temaFloresta, temaCeu, cacheSprites);

        // --- RENDERIZACAO HUD E OVERLAYS (2D) ---
        RaycasterHUD::desenhar(tela, LARGURA_TELA, ALTURA_TELA, jogadorX, jogadorY, anguloVisao, matrizDoMapa, tituloMapa, temaFloresta, jogador);

        // Envia o frame processado para o terminal de uma vez de forma linear (Zero Flickering!)
        string bufferFrame = "\033[?25l\033[H"; 
        bufferFrame.reserve(LARGURA_TELA * ALTURA_TELA * 15); 
        for (int y = 0; y < ALTURA_TELA; y++) {
            for (int x = 0; x < LARGURA_TELA; x++) {
                if (y == ALTURA_TELA - 1 && x == LARGURA_TELA - 1) break; // Pula o ultimo pixel para nao bugar o scroll automatico do terminal
                bufferFrame += tela[y * LARGURA_TELA + x];
            }
            if (y < ALTURA_TELA - 1) bufferFrame += "\n"; // Nao quebra linha na ultima fileira da tela
        }

        cout << bufferFrame << flush; // O flush empurra a string toda pro Windows de uma so vez

        // Frame Pacing dinâmico para cravar ~60 FPS reais
        auto frameEnd = chrono::system_clock::now();
        auto frameDuration = chrono::duration_cast<chrono::milliseconds>(frameEnd - tp2).count();
        int sleepTime = 16 - static_cast<int>(frameDuration);
        if (sleepTime > 0) {
            this_thread::sleep_for(chrono::milliseconds(sleepTime));
        }
    }

    // Ao apertar ESC, o loop morre, limpa o console e o controle volta para o jogo top-down padrao
    ControleDeInput::limparBuffer();
    // So limpa a tela se o fechamento foi manual (ESC). Se bateu em entidade, preserva a visao 3D como fundo pro Popup!
    if (outHitX == -1 && outHitY == -1) {
        animarOlho(false, tela);
        Aparencia::limparTela();
    }
    return 0;
}
