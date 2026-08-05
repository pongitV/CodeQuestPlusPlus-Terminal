#pragma once
#include <vector>
#include <string>
#include <chrono>
#include "Domain/Characters/Classes/BaseClass.h"

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
    void concert() {
        if (isHidden) {
            SystemParametersInfoA(SPI_SETCURSORS, 0, NULL, 0); // Restaura cursores padrao do sistema
            isHidden = false;
        }
    }
    ~MouseHider() {
        concert();
    }
};
#endif

class RaycasterControls {
public:
    static char processInputEControls(
        Character* player,
        float& playerX,
        float& playerY,
        float& angleVisa,
        float& pitchOffset,
        float timeDelta,
        float speedMovement,
        const std::vector<std::string>& mapMatrix,
        int SCREEN_HEIGHT,
        float sensitivityX,
        float sensitivityY,
        bool& firstIterationMouse,
        int& outHitX,
        int& outHitY,
        bool& running,
        std::chrono::steady_clock::time_point& tp1,
#ifdef _WIN32
        MouseHider& mouseHider,
#endif
        bool& isMoving,
        float& bobbingTeam,
        float& bobbingAmplitude,
        int& bobbingOffset
    );
};
