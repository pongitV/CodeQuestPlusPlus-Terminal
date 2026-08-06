/*
 * Arquivo: DialogFunctions.cpp
 * Proposito: Implementacao das rotinas de exibicao e formatacao de dialogos de NPCs e sistema.
 */

#include "Core/Utils/DialogFunctions.h"
#include "Core/Terminal/Appearance/Appearance.h"
#include <iostream>

void DialogueFunctions::printDialogueNPC(const std::string& npcName, Color npcColor, const std::string& text, bool newLineBefore, bool newLineAfter) {
    if (newLineBefore) {
        std::cout << "\n";
        // Imprime a tag colorida, depois reseta a cor
        std::cout << Appearance::color(npcColor) << "[" << npcName << "]: " << Appearance::color(Color::RESET);
        // Imprime o texto com a cor padrao
        Appearance::printTyping(text, Appearance::typingDelayMS, newLineAfter);
    } else {
        // Calcula o preenchimento para alinhar com o texto da primeira linha e o imprime
        std::string tag = "[" + npcName + "]: ";
        std::cout << std::string(tag.length(), ' ');
        // Imprime o texto com a cor padrao
        Appearance::printTyping(text, Appearance::typingDelayMS, newLineAfter);
    }
}

void DialogueFunctions::printDialogueNPC(const std::string& npcName, Color npcColor, const std::vector<std::string>& lines) {
    if (lines.empty()) return;
    
    // A primeira linha imprime a quebra de linha inicial e o Nome
    printDialogueNPC(npcName, npcColor, lines[0], true, true);
    
    // As linhas subsequentes apenas herdam o alinhamento
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
