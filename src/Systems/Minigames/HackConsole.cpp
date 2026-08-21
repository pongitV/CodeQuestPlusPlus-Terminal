#include "Systems/Minigames/HackConsole.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "Domain/Characters/Character.h"
#include <iostream>
#include <vector>

using namespace std;

bool HackConsole::startHack(Character* player) {
    Appearance::clearScreen();
    
    vector<string> linesCode = {
        "// [PT-BR] Sistema de Seguranca v1.2",
        "// [EN-US] Security System v1.2",
        "bool unlockDoor() {",
        "    int accessLevel = getPlayerAccess();",
        "    if (accessLevel < 5) {",
        "        ______ false; // [PT-BR] ERRO AQUI: Faltando palavra-chave de retorno",
        "                      // [EN-US] ERROR HERE: Missing return keyword",
        "    }",
        "    return true;",
        "}"
    };
    
    string answer = "";
    bool running = true;
    bool hackSuccess = false;
    
    while (running) {
        Appearance::clearScreen();
        cout << Appearance::color(Color::CYAN) << "=== CONSOLE DE DESENVOLVEDOR ===" << Appearance::color(Color::RESET) << "\n\n";
        
        for (const auto& line : linesCode) {
            cout << line << "\n";
        }
        
        cout << "\n> Insira a palavra-chave que falta: " << answer;
        
        char key = InputControl::readKey();
        if (key == 13) {
            // [PT-BR] Tecla ENTER confirmada
            // [EN-US] ENTER key confirmed
            if (answer == "return") {
                hackSuccess = true;
                running = false;
            } else {
                cout << "\n\n" << Appearance::color(Color::RED) << "[ERRO DE SINTAXE] Acesso negado." << Appearance::color(Color::RESET);
                InputControl::waitForEnter();
                running = false;
            }
        } else if (key == 8) {
            // [PT-BR] Tecla BACKSPACE pressionada
            // [EN-US] BACKSPACE key pressed
            if (!answer.empty()) answer.pop_back();
        } else if (key == 27) {
            // [PT-BR] Tecla ESC para cancelar
            // [EN-US] ESC key to cancel
            running = false;
        } else if (key >= 32 && key <= 126) {
            answer += key;
        }
    }
    
    if (hackSuccess) {
        Appearance::clearScreen();
        cout << Appearance::color(Color::GREEN) << "\n[COMPILACAO BEM-SUCEDIDA] Acesso de dev concedido." << Appearance::color(Color::RESET) << "\n";
        
        cout << "\nVoce encontrou um Log corrompido! Ganhou +200 XP e curou vida.\n";
        player->gainXp(200);
        player->modifyHealth(50);
        InputControl::waitForEnter();
    }
    
    return hackSuccess;
}
