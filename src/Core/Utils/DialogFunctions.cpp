// [PT-BR] Arquivo: DialogFunctions.cpp
// [PT-BR] Proposito: Implementacao das rotinas de exibicao e formatacao de dialogos de NPCs e sistema.
// [EN-US] File: DialogFunctions.cpp
// [EN-US] Purpose: Implementation of routines for displaying and formatting NPC dialogues and system messages.

#include "Core/Utils/DialogFunctions.h"
#include "Core/Terminal/Appearance/Appearance.h"
#include <iostream>

void DialogueFunctions::printDialogueNPC(const std::string& npcName, Color npcColor, const std::string& text, bool newLineBefore, bool newLineAfter) {
    if (newLineBefore) {
        std::cout << "\n";
        // [PT-BR] Imprime a tag colorida com o nome do NPC e reseta a cor ANSI
        // [EN-US] Prints colored NPC tag and resets ANSI color
        std::cout << Appearance::color(npcColor) << "[" << npcName << "]: " << Appearance::color(Color::RESET);
        // [PT-BR] Imprime o texto da fala com efeito de digitacao
        // [EN-US] Prints dialogue text with typewriter effect
        Appearance::printTyping(text, Appearance::typingDelayMS, newLineAfter);
    } else {
        // [PT-BR] Calcula o preenchimento para alinhar com a primeira linha do dialogo
        // [EN-US] Computes padding to align subsequent lines with the first dialogue line
        std::string tag = "[" + npcName + "]: ";
        std::cout << std::string(tag.length(), ' ');
        // [PT-BR] Imprime a continuacao da fala com efeito de digitacao
        // [EN-US] Prints continued dialogue with typewriter effect
        Appearance::printTyping(text, Appearance::typingDelayMS, newLineAfter);
    }
}

void DialogueFunctions::printDialogueNPC(const std::string& npcName, Color npcColor, const std::vector<std::string>& lines) {
    if (lines.empty()) return;
    
    // [PT-BR] A primeira linha imprime a quebra de linha inicial e o nome do NPC
    // [EN-US] The first line prints the initial newline and the NPC name tag
    printDialogueNPC(npcName, npcColor, lines[0], true, true);
    
    // [PT-BR] As linhas subsequentes herdam o alinhamento visual da tag
    // [EN-US] Subsequent lines inherit the visual alignment from the tag
    for (size_t i = 1; i < lines.size(); ++i) {
        printDialogueNPC(npcName, npcColor, lines[i], false, true);
    }
}

std::string DialogueFunctions::formatNarrationMsg(const std::string& text) {
    return Appearance::color(Color::GRAY) + "[NARRACAO]: " + text + Appearance::color(Color::RESET);
}

std::string DialogueFunctions::formatSystemMsg(const std::string& text, Color themeColor) {
    return Appearance::color(themeColor) + "[SISTEMA]: " + text + Appearance::color(Color::RESET);
}

std::string DialogueFunctions::formatSkillMsg(const std::string& text, Color themeColor) {
    if (themeColor == Color::LIGHT_GREEN) themeColor = Color::PURPLE;
    return Appearance::color(themeColor) + "[HABILIDADE]: " + text + Appearance::color(Color::RESET);
}

std::string DialogueFunctions::formatStatusMsg(const std::string& text, Color themeColor) {
    return Appearance::color(themeColor) + "[STATUS]: " + text + Appearance::color(Color::RESET);
}

std::string DialogueFunctions::formatDropMsg(const std::string& text, Color themeColor) {
    return Appearance::color(themeColor) + "[DROP]: " + text + Appearance::color(Color::RESET);
}

std::string DialogueFunctions::formatCombatMsg(const std::string& text, Color themeColor) {
    return Appearance::color(themeColor) + "[COMBATE]: " + text + Appearance::color(Color::RESET);
}

std::string DialogueFunctions::formatInteractionMsg(const std::string& text, Color themeColor) {
    return Appearance::color(themeColor) + "[INTERACAO]: " + text + Appearance::color(Color::RESET);
}
