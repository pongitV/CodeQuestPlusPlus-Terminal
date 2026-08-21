#include <iostream>
#include <memory>
#include <array>

#ifdef _WIN32
    #include <windows.h>
    // [PT-BR] Necessário para a chamada ShellExecuteEx (elevação de privilégios)
    // [EN-US] Required for ShellExecuteEx call (privilege elevation)
    #include <shellapi.h>
    // [PT-BR] Necessário para a verificação IsUserAnAdmin
    // [EN-US] Required for IsUserAnAdmin verification
    #include <shlobj.h>
#endif

#include "Domain/Characters/Classes/Archer.h"
#include "Domain/Characters/Classes/Bard.h"
#include "Domain/Characters/Classes/BaseClass.h"
#include "Domain/Characters/Classes/Warrior.h"
#include "Domain/Characters/Classes/Mage.h"
#include "Core/Engine/GameMenu.h"
#include "World/Village/Map1Village.h"
#include "World/Forest/Map2Forest.h"
#include "World/Kingdom/Map3KingdomBridge.h"
#include "World/Kingdom/Map4Kingdom.h"
#include "Domain/Characters/Races/Dwarf.h"
#include "Domain/Characters/Races/Elf.h"
#include "Domain/Characters/Races/Human.h"
#include "Domain/Characters/Races/Orc.h"
#include "Systems/Progression/Progression.h"
#include "Systems/Progression/ProgressionFlags.h"
#include "Core/Utils/Appearance.h"
#include "Core/Terminal/InputOutputControl/InputControl.h"
#include "UI/PerspectiveManager.h"

// [PT-BR] Garante que o processo do jogo execute com privilégios de Administrador no Windows.
// [EN-US] Ensures the game process runs with Administrator privileges on Windows.
bool ensureAdmin() noexcept 
{
#ifdef _WIN32
    if (!IsUserAnAdmin()) 
    {
        std::array<char, MAX_PATH> pathBuffer{};
        if (GetModuleFileNameA(nullptr, pathBuffer.data(), MAX_PATH) != 0) 
        {
            SHELLEXECUTEINFOA sei{};
            sei.cbSize = sizeof(sei);
            // [PT-BR] Verbo "runas" solicita elevação UAC
            // [EN-US] "runas" verb requests UAC privilege elevation
            sei.lpVerb = "runas";
            sei.lpFile = pathBuffer.data();
            sei.hwnd = nullptr;
            sei.nShow = SW_NORMAL;

            if (ShellExecuteExA(&sei)) 
            {
                // [PT-BR] Sucesso ao abrir nova instância com privilégios elevados; encerra a instância atual
                // [EN-US] Successfully launched elevated instance; terminates current instance
                return true;
            }
        }
    }
#endif
    // [PT-BR] Continua a execução normal (já é administrador ou plataforma não-Windows)
    // [EN-US] Continues normal execution (already admin or non-Windows platform)
    return false;
}

#include "Core/Engine/StateManager.h"
#include "Core/Terminal/TerminalSessionGuard.h"

int main() 
{
    // [PT-BR] 1. Tenta elevar privilégios para Administrador antes de iniciar os subsistemas
    // [EN-US] 1. Attempts to elevate privileges to Administrator before booting subsystems
    if (ensureAdmin()) return 0;

    // [PT-BR] Guard RAII para garantir a restauracao do terminal no encerramento
    // [EN-US] RAII guard to guarantee terminal restoration upon exit
    TerminalSessionGuard sessionGuard;

    // [PT-BR] 2. Configura a tela do console e inicializa o modo de renderização do terminal
    // [EN-US] 2. Configures the console display and initializes the terminal rendering mode
    Appearance::bootConsole();
    Appearance::maximizeWindowTerminal(); 
    Appearance::clearScreen();
    
    // [PT-BR] Configura a captura de eventos de mouse no console
    // [EN-US] Configures mouse event capture in the console
    InputControl::enableMouseInput();
    
    // [PT-BR] 3. Inicializa os renderizadores no Gerenciador de Perspectivas (PerspectiveManager)
    // [EN-US] 3. Initializes perspective renderers in PerspectiveManager
    PerspectiveManager::getInstance().boot();

    // [PT-BR] 4. Inicia o loop principal do jogo através do padrão de estados (StateManager)
    // [EN-US] 4. Starts the main game loop using the State Pattern (StateManager)
    Game rpg(std::make_unique<MenuState>());
    rpg.run();

    return 0;
}
