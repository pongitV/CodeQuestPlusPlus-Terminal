// [PT-BR] Arquivo: TerminalAppearance.h
// [PT-BR] Proposito: Define cores e estilos especificos para simular a aparencia de uma IDE/codigo no terminal.
// [EN-US] File: TerminalAppearance.h
// [EN-US] Purpose: Defines colors and styles to simulate an IDE/code appearance in the terminal.

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <functional>

class AppearanceTerminal {
public:
    // [PT-BR] --- CORES DA PALETA C++ ---
    // [EN-US] --- C++ PALETTE COLORS ---
    static std::string colorWordKey();
    static std::string colorType();
    static std::string colorString();
    static std::string colorNumber();
    static std::string colorComment();
    static std::string colorFunction();
    static std::string colorVariable();
    static std::string colorOperator();
    static std::string colorReset();

    // [PT-BR] --- FUNCOES DE BARRA DE VIDA COM CARACTERES ASCII ---
    // [EN-US] --- HEALTH BAR FUNCTIONS WITH ASCII CHARACTERS ---
    static std::string generateBarLifeASCII(double pct, int size);
    static std::string generateHealthBarIDE(double pct, int size);

    // [PT-BR] --- ESTILIZACAO DE TEXTO EM ESTILO CODIGO ---
    // [EN-US] --- CODE-STYLE TEXT STYLING ---
    static std::string styleHowCode(const std::string& text, const std::string& type = "string");
    static std::string styleHowType(const std::string& text);
    static std::string styleHowFunction(const std::string& text);
    static std::string styleHowComment(const std::string& text);
    static std::string styleHowNumber(const std::string& text);

    // [PT-BR] --- CONSTRUCAO DE CAIXAS EM ESTILO CODIGO ---
    // [EN-US] --- CODE-STYLE BOX CONSTRUCTION ---
    static std::vector<std::string> createBoxCode(const std::vector<std::string>& content, const std::string& title = "Info");
    static std::vector<std::string> createBoxDrop(const std::vector<std::string>& drops);

    // [PT-BR] --- IMPRESSOES ESPECIFICAS DO TERMINAL ---
    // [EN-US] --- TERMINAL-SPECIFIC PRINTING ---
    static void printTitleHowCode(const std::string& title);
    static void printStatsHowStruct(const std::vector<std::pair<std::string, std::string>>& fields);
    static void printLogsCombat(const std::vector<std::string>& messages);
    static void printBoxDrops(const std::vector<std::string>& drops);

    // [PT-BR] --- UTILIDADES DE FORMATACAO ---
    // [EN-US] --- FORMATTING UTILITIES ---
    static std::string formatVarName(const std::string& name);
    static std::string formatType(const std::string& type);
    static std::string formatValue(const std::string& value);
    static std::string formatOperation(const std::string& operation);
};
