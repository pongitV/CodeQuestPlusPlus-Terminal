// [PT-BR] Arquivo: BaseScreen.h
// [PT-BR] Proposito: Classe base estatica para construcao e renderizacao de molduras, barras e loops de menu de telas.
// [EN-US] File: BaseScreen.h
// [EN-US] Purpose: Static base class for building and rendering borders, bars, and menu loops of screens.

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include "Core/Utils/Appearance.h"

class Character;

// [PT-BR] Provedor de utilitarios de renderizacao de telas e loops interativos.
// [EN-US] Provider of screen rendering utilities and interactive loops.
class BaseScreen {
public:
    static std::string generateBarGradient(double pct, int size, Color colorEnd);
    static void printLineDivider(char character = '=');
    static std::vector<std::string> createBox(const std::vector<std::string>& lines, const std::string& title = "", int widthMinimal = 0, Color colorBox = Color::WHITE, const std::string& bgAnsi = "");
    static std::vector<std::string> createBoxWithArt(const std::vector<std::string>& art, const std::vector<std::string>& linesText, const std::string& title = "", int widthMinimal = 0, Color colorBox = Color::WHITE, const std::string& bgAnsi = "");

    static void executeLoop(
        const std::function<void(bool)>& renderHeader,
        const std::function<void()>& renderContent,
        const std::function<std::vector<std::string>()>& builderOptionsMenu,
        const std::function<bool(int)>& processChoose,
        bool centralizeMenu = true,
        const std::string& marginMenu = ""
    );
    
    static void executeDefaultLoop(
        const std::string& title,
        Color themeColor,
        const std::function<void()>& renderContent,
        const std::function<std::vector<std::string>()>& builderOptionsMenu,
        const std::function<bool(int)>& processChoose
    );

    static bool mustAnimateScreenEntry(std::chrono::steady_clock::time_point& lastAccess, int delayMilliseconds);
};