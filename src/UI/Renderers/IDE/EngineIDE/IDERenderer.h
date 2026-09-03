#pragma once

#include "UI/PerspectiveRenderer.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "Core/Utils/AnsiFormatter.h"
#include "UI/Renderers/IDE/IDETheme.h"
#include <iostream>
#include <vector>
#include <string>

class IDERenderer : public PerspectiveRenderer {
public:
    std::vector<std::string> formatIDEText(const std::vector<std::string>& text) {
        return AnsiFormatter::formatIDEText(text);
    }

    std::vector<std::string> formatIDEArt(const std::vector<std::string>& art) {
        return AnsiFormatter::formatIDEArt(art);
    }

    std::string formatIDETitle(const std::string& title) {
        return AnsiFormatter::formatIDETitle(title);
    }

    void displayPopup(const std::string& title, const std::vector<std::string>& lines, Color /*colorHeader*/, const std::vector<std::string>& /*logoArt*/ = {}) override {
        Appearance::clearScreen();
        int termW = Appearance::getTerminalWidth();
        int termH = Appearance::getTerminalHeight();

        std::vector<std::string> codeLines;
        codeLines.push_back(IDETheme::comment("// Canal de rpc: DialogueStream::SyncRpc"));
        codeLines.push_back(IDETheme::keyword("namespace ") + IDETheme::type("Engine::Network") + IDETheme::punctuation(" {"));
        codeLines.push_back(IDETheme::keyword("struct ") + IDETheme::type("DialoguePayload") + IDETheme::punctuation(" {"));
        codeLines.push_back("    " + IDETheme::type("const char*") + " " + IDETheme::variable("interlocutor") + " = " + IDETheme::stringLiteral(title) + ";");
        for (size_t i = 0; i < lines.size(); ++i) {
            codeLines.push_back("    " + IDETheme::type("const char*") + " " + IDETheme::variable("payload_" + std::to_string(i)) + " = " + IDETheme::stringLiteral(lines[i]) + ";");
        }
        codeLines.push_back(IDETheme::punctuation("};"));
        codeLines.push_back(IDETheme::punctuation("} // namespace Engine::Network"));

        std::vector<std::string> tabs = { "DialogueStream.rpc", "EventLog.sys" };
        auto editorView = IDETheme::renderEditorView(tabs, 0, "// rpc/DialogueStream.rpc > " + title, codeLines, termW, termH, "[ENTER] Continuar");

        for (const auto& l : editorView) {
            std::cout << l << "\n";
        }
        std::cout << "\033[J" << std::flush;
        InputControl::waitForEnter();
    }

    void startPopupInteraction() override {}

    int readMenuSelectionInPopup(
        const std::string& title,
        const std::vector<std::string>& descriptions,
        const std::vector<std::string>& options,
        Color /*colorHeader*/,
        const std::vector<std::string>& /*logoArt*/ = {},
        bool /*returnEnabled*/ = true
    ) override {
        if (options.empty()) return -1;

        int selected = 0;
        int total = static_cast<int>(options.size());
        InputControl::clearBuffer();

        while (true) {
            Appearance::clearScreen();
            int termW = Appearance::getTerminalWidth();
            int termH = Appearance::getTerminalHeight();

            std::vector<std::string> codeLines;
            codeLines.push_back(IDETheme::comment("// Contexto de interação: " + title));
            for (const auto& d : descriptions) {
                codeLines.push_back(IDETheme::comment("// " + d));
            }
            codeLines.push_back("");
            codeLines.push_back(IDETheme::keyword("enum class ") + IDETheme::type("InteractionDirective") + IDETheme::punctuation(" : uint8_t {"));
            for (int i = 0; i < total; ++i) {
                std::string line = "    DIRECTIVE_" + std::to_string(i) + " = 0x" + (i < 16 ? "0" : "") + std::to_string(i) + ", // " + options[i];
                if (i == selected) {
                    codeLines.push_back(std::string(IDETheme::COLOR_ACTIVE_TAB) + "  > " + line + " <" + std::string(IDETheme::COLOR_RESET));
                } else {
                    codeLines.push_back("    " + IDETheme::punctuation(line));
                }
            }
            codeLines.push_back(IDETheme::punctuation("};"));
            codeLines.push_back("");
            codeLines.push_back(IDETheme::type("InteractionDirective") + " " + IDETheme::variable("directive") + " = " + IDETheme::type("InteractionDirective::") + "DIRECTIVE_" + std::to_string(selected) + ";");

            std::vector<std::string> tabs = {
                "InteractionDispatcher.sys",
                "ActionDirective.hpp"
            };
            auto editorView = IDETheme::renderEditorView(tabs, 0, "// src/Systems/Interaction/InteractionDispatcher.sys > " + title, codeLines, termW, termH, "[W/S] Selecionar Diretiva | [ENTER] Confirmar");

            for (const auto& l : editorView) {
                std::cout << l << "\n";
            }
            std::cout << "\033[J" << std::flush;

            char key = InputControl::readKey();
            if (key == 'w' || key == 'W' || key == 72) {
                selected = (selected - 1 + total) % total;
            } else if (key == 's' || key == 'S' || key == 80) {
                selected = (selected + 1) % total;
            } else if (key == '\r' || key == '\n') {
                return selected;
            }
        }
    }

    void clearScreen() override { Appearance::clearScreen(); }

    void displayTextPanel(const std::string& text, Color color) override {
        Appearance::displayTextPanel(AnsiFormatter::formatYESDText(text), color);
    }
};

using GORenderer = IDERenderer;
