// [PT-BR] Arquivo: InputControl.h
// [PT-BR] Proposito: Controle de entrada do usuario pelo terminal (teclado, mouse e captura de caracteres).
// [EN-US] File: InputControl.h
// [EN-US] Purpose: User input control via terminal (keyboard, mouse, and character capturing).

#pragma once

#include <string>
#include <vector>
#include "Core/Terminal/Appearance/Appearance.h"
#include <functional>

// [PT-BR] Comandos de navegacao mapeados a partir de teclas
// [EN-US] Navigation commands mapped from keys
enum class MapCommand {
    Up,
    Down,
    Left,
    Right,
    Inventory,
    CharacterSheet,
    Bestiary,
    None
};

// [PT-BR] Abstrai a captura e o processamento de entradas de teclado e mouse.
// [EN-US] Abstracts keyboard and mouse input capturing and processing.
class InputControl 
{
public:
    // [PT-BR] Verifica se alguma tecla foi pressionada (non-blocking)
    // [EN-US] Checks if any key was pressed (non-blocking)
    static bool pressedKey();

    // [PT-BR] Le um caractere do teclado sem necessidade de pressionar Enter
    // [EN-US] Reads a character from the keyboard without requiring Enter
    static char readKey();

    // [PT-BR] Converte a tecla pressionada no comando correspondente
    // [EN-US] Translates pressed key into the corresponding command
    static MapCommand translateKeyToCommand(char key);

    // [PT-BR] Limpa o buffer de entrada do terminal
    // [EN-US] Clears terminal input buffer
    static void clearBuffer();

    // [PT-BR] Le uma string digitada pelo usuario protegendo contra entradas invalidas
    // [EN-US] Reads a user-input string protected against invalid entries
    static std::string readEntryProtected(const std::string& promptMessage = "");
    
    // [PT-BR] Habilita e gerencia captura de mouse no terminal Windows
    // [EN-US] Enables and manages mouse capture in Windows terminal
    static void enableMouseInput();
    static bool pollMouseState(int& mouseX, int& mouseY, bool& isLeftPressed, bool& isRightPressed);
    
    // [PT-BR] Leituras estruturadas de inteiros e selecoes de menu
    // [EN-US] Structured integer readings and menu selection prompts
    static int readIntegerWithLimits(const std::string& promptMessage, int minimum, int maximum, bool centralizePrompt = false, const std::string& marginPersonalized = "");
    static int readSelectionMenuWithArrows(const std::vector<std::string>& options, bool centralize = true, const std::string& marginPersonalized = "", const std::vector<std::string>& panelRight = {});
    static int readMenuSelectionInPopup(const std::string& title, const std::vector<std::string>& text, const std::vector<std::string>& options, Color themeColor = Color::WHITE, const std::vector<std::string>& asciiArt = {}, bool animateEntrance = true);
    static void waitForEnter(const std::string& message = "Pressione ENTER para continuar...");
    
    static void executeLoopMenuPopup(
        const std::function<void()>& displayDialogue,
        const std::function<std::vector<std::string>()>& getOptions,
        const std::function<bool(const std::string&)>& processOption,
        const std::string& title,
        Color themeColor,
        const std::vector<std::string>& asciiArt
    );
    static std::function<void()> onWaitEnterUpdate;
    static std::string enterPromptText;
};
