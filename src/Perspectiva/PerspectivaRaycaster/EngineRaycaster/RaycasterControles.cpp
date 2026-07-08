#include "RaycasterControles.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../TelasBase/Pause/TelaPause.h"
#include "../../../Sistemas/Inventario/InventarioCombate.h"
#include "../../TelasBase/Atributos/TelaAtributos.h"
#include "../../TelasBase/Diario/TelaDiario.h"
#include "../../../Core/Controladores/Debug.h"
#include "RaycasterMundo.h"
#include "RaycasterQuadro.h"
#include <cmath>
#include <thread>

using namespace std;

char RaycasterControles::processarInputEControles(
    Personagem* jogador,
    float& jogadorX,
    float& jogadorY,
    float& anguloVisao,
    float& pitchOffset,
    float tempoDelta,
    float velocidadeMovimento,
    const std::vector<std::string>& matrizDoMapa,
    int ALTURA_TELA,
    float sensibilidadeX,
    float sensibilidadeY,
    bool& primeiraIteracaoMouse,
    int& outHitX,
    int& outHitY,
    bool& rodando,
    std::chrono::steady_clock::time_point& tp1,
#ifdef _WIN32
    MouseHider& mouseHider,
#endif
    bool& isMoving,
    float& bobbingTime,
    float& bobbingAmplitude,
    int& bobbingOffset
) {
    int larguraMapa = matrizDoMapa.empty() ? 0 : matrizDoMapa[0].size();
    int alturaMapa = matrizDoMapa.size();
    
    float oldPlayerX = jogadorX;
    float oldPlayerY = jogadorY;
    int oldCellX = (int)jogadorX;
    int oldCellY = (int)jogadorY;

#ifdef _WIN32
    // --- CONTROLES ASSINCRONOS E MOUSE ---
    HWND hwnd = GetConsoleWindow();
    if (hwnd && GetForegroundWindow() == hwnd) {
        mouseHider.hide(); // Oculta 100% o cursor do mouse
        RECT rect;
        GetWindowRect(hwnd, &rect);
        int centerX = rect.left + (rect.right - rect.left) / 2;
        int centerY = rect.top + (rect.bottom - rect.top) / 2; 
        
        if (primeiraIteracaoMouse) {
            SetCursorPos(centerX, centerY);
            primeiraIteracaoMouse = false;
        } else {
            POINT p;
            if (GetCursorPos(&p)) {
                int deltaX = p.x - centerX;
                int deltaY = p.y - centerY;
                
                if (deltaX != 0 || deltaY != 0) {
                    anguloVisao += deltaX * sensibilidadeX; // Yaw (Esquerda/Direita)
                    pitchOffset -= deltaY * sensibilidadeY;  // Pitch corrigido
                    
                    // Limitar o angulo de olhar para cima/baixo
                    float maxPitch = ALTURA_TELA * 0.7f;
                    if (pitchOffset > maxPitch) pitchOffset = maxPitch;
                    if (pitchOffset < -maxPitch) pitchOffset = -maxPitch;
                    
                    SetCursorPos(centerX, centerY); 
                }
            }
        }
    } else {
        mouseHider.show(); // Mostra se a janela perder o foco
    }

    if (GetAsyncKeyState('V') & 0x8000) {
        mouseHider.show();
        rodando = false;
        while (GetAsyncKeyState('V') & 0x8000) std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    struct PopupEntry {
        int tecla;
        void (*acao)(Personagem*);
    };
    static const PopupEntry popups[] = {
        {VK_ESCAPE, [](Personagem* p) { TelaPause::exibir(p); }},
        {'I',       [](Personagem* p) { InventarioCombate::gerenciarInventario(p); }},
        {'C',       [](Personagem* p) { TelaAtributos::gerenciarFichaDoJogador(p); }},
        {'B',       [](Personagem* p) { TelaDiario::exibir(p); }},
    };
    for (const auto& p : popups) {
        if (GetAsyncKeyState(p.tecla) & 0x8000) {
            primeiraIteracaoMouse = true;
            mouseHider.show();
            ControleDeInput::limparBuffer();
            p.acao(jogador);
            RaycasterQuadro::restaurarUltimoQuadro();
            tp1 = chrono::steady_clock::now();
            break;
        }
    }

    if (GetAsyncKeyState('M') & 0x8000) {
        primeiraIteracaoMouse = true;
        mouseHider.show();
        while (GetAsyncKeyState('M') & 0x8000)
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        ControleDeInput::limparBuffer();
        tp1 = chrono::steady_clock::now();
        return 'M';
    }

    {
        int teclasDebug[] = {VK_OEM_3, VK_OEM_5, VK_OEM_PLUS, VK_OEM_102};
        bool apertouDebug = false;
        for (int td : teclasDebug)
            if (GetAsyncKeyState(td) & 0x8000) { apertouDebug = true; break; }
        if (apertouDebug) {
            primeiraIteracaoMouse = true;
            mouseHider.show();
            for (int td : teclasDebug)
                while (GetAsyncKeyState(td) & 0x8000)
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
            ControleDeInput::limparBuffer();
            Debug::exibirMenuDebug(jogador);
            RaycasterQuadro::restaurarUltimoQuadro();
            tp1 = chrono::steady_clock::now();
        }
    }

    // Movimento e Strafing (Com sistema de Sliding)
    float moveX = 0.0f;
    float moveY = 0.0f;

    if (GetAsyncKeyState('W') & 0x8000) {
        isMoving = true;
        moveX += cosf(anguloVisao) * velocidadeMovimento * tempoDelta;
        moveY += sinf(anguloVisao) * velocidadeMovimento * tempoDelta;
    }
    if (GetAsyncKeyState('S') & 0x8000) {
        isMoving = true;
        moveX -= cosf(anguloVisao) * velocidadeMovimento * tempoDelta;
        moveY -= sinf(anguloVisao) * velocidadeMovimento * tempoDelta;
    }
    if (GetAsyncKeyState('A') & 0x8000) { // Strafe Esquerda (-90 graus)
        isMoving = true;
        moveX += cosf(anguloVisao - 1.5708f) * velocidadeMovimento * tempoDelta;
        moveY += sinf(anguloVisao - 1.5708f) * velocidadeMovimento * tempoDelta;
    }
    if (GetAsyncKeyState('D') & 0x8000) { // Strafe Direita (+90 graus)
        isMoving = true;
        moveX += cosf(anguloVisao + 1.5708f) * velocidadeMovimento * tempoDelta;
        moveY += sinf(anguloVisao + 1.5708f) * velocidadeMovimento * tempoDelta;
    }

    if (isMoving) {
        float novoX = jogadorX + moveX;
        float novoY = jogadorY + moveY;
        
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
    bobbingOffset = (int)(sinf(bobbingTime) * bobbingAmplitude * (ALTURA_TELA * 0.02f));

    // Verifica se o jogador pisou em um trigger (Inimigo ou Teleporte) para acionar a transicao de mapa/combate
    int newCellX = (int)jogadorX;
    int newCellY = (int)jogadorY;
    if (newCellX != oldCellX || newCellY != oldCellY) {
        char cell = matrizDoMapa[newCellY][newCellX];
        bool isLabel = RaycasterMundo::isMapLabel(newCellX, newCellY, matrizDoMapa);
        if (RaycasterMundo::isTeleport(cell) || (!isLabel && RaycasterMundo::isEntity(cell))) {
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

    return '\0';
}
