// [PT-BR] Arquivo: DialogFunctions.h
// [PT-BR] Proposito: Funcoes para formatacao de dialogos de NPCs, mensagens de sistema e narrativas.
// [EN-US] File: DialogFunctions.h
// [EN-US] Purpose: Functions for formatting NPC dialogues, system messages, and narratives.

#pragma once

#include <string>
#include <vector>
#include "Core/Terminal/Appearance/Appearance.h"

// [PT-BR] Utilitarios de formatacao visual de falas e mensagens registradas no terminal.
// [EN-US] Visual formatting utilities for speeches and messages logged to the terminal.
class DialogueFunctions {
public:
    // [PT-BR] Exibe dialogo de NPC formatado com cores e identacao
    // [EN-US] Displays formatted NPC dialogue with colors and indentation
    static void printDialogueNPC(const std::string& npcName, Color npcColor, const std::string& text, bool newLineBefore = true, bool newLineAfter = true);
    static void printDialogueNPC(const std::string& npcName, Color npcColor, const std::vector<std::string>& lines);
    
    // [PT-BR] Formatadores de mensagens com estilos ANSI para cada categoria de evento
    // [EN-US] Message formatters with ANSI styles for each event category
    static std::string formatNarrationMsg(const std::string& text);
    static std::string formatSystemMsg(const std::string& text, Color themeColor = Color::YELLOW);
    static std::string formatSkillMsg(const std::string& text, Color themeColor = Color::LIGHT_GREEN);
    static std::string formatStatusMsg(const std::string& text, Color themeColor = Color::YELLOW);
    static std::string formatDropMsg(const std::string& text, Color themeColor = Color::WHITE);
    static std::string formatCombatMsg(const std::string& text, Color themeColor = Color::WHITE);
    static std::string formatInteractionMsg(const std::string& text, Color themeColor = Color::CYAN);
};
