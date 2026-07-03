#pragma once
#include <vector>
#include <string>
#include <chrono>
#include "../../../Entidades/Classes/ClasseBase.h"

#ifdef _WIN32
#include <windows.h>

struct MouseHider {
    bool isHidden;
    MouseHider() : isHidden(false) {}
    void hide() {
        if (!isHidden) {
            BYTE ANDmaskCursor[] = { 0xFF };
            BYTE XORmaskCursor[] = { 0x00 };
            HCURSOR hCursor1 = CreateCursor(NULL, 0, 0, 1, 1, ANDmaskCursor, XORmaskCursor);
            SetSystemCursor(hCursor1, 32512); // OCR_NORMAL
            HCURSOR hCursor2 = CreateCursor(NULL, 0, 0, 1, 1, ANDmaskCursor, XORmaskCursor);
            SetSystemCursor(hCursor2, 32513); // OCR_IBEAM
            isHidden = true;
        }
    }
    void show() {
        if (isHidden) {
            SystemParametersInfoA(SPI_SETCURSORS, 0, NULL, 0); // Restaura cursores padrao do sistema
            isHidden = false;
        }
    }
    ~MouseHider() {
        show();
    }
};
#endif

class RaycasterControles {
public:
    static char processarInputEControles(
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
    );
};
