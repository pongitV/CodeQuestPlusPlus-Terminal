#include "UI/Renderers/3D/EngineRaycaster/RaycasterControls.h"
#include "Core/Utils/InputControl.h"
#include "Core/Utils/Appearance.h"
#include "UI/Screens/Pause/PauseScreen.h"
#include "Systems/Inventory/CombatInventory.h"
#include "UI/Screens/Attributes/AttributesScreen.h"
#include "UI/Screens/Diary/DiaryScreen.h"
#include "Core/Engine/Debug.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterWorld.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterFrame.h"
#include <cmath>
#include <thread>

using namespace std;

char RaycasterControls::processInputEControls(
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
) {
    int widthMap = mapMatrix.empty() ? 0 : mapMatrix[0].size();
    int heightMap = mapMatrix.size();
    
    float oldPlayerX = playerX;
    float oldPlayerY = playerY;
    int oldCellX = (int)playerX;
    int oldCellY = (int)playerY;

#ifdef _WIN32
    // --- CONTROLES ASSINCRONOS E MOUSE ---
    HWND hwnd = GetConsoleWindow();
    if (hwnd && GetForegroundWindow() == hwnd) {
        mouseHider.hide(); // Oculta 100% o cursor do mouse
        RECT rect;
        GetWindowRect(hwnd, &rect);
        int centerX = rect.left + (rect.right - rect.left) / 2;
        int centerY = rect.top + (rect.bottom - rect.top) / 2; 
        
        if (firstIterationMouse) {
            SetCursorPos(centerX, centerY);
            firstIterationMouse = false;
        } else {
            POINT p;
            if (GetCursorPos(&p)) {
                int deltaX = p.x - centerX;
                int deltaY = p.y - centerY;
                
                if (deltaX != 0 || deltaY != 0) {
                    angleVisa += deltaX * sensitivityX; // Yaw (Esquerda/Direita)
                    pitchOffset -= deltaY * sensitivityY;  // Pitch corrigido
                    
                    // Limitar o angulo de olhar para cima/baixo
                    float maxPitch = SCREEN_HEIGHT * 0.7f;
                    if (pitchOffset > maxPitch) pitchOffset = maxPitch;
                    if (pitchOffset < -maxPitch) pitchOffset = -maxPitch;
                    
                    SetCursorPos(centerX, centerY); 
                }
            }
        }
    } else {
        mouseHider.concert(); // Mostra se a janela perder o foco
        
        // Auto-pause ao perder o foco (Alt-Tab)
        firstIterationMouse = true;
        InputControl::clearBuffer();
        PauseScreen::display(player);
        RaycasterFrame::restoreLastFrame();
        tp1 = chrono::steady_clock::now();
    }

    if (GetAsyncKeyState('V') & 0x8000) {
        mouseHider.concert();
        InputControl::clearBuffer();
        Appearance::displayPopup(
            "PERSPECTIVA IDE",
            {"A perspectiva IDE esta em construcao!",
             "",
             "Em breve voce podera explorar o jogo",
             "no estilo de um terminal de programacao.",
             "Por enquanto, apenas a visao 3D esta disponivel."},
            Color::YELLOW
        );
        RaycasterFrame::restoreLastFrame();
        firstIterationMouse = true;
        while (GetAsyncKeyState('V') & 0x8000) std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    struct PopupEntry {
        int key;
        void (*action)(Character*);
    };
    static const PopupEntry popups[] = {
        {VK_ESCAPE, [](Character* p) { PauseScreen::display(p); }},
        {'I',       [](Character* p) { CombatInventory::manageInventory(p); }},
        {'C',       [](Character* p) { AttributesScreen::managePlayerCharacterSheet(p); }},
        {'B',       [](Character* p) { DiaryScreen::display(p); }},
    };
    for (const auto& p : popups) {
        if (GetAsyncKeyState(p.key) & 0x8000) {
            firstIterationMouse = true;
            mouseHider.concert();
            InputControl::clearBuffer();
            p.action(player);
            RaycasterFrame::restoreLastFrame();
            tp1 = chrono::steady_clock::now();
            break;
        }
    }

    if (GetAsyncKeyState('M') & 0x8000) {
        firstIterationMouse = true;
        mouseHider.concert();
        while (GetAsyncKeyState('M') & 0x8000)
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        InputControl::clearBuffer();
        tp1 = chrono::steady_clock::now();
        return 'M';
    }

    {
        int keysDebug[] = {VK_OEM_3, VK_OEM_5, VK_OEM_PLUS, VK_OEM_102};
        bool pressedDebug = false;
        for (int td : keysDebug)
            if (GetAsyncKeyState(td) & 0x8000) { pressedDebug = true; break; }
        if (pressedDebug) {
            firstIterationMouse = true;
            mouseHider.concert();
            for (int td : keysDebug)
                while (GetAsyncKeyState(td) & 0x8000)
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
            InputControl::clearBuffer();
            Debug::displayDebugMenu(player);
            RaycasterFrame::restoreLastFrame();
            tp1 = chrono::steady_clock::now();
        }
    }

    // Movimento e Strafing (Com sistema de Sliding)
    float moveX = 0.0f;
    float moveY = 0.0f;

    if (GetAsyncKeyState('W') & 0x8000) {
        isMoving = true;
        moveX += cosf(angleVisa) * speedMovement * timeDelta;
        moveY += sinf(angleVisa) * speedMovement * timeDelta;
    }
    if (GetAsyncKeyState('S') & 0x8000) {
        isMoving = true;
        moveX -= cosf(angleVisa) * speedMovement * timeDelta;
        moveY -= sinf(angleVisa) * speedMovement * timeDelta;
    }
    if (GetAsyncKeyState('A') & 0x8000) { // Strafe Esquerda (-90 graus)
        isMoving = true;
        moveX += cosf(angleVisa - 1.5708f) * speedMovement * timeDelta;
        moveY += sinf(angleVisa - 1.5708f) * speedMovement * timeDelta;
    }
    if (GetAsyncKeyState('D') & 0x8000) { // Strafe Direita (+90 graus)
        isMoving = true;
        moveX += cosf(angleVisa + 1.5708f) * speedMovement * timeDelta;
        moveY += sinf(angleVisa + 1.5708f) * speedMovement * timeDelta;
    }

    if (isMoving) {
        float newX = playerX + moveX;
        float newY = playerY + moveY;
        
        if (newY >= 0 && newY < heightMap && playerX >= 0 && playerX < widthMap) {
            if (RaycasterWorld::isWalkable((int)playerX, (int)newY, mapMatrix)) playerY = newY;
        }
        if (playerY >= 0 && playerY < heightMap && newX >= 0 && newX < widthMap) {
            if (RaycasterWorld::isWalkable((int)newX, (int)playerY, mapMatrix)) playerX = newX;
        }
    }

    // Efeito de Head Bobbing (Balanco da Camera)
    if (isMoving) {
        bobbingTeam += timeDelta * 12.0f;
        bobbingAmplitude += timeDelta * 5.0f; // Aumenta a forca do passo
        if (bobbingAmplitude > 1.0f) bobbingAmplitude = 1.0f;
    } else {
        bobbingAmplitude -= timeDelta * 5.0f; // Suaviza a parada em 0.2 segundos
        if (bobbingAmplitude < 0.0f) {
            bobbingAmplitude = 0.0f;
            bobbingTeam = 0.0f;
        } else {
            bobbingTeam += timeDelta * 12.0f;
        }
    }
    bobbingOffset = (int)(sinf(bobbingTeam) * bobbingAmplitude * (SCREEN_HEIGHT * 0.02f));

    // Verifica se o player pisou em um trigger (Inimigo ou Teleporte) para acionar a transicao de mapa/combate
    int newCellX = (int)playerX;
    int newCellY = (int)playerY;
    if (newCellX != oldCellX || newCellY != oldCellY) {
        char cell = mapMatrix[newCellY][newCellX];
        bool isLabel = RaycasterWorld::isMapLabel(newCellX, newCellY, mapMatrix);
        if (RaycasterWorld::isTeleport(cell) || (!isLabel && RaycasterWorld::isEntity(cell))) {
            outHitX = newCellX;
            outHitY = newCellY;
            playerX = oldPlayerX; // Retorna para a exata posicao anterior flutuante
            playerY = oldPlayerY;
            running = false; // Sai do loop 3D e devolve o controle pro mapa top-down processar o evento!
        }
    }
#else
    // Fallback previne loop infinito caso compilado fora do windows
    running = false; 
#endif

    return '\0';
}
