#include <iostream>
#include <memory>
#include <array>

#ifdef _WIN32
    #include <windows.h>
    // Necessario para a chamada ShellExecuteEx (elevacao de privilegios)
    #include <shellapi.h>
    // Necessario para a verificacao IsUserAnAdmin
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

// Garante que o processo do jogo execute com privilegios de Administrador no Windows.
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
            // Verbo "runas" solicita elevacao UAC
            sei.lpVerb = "runas";
            sei.lpFile = pathBuffer.data();
            sei.hwnd = nullptr;
            sei.nShow = SW_NORMAL;

            if (ShellExecuteExA(&sei)) 
            {
                // Sucesso ao abrir nova instancia com privilegios elevados; encerra a instancia atual
                return true;
            }
        }
    }
#endif
    // Continua a execucao normal (ja e administrador ou plataforma nao-Windows)
    return false;
}

#include "Core/Engine/StateManager.h"
#include "Core/Terminal/TerminalSessionGuard.h"

int main() 
{
    // 1. Tenta elevar privilegios para Administrador antes de iniciar os subsistemas
    if (ensureAdmin()) return 0;

    // Guard RAII para garantir a restauracao do terminal no encerramento
    TerminalSessionGuard sessionGuard;

    // 2. Configura a tela do console e inicializa o modo de renderizacao do terminal
    Appearance::bootConsole();
    Appearance::maximizeWindowTerminal(); 
    Appearance::clearScreen();
    
    // Configura a captura de eventos de mouse no console
    InputControl::enableMouseInput();
    
    // 3. Inicializa os renderizadores no Gerenciador de Perspectivas (PerspectiveManager)
    PerspectiveManager::getInstance().boot();

    // 4. Inicia o loop principal do jogo atraves do padrao de estados (StateManager)
    Game rpg(std::make_unique<MenuState>());
    rpg.run();

    return 0;
}
