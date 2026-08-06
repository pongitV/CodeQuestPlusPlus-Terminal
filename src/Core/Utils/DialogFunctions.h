/*
 * Arquivo: DialogFunctions.h
 * Proposito: Funcoes para formatacao de dialogos de NPCs, mensagens de sistema e narrativas.
 */

#pragma once

#include <string>
#include <vector>
#include "Core/Terminal/Appearance/Appearance.h"

/*
 * Utilitarios de formatacao visual de falas e mensagens registradas no terminal.
 */
class DialogueFunctions {
public:
    // Exibe dialogo de NPC formatado com cores e identacao
    static void printDialogueNPC(const std::string& npcName, Color npcColor, const std::string& text, bool newLineBefore = true, bool newLineAfter = true);
    static void printDialogueNPC(const std::string& npcName, Color npcColor, const std::vector<std::string>& lines);
    
    // Formatadores de mensagens com estilos ANSI para cada categoria de evento
    static std::string formatNarrationMsg(const std::string& text);
    static std::string formatSystemMsg(const std::string& text, Color themeColor = Color::YELLOW);
    static std::string formatSkillMsg(const std::string& text, Color themeColor = Color::LIGHT_GREEN);
    static std::string formatStatusMsg(const std::string& text, Color themeColor = Color::YELLOW);
    static std::string formatDropMsg(const std::string& text, Color themeColor = Color::WHITE);
    static std::string formatCombatMsg(const std::string& text, Color themeColor = Color::WHITE);
    static std::string formatInteractionMsg(const std::string& text, Color themeColor = Color::CYAN);
};
