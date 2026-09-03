#include "UI/Renderers/IDE/IDEScreens/Menu/IDEOpeningScreen.h"
#include <iostream>
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "UI/Renderers/IDE/IDETheme.h"

void IDEOpeningScreen::display() {
    std::cout << "\033[?25l";
    Appearance::clearScreen();

    int width = Appearance::getTerminalWidth();
    int height = Appearance::getTerminalHeight();

    std::vector<std::string> lines;
    lines.push_back(IDETheme::preprocessor("#pragma once"));
    lines.push_back(IDETheme::comment("// [BOOT] Inicializacao do Runtime C++23 da Engine..."));
    lines.push_back("");
    lines.push_back(IDETheme::type("std") + IDETheme::punctuation("::") + IDETheme::function("println") + IDETheme::punctuation("(") + IDETheme::stringLiteral("Carregando modulos principais...") + IDETheme::punctuation(");"));
    lines.push_back(IDETheme::type("std") + IDETheme::punctuation("::") + IDETheme::function("println") + IDETheme::punctuation("(") + IDETheme::stringLiteral("[OK] Modulo Grafico Inicializado") + IDETheme::punctuation(");"));
    lines.push_back(IDETheme::type("std") + IDETheme::punctuation("::") + IDETheme::function("println") + IDETheme::punctuation("(") + IDETheme::stringLiteral("[OK] Sistema de Input Pronto") + IDETheme::punctuation(");"));
    lines.push_back(IDETheme::type("std") + IDETheme::punctuation("::") + IDETheme::function("println") + IDETheme::punctuation("(") + IDETheme::stringLiteral("[OK] Motor de Perspectiva (IDE) Ativo") + IDETheme::punctuation(");"));
    lines.push_back("");
    lines.push_back(IDETheme::type("Engine::Core") + IDETheme::punctuation("::") + IDETheme::function("initialize") + IDETheme::punctuation("();"));
    lines.push_back(IDETheme::type("Engine::Core") + IDETheme::punctuation("::") + IDETheme::function("runMainLoop") + IDETheme::punctuation("();"));

    std::vector<std::string> tabs = {
        "BootLoader.sys",
        "KernelInit.cpp"
    };
    auto editorView = IDETheme::renderEditorView(tabs, 0, "// sys/boot/BootLoader.sys > void startRuntime()", lines, width, height, "[PRESSIONE QUALQUER TECLA PARA INICIAR]");

    for (const auto& l : editorView) std::cout << l << "\n";
    std::cout << "\033[J" << std::flush;

    InputControl::clearBuffer();
    InputControl::readKey();
    InputControl::clearBuffer();
}