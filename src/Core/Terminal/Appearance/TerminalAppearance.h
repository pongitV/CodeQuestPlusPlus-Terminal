/*
 * Arquivo: TerminalAppearance.h
 * Proposito: Define cores e estilos especificos para simular a aparencia de uma IDE/codigo no terminal.
 */
#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <functional>

class AppearanceTerminal {
public:
    // --- CORES DA PALETA C++ ---
    static std::string colorWordKey();
    static std::string colorType();
    static std::string colorString();
    static std::string colorNumber();
    static std::string colorComment();
    static std::string colorFunction();
    static std::string colorVariable();
    static std::string colorOperator();
    static std::string colorReset();

    // --- FUNCOES DE BARRA DE VIDA COM CARACTERES ASCII ---
    static std::string generateBarLifeASCII(double pct, int size);
    static std::string generateBarLifeGO(double pct, int size);

    // --- ESTILIZACAO DE TEXTO EM ESTILO CODIGO ---
    static std::string styleHowCode(const std::string& text, const std::string& type = "string");
    static std::string styleHowType(const std::string& text);
    static std::string styleHowFunction(const std::string& text);
    static std::string styleHowComment(const std::string& text);
    static std::string styleHowNumber(const std::string& text);

    // --- CONSTRUCAO DE CAIXAS EM ESTILO CODIGO ---
    static std::vector<std::string> createBoxCode(const std::vector<std::string>& content, const std::string& title = "Info");
    static std::vector<std::string> createBoxDrop(const std::vector<std::string>& drops);

    // --- IMPRESSOES ESPECIFICAS DO TERMINAL ---
    static void printTitleHowCode(const std::string& title);
    static void printStatsHowStruct(const std::vector<std::pair<std::string, std::string>>& fields);
    static void printLogsCombat(const std::vector<std::string>& messages);
    static void printBoxDrops(const std::vector<std::string>& drops);

    // --- UTILIDADES DE FORMATACAO ---
    static std::string formatVarName(const std::string& name);
    static std::string formatType(const std::string& type);
    static std::string formatValue(const std::string& value);
    static std::string formatOperation(const std::string& operation);
};
