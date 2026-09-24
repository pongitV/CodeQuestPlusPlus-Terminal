#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <sstream>
#include "Core/Utils/Appearance.h"

namespace IDETheme {

    // [PT-BR] Paleta ANSI 24-bit TrueColor estilo VS Code Dark+ / OneDark
    // [EN-US] 24-bit TrueColor ANSI palette in VS Code Dark+ / OneDark style
    inline constexpr std::string_view COLOR_KEYWORD    = "\033[38;2;86;156;214m";  // Azul (#569cd6)
    inline constexpr std::string_view COLOR_TYPE       = "\033[38;2;78;201;176m";  // Ciano/Teal (#4ec9b0)
    inline constexpr std::string_view COLOR_STRING     = "\033[38;2;214;157;133m"; // Laranja Coral (#ce9178)
    inline constexpr std::string_view COLOR_NUMBER     = "\033[38;2;181;206;168m"; // Verde Claro (#b5cea8)
    inline constexpr std::string_view COLOR_COMMENT    = "\033[38;2;106;153;85m";  // Verde Comentário (#6a9955)
    inline constexpr std::string_view COLOR_FUNCTION   = "\033[38;2;220;220;170m"; // Amarelo Função (#dcdcaa)
    inline constexpr std::string_view COLOR_VARIABLE   = "\033[38;2;156;220;254m"; // Azul Claro Variável (#9cdcfe)
    inline constexpr std::string_view COLOR_OPERATOR   = "\033[38;2;212;212;212m"; // Cinza Pontuação (#d4d4d4)
    inline constexpr std::string_view COLOR_PREPROC    = "\033[38;2;197;134;192m"; // Roxo Preprocessador (#c586c0)
    inline constexpr std::string_view COLOR_HEADER_BG  = "\033[38;2;200;200;200m";
    inline constexpr std::string_view COLOR_ACTIVE_TAB = "\033[1;38;2;78;201;176m"; // Ciano brilhante bold (foreground only, sem fundo cinza)
    inline constexpr std::string_view COLOR_INACT_TAB  = "\033[38;2;130;130;130m"; // Cinza claro (foreground only, sem fundo cinza)
    inline constexpr std::string_view COLOR_LINE_NUM   = "\033[38;2;133;133;133m"; // Cinza número de linha
    inline constexpr std::string_view COLOR_STATUS_BAR = "\033[38;2;0;122;204m";   // Azul foreground
    inline constexpr std::string_view COLOR_FLASH_HIT  = "\033[1;38;2;244;71;71m";  // Vermelho impacto bold
    inline constexpr std::string_view COLOR_FLASH_CURE = "\033[1;38;2;78;201;176m"; // Verde/Ciano cura bold
    inline constexpr std::string_view COLOR_RESET      = "\033[0m";

    // Formatadores inline
    inline std::string keyword(std::string_view text) {
        return std::string(COLOR_KEYWORD) + std::string(text) + std::string(COLOR_RESET);
    }

    inline std::string type(std::string_view text) {
        return std::string(COLOR_TYPE) + std::string(text) + std::string(COLOR_RESET);
    }

    inline std::string variable(std::string_view text) {
        return std::string(COLOR_VARIABLE) + std::string(text) + std::string(COLOR_RESET);
    }

    inline std::string function(std::string_view text) {
        return std::string(COLOR_FUNCTION) + std::string(text) + std::string(COLOR_RESET);
    }

    inline std::string stringLiteral(std::string_view text) {
        return std::string(COLOR_STRING) + "\"" + std::string(text) + "\"" + std::string(COLOR_RESET);
    }

    inline std::string number(std::string_view text) {
        return std::string(COLOR_NUMBER) + std::string(text) + std::string(COLOR_RESET);
    }

    inline std::string number(int val) {
        return std::string(COLOR_NUMBER) + std::to_string(val) + std::string(COLOR_RESET);
    }

    inline std::string comment(std::string_view text) {
        return std::string(COLOR_COMMENT) + std::string(text) + std::string(COLOR_RESET);
    }

    inline std::string preprocessor(std::string_view text) {
        return std::string(COLOR_PREPROC) + std::string(text) + std::string(COLOR_RESET);
    }

    inline std::string punctuation(std::string_view text) {
        return std::string(COLOR_OPERATOR) + std::string(text) + std::string(COLOR_RESET);
    }

    inline std::string statement(std::string_view typeName, std::string_view varName, std::string_view value, std::string_view inlineComment = "") {
        std::string line = type(typeName) + " " + variable(varName) + punctuation(" = ") + std::string(value) + punctuation(";");
        if (!inlineComment.empty()) {
            line += " " + comment(inlineComment);
        }
        return line;
    }

    inline std::string renderTabBar(const std::vector<std::string>& tabs, size_t activeIndex, int /*totalWidth*/) {
        std::string bar = "";
        for (size_t i = 0; i < tabs.size(); ++i) {
            if (i == activeIndex) {
                bar += punctuation("[") + std::string(COLOR_ACTIVE_TAB) + tabs[i] + std::string(COLOR_RESET) + punctuation("] ");
            } else {
                bar += " " + std::string(COLOR_INACT_TAB) + tabs[i] + std::string(COLOR_RESET) + "  ";
            }
        }
        return bar;
    }

    inline std::string renderStatusBar(std::string_view leftText, std::string_view rightText, int totalWidth) {
        int leftLen = static_cast<int>(leftText.length());
        int rightLen = static_cast<int>(rightText.length());
        int spaces = totalWidth - leftLen - rightLen;
        if (spaces < 0) spaces = 0;
        return std::string(COLOR_STATUS_BAR) + " " + std::string(leftText) + std::string(spaces > 2 ? spaces - 2 : 0, ' ') + std::string(rightText) + " " + std::string(COLOR_RESET);
    }

    // [PT-BR] Barra de vida expressa no estilo de código C++: [████░░] 30/50
    // [EN-US] Health bar styled in C++ code syntax: [████░░] 30/50
    inline std::string renderCodeHealthBar(int current, int max, int barWidth = 10) {
        if (max <= 0) max = 1;
        float ratio = static_cast<float>(current) / static_cast<float>(max);
        if (ratio < 0.0f) ratio = 0.0f;
        if (ratio > 1.0f) ratio = 1.0f;
        int filled = static_cast<int>(ratio * barWidth);
        int empty = barWidth - filled;

        std::string barColor;
        if (ratio > 0.5f) barColor = "\033[38;2;78;201;176m";       // Ciano/Verde
        else if (ratio > 0.25f) barColor = "\033[38;2;220;220;170m"; // Amarelo
        else barColor = "\033[38;2;244;71;71m";                     // Vermelho perigo

        std::string result = punctuation("[") + barColor;
        for (int i = 0; i < filled; ++i) result += "█";
        result += "\033[38;2;90;90;90m";
        for (int i = 0; i < empty; ++i) result += "░";
        result += std::string(COLOR_RESET) + punctuation("] ") + number(current) + punctuation("/") + number(max);
        return result;
    }

    // [PT-BR] Centraliza uma linha individual de acordo com a largura do terminal
    // [EN-US] Centers an individual line according to terminal width
    inline std::string centerLine(const std::string& line, int totalWidth) {
        int vLen = Appearance::getVisualLength(line);
        int pad = std::max(0, (totalWidth - vLen) / 2);
        return std::string(pad, ' ') + line;
    }

    // [PT-BR] Centraliza um bloco de linhas preservando o alinhamento e indentação interna
    // [EN-US] Centers a block of lines while preserving internal alignment and indentation
    inline std::vector<std::string> centerBlock(const std::vector<std::string>& block, int totalWidth) {
        int maxLen = 0;
        for (const auto& l : block) {
            int v = Appearance::getVisualLength(l);
            if (v > maxLen) maxLen = v;
        }
        int pad = std::max(0, (totalWidth - maxLen) / 2);
        std::string padStr(pad, ' ');
        std::vector<std::string> result;
        result.reserve(block.size());
        for (const auto& l : block) {
            result.push_back(padStr + l);
        }
        return result;
    }

    // [PT-BR] Calcula espaçamento vertical superior para centralizar conteúdo
    // [EN-US] Calculates top vertical padding to center content
    inline int calculateTopPadding(int contentHeight, int termHeight) {
        if (termHeight <= contentHeight) return 0;
        return (termHeight - contentHeight) / 2;
    }

    // [PT-BR] Centraliza um bloco de linhas horizontalmente e verticalmente na tela
    // [EN-US] Centers a block of lines horizontally and vertically on screen
    inline std::vector<std::string> centerScreen(const std::vector<std::string>& block, int totalWidth, int totalHeight) {
        auto horizontallyCentered = centerBlock(block, totalWidth);
        int topPadding = calculateTopPadding(static_cast<int>(horizontallyCentered.size()), totalHeight);

        std::vector<std::string> result;
        result.reserve(topPadding + horizontallyCentered.size());
        for (int i = 0; i < topPadding; ++i) {
            result.push_back("");
        }
        result.insert(result.end(), horizontallyCentered.begin(), horizontallyCentered.end());
        return result;
    }

    // [PT-BR] Renderiza a visualização clássica de IDE:
    // [EN-US] Renders the classic IDE view:
    // - Linha 0: Abas coladas no teto / Tabs on top
    // - Linha 1: Caminho / Technical file breadcrumb
    // - Linhas restantes: Bloco de código-fonte centralizado / Centered source code block
    // - Rodapé: Barra de status / Technical telemetry status bar
    inline std::vector<std::string> renderEditorView(
        const std::vector<std::string>& tabs,
        int activeTab,
        const std::string& breadcrumb,
        const std::vector<std::string>& codeBlock,
        int totalWidth,
        int totalHeight,
        const std::string& footer = ""
    ) {
        std::vector<std::string> result;
        // Linha 0: Abas no teto
        result.push_back(renderTabBar(tabs, activeTab, totalWidth));

        // Linha 1: Breadcrumb do arquivo/classe
        if (!breadcrumb.empty()) {
            result.push_back(comment(breadcrumb));
        } else {
            result.push_back("");
        }

        auto centeredCode = centerBlock(codeBlock, totalWidth);
        int reservedLines = static_cast<int>(result.size()) + (footer.empty() ? 0 : 2);
        int availableH = std::max(0, totalHeight - reservedLines);
        int topPadding = calculateTopPadding(static_cast<int>(centeredCode.size()), availableH);

        for (int i = 0; i < topPadding; ++i) {
            result.push_back("");
        }
        result.insert(result.end(), centeredCode.begin(), centeredCode.end());

        if (!footer.empty()) {
            result.push_back("");
            result.push_back(centerLine(footer, totalWidth));
        }

        return result;
    }

} // namespace IDETheme


