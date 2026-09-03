#include "UI/Renderers/IDE/IDEScreens/Pause/IDEPauseScreen.h"
#include "UI/Renderers/IDE/IDETheme.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include <iostream>
#include <vector>

namespace {
    int renderMenuIDE(const std::string& title, const std::vector<std::string>& options) {
        int selected = 0;
        int maxOp = static_cast<int>(options.size());
        InputControl::clearBuffer();

        while (true) {
            Appearance::clearScreen();
            int width = Appearance::getTerminalWidth();
            int height = Appearance::getTerminalHeight();

            std::vector<std::string> lines;
            lines.push_back(IDETheme::comment("// Breakpoint disparado: " + title));
            lines.push_back(IDETheme::comment("// Execução do process suspensa na thread. Selecione uma directive:"));
            lines.push_back("");
            lines.push_back(IDETheme::keyword("enum class ") + IDETheme::type("DebugDirective") + IDETheme::punctuation(" : uint8_t {"));

            for (int i = 0; i < maxOp; ++i) {
                std::string line = "    " + options[i] + ",";
                if (i == selected) {
                    lines.push_back(std::string(IDETheme::COLOR_ACTIVE_TAB) + "  > " + line + " <" + std::string(IDETheme::COLOR_RESET));
                } else {
                    lines.push_back("    " + IDETheme::punctuation(line));
                }
            }
            lines.push_back(IDETheme::punctuation("};"));

            std::vector<std::string> tabs = {
                "Debugger.cpp",
                "ProcessThread.sys"
            };
            auto editorView = IDETheme::renderEditorView(tabs, 0, "// src/Core/Debugger.cpp > void Debugger::onBreakpoint()", lines, width, height, "[W/S] Selecionar | [ENTER] Executar Diretiva | [ESC] Retomar");

            for (const auto& l : editorView) std::cout << l << "\n";
            std::cout << "\033[J" << std::flush;

            char key = InputControl::readKey();
            if (key == 'w' || key == 'W' || key == 72) {
                selected = (selected - 1 + maxOp) % maxOp;
            } else if (key == 's' || key == 'S' || key == 80) {
                selected = (selected + 1) % maxOp;
            } else if (key == '\r' || key == '\n') {
                return selected;
            } else if (key == 27) {
                return 0; // Retomar
            }
        }
    }
}

int IDEPauseScreen::renderMenuPause() {
    std::vector<std::string> options = {
        "RESUME_GAME_THREAD",
        "OPEN_SETTINGS_CONFIG",
        "RETURN_TO_MAIN_MENU",
        "TERMINATE_PROCESS"
    };
    return renderMenuIDE("PauseInterrupt", options);
}

int IDEPauseScreen::renderSettingsMenu(Character* /*player*/) {
    std::vector<std::string> options = {
        "APPEARANCE_AND_THEME",
        "MOUSE_AND_SENSITIVITY",
        "RESTORE_DEFAULTS",
        "RETURN_TO_PAUSE_MENU"
    };
    return renderMenuIDE("SettingsConfig", options);
}

int IDEPauseScreen::renderMenuAppearance(Character* /*player*/) {
    std::vector<std::string> options = {
        "TOGGLE_IDE_PERSPECTIVE",
        "BACKGROUND_COLOR_PALETTE",
        "RETURN_TO_SETTINGS"
    };
    return renderMenuIDE("AppearanceSubsystem", options);
}

int IDEPauseScreen::renderMenuBackground(int /*colorBackgroundCurrentIndex*/) {
    std::vector<std::string> options = {
        "THEME_DEFAULT_DARK",
        "THEME_MONOKAI",
        "THEME_SOLARIZED",
        "THEME_TERMINAL_BLACK",
        "RETURN_PREVIOUS"
    };
    return renderMenuIDE("BackgroundPalette", options);
}

int IDEPauseScreen::renderMenuSensitivity(int /*percentX*/, int /*percentY*/) {
    std::vector<std::string> options = {
        "INCREMENT_SENSITIVITY_X",
        "DECREMENT_SENSITIVITY_X",
        "INCREMENT_SENSITIVITY_Y",
        "DECREMENT_SENSITIVITY_Y",
        "CONFIRM_AND_RETURN"
    };
    return renderMenuIDE("InputSensitivity", options);
}
