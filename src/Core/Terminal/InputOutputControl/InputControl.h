/*
 * Arquivo: InputControl.h
 * Propósito: Controle de entrada do usuário pelo terminal (teclado, mouse e captura de caracteres).
 */

#pragma once

#include <string>
#include <vector>
#include "Core/Terminal/Appearance/Appearance.h"
#include <functional>

// Comandos de navegação mapeados a partir de teclas
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

/*
 * Abstrai a captura e o processamento de entradas de teclado e mouse.
 */
class InputControl 
{
public:
    // Verifica se alguma tecla foi pressionada
    static bool pressedKey();

    // Lê um caractere do teclado sem necessidade de pressionar Enter
    static char readKey();

    // Converte a tecla pressionada no comando correspondente
    static MapCommand translateKeyToCommand(char key);

    // Limpa o buffer de entrada do terminal
    static void clearBuffer();

    // Lê uma string digitada pelo usuário protegendo contra entradas inválidas
    static std::string readEntryProtected(const std::string& promptMessage = "");
    
    // Habilita e gerencia captura de mouse no terminal Windows
    static void enableMouseInput();
    static bool readStateDragHorizontalMouse(int& deltaX);
    
    // Leituras estruturadas de inteiros e seleções de menu
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

