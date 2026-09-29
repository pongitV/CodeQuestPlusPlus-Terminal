#include "UI/Renderers/IDE/IDEScreens/Map/IDEMapScreen.h"
#include "UI/Renderers/IDE/IDETheme.h"
#include "Core/Utils/Appearance.h"
#include <iostream>
#include <vector>

void IDEMapScreen::renderPopup(const std::vector<std::string>& /*arte*/, const std::vector<std::string>& places, int selection, bool /*redesenhoCompleto*/) {
    Appearance::clearScreen();
    int width = Appearance::getTerminalWidth();
    int height = Appearance::getTerminalHeight();

    std::vector<std::string> lines;
    lines.push_back(IDETheme::preprocessor("#pragma once"));
    lines.push_back(IDETheme::keyword("namespace ") + IDETheme::type("World::Navigation") + IDETheme::punctuation(" {"));
    lines.push_back("");
    lines.push_back("    " + IDETheme::keyword("enum class ") + IDETheme::type("Sector") + IDETheme::punctuation(" : uint8_t {"));

    for (size_t i = 0; i < places.size(); ++i) {
        std::string placeName = places[i];
        for (char& c : placeName) {
            if (c == ' ') c = '_';
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        }

        std::string line = "        " + placeName + " = 0x" + (i < 16 ? "0" : "") + std::to_string(i) + ",";
        if (static_cast<int>(i) == selection) {
            lines.push_back(std::string(IDETheme::COLOR_ACTIVE_TAB) + "> " + line + " // [ALVO DE ROTEAMENTO] <" + std::string(IDETheme::COLOR_RESET));
        } else {
            lines.push_back("  " + IDETheme::punctuation(line));
        }
    }

    lines.push_back("    " + IDETheme::punctuation("};"));
    lines.push_back("");
    lines.push_back("    " + IDETheme::type("Sector") + " " + IDETheme::variable("activeDestination") + " = " + IDETheme::type("Sector::") + "[selection]" + ";");
    lines.push_back(IDETheme::punctuation("} // namespace World::Navigation"));

    std::vector<std::string> tabs = {
        "WorldMatrix.hpp",
        "FastTravelRouter.cpp"
    };
    auto editorView = IDETheme::renderEditorView(tabs, 0, "// src/World/FastTravelRouter.cpp > enum class Sector", lines, width, height, "[A/D ou W/S] Mudar Setor | [ENTER] Roteamento | [ESC] Fechar");

    for (const auto& l : editorView) std::cout << l << "\n";
    std::cout << "\033[J" << std::flush;
}
