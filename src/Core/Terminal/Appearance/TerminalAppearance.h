#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <functional>

class TerminalAppearance {
public:
    // CORES DA PALETA C++
    static std::string colorKeyword();
    static inline std::string colorWordKey() { return colorKeyword(); }
    static std::string colorType();
    static std::string colorString();
    static std::string colorNumber();
    static std::string colorComment();
    static std::string colorFunction();
    static std::string colorVariable();
    static std::string colorOperator();
    static std::string colorReset();

    // FUNCOES DE BARRA DE VIDA COM CARACTERES ASCII
    static std::string generateLifeBarASCII(double pct, int size);
    static inline std::string generateBarLifeASCII(double pct, int size) { return generateLifeBarASCII(pct, size); }
    static std::string generateHealthBarIDE(double pct, int size);

    // ESTILIZACAO DE TEXTO EM ESTILO CODIGO
    static std::string styleAsCode(const std::string& text, const std::string& type = "string");
    static inline std::string styleHowCode(const std::string& text, const std::string& type = "string") { return styleAsCode(text, type); }
    static std::string styleAsType(const std::string& text);
    static inline std::string styleHowType(const std::string& text) { return styleAsType(text); }
    static std::string styleAsFunction(const std::string& text);
    static inline std::string styleHowFunction(const std::string& text) { return styleAsFunction(text); }
    static std::string styleAsComment(const std::string& text);
    static inline std::string styleHowComment(const std::string& text) { return styleAsComment(text); }
    static std::string styleAsNumber(const std::string& text);
    static inline std::string styleHowNumber(const std::string& text) { return styleAsNumber(text); }

    // CONSTRUCAO DE CAIXAS EM ESTILO CODIGO
    static std::vector<std::string> createBoxCode(const std::vector<std::string>& content, const std::string& title = "Info");
    static std::vector<std::string> createBoxDrop(const std::vector<std::string>& drops);

    // IMPRESSOES ESPECIFICAS DO TERMINAL
    static void printTitleAsCode(const std::string& title);
    static inline void printTitleHowCode(const std::string& title) { printTitleAsCode(title); }
    static void printStatsAsStruct(const std::vector<std::pair<std::string, std::string>>& fields);
    static inline void printStatsHowStruct(const std::vector<std::pair<std::string, std::string>>& fields) { printStatsAsStruct(fields); }
    static void printCombatLogs(const std::vector<std::string>& messages);
    static inline void printLogsCombat(const std::vector<std::string>& messages) { printCombatLogs(messages); }
    static void printBoxDrops(const std::vector<std::string>& drops);

    // UTILIDADES DE FORMATACAO
    static std::string formatVarName(const std::string& name);
    static std::string formatType(const std::string& type);
    static std::string formatValue(const std::string& value);
    static std::string formatOperation(const std::string& operation);
};

// Apelido para manter compatibilidade
using AppearanceTerminal = TerminalAppearance;
