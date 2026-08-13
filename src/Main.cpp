#include <iostream>
#include <memory>
#include <array>

#ifdef _WIN32
    #include <windows.h>
    #include <shellapi.h> // Necessario para ShellExecuteEx
    #include <shlobj.h>   // Necessario para IsUserAnAdmin
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

// Funcao para garantir que o jogo rode como Administrador
bool ensureAdmin() noexcept 
{
#ifdef _WIN32
    if (!IsUserAnAdmin()) 
    {
        std::array<char, MAX_PATH> pathBuffer{};
        if (GetModuleFileNameA(nullptr, pathBuffer.data(), MAX_PATH) != 0) 
        {
            SHELLEXECUTEINFOA sei = { sizeof(sei) };
            sei.lpVerb = "runas"; // Comando para elevar privilegios
            sei.lpFile = pathBuffer.data();
            sei.hwnd = nullptr;
            sei.nShow = SW_NORMAL;

            if (ShellExecuteExA(&sei)) 
            {
                return true; // Sucesso ao abrir nova instancia, fechar a atual
            }
        }
    }
#endif
    return false; // Continua execucao normal (ja e admin ou falhou)
}

#include "Core/Engine/StateManager.h"

int main() 
{
    // 1. Tenta elevar para Administrador antes de tudo
    if (ensureAdmin()) return 0;

    // 2. Configura a tela (agora com permissao total)
    Appearance::bootConsole();
    Appearance::maximizeWindowTerminal(); 
    Appearance::clearScreen();
    
    // Configura Mouse Events do Console
    InputControl::enableMouseInput();
    
    // 3. Inicializa os renders do Gerenciador de Visoes (Seta o RendererProvider)
    PerspectiveManager::getInstance().boot();

    // 3. Inicia o fluxo do jogo usando o State Pattern
    Game rpg(std::make_unique<MenuState>());
    rpg.run();

    return 0;
}
