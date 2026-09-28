#include "UI/Renderers/IDE/IDEScreens/Menu/IDENameScreen.h"
#include "UI/Renderers/IDE/IDEScreens/Menu/IDEMenuScreen.h"
#include "UI/Renderers/IDE/IDETheme.h"
#include <iostream>
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"

NameScreen::Result IDENameScreen::display() {
    std::string name;

    while (true) {
        std::cout << "\033[?25l";
        Appearance::clearScreen();

        int width = Appearance::getTerminalWidth();
        int height = Appearance::getTerminalHeight();

        std::vector<std::string> blockCentral;
        blockCentral.push_back(IDETheme::preprocessor("#pragma once"));
        blockCentral.push_back(IDETheme::comment("// Inicializacao da instancia do Heroi no jogo..."));
        blockCentral.push_back(IDETheme::keyword("namespace ") + IDETheme::type("Domain::Characters") + IDETheme::punctuation(" {"));
        blockCentral.push_back("");
        blockCentral.push_back("    " + IDETheme::type("std::string") + " " + IDETheme::variable("heroIdentifier") + IDETheme::punctuation(";"));

        if (!name.empty() && name.length() > 20) {
            blockCentral.push_back("    " + IDETheme::type("std::cerr") + IDETheme::punctuation(" << ") + IDETheme::stringLiteral("Erro: Identificador excede 20 caracteres.") + IDETheme::punctuation(" << std::endl;"));
        }

        blockCentral.push_back("    " + IDETheme::type("std::cin") + IDETheme::punctuation(" >> ") + IDETheme::variable("heroIdentifier") + IDETheme::punctuation(";") + IDETheme::comment(" // Digite o nome ou '0' para voltar"));
        blockCentral.push_back("");
        blockCentral.push_back(IDETheme::punctuation("} // namespace Domain::Characters"));

        std::vector<std::string> tabs = {
            "PlayerProfile.hpp",
            "InputReader.sys"
        };
        auto editorView = IDETheme::renderEditorView(tabs, 0, "// src/Domain/Characters/PlayerProfile.hpp > heroIdentifier", blockCentral, width, height, "Prompt: Digite o nome do personagem e pressione [ENTER]");

        for (const auto& l : editorView) std::cout << l << "\n";
        std::cout << "\033[J" << std::flush;

        std::string promptStr = "> ";
        int spacesX = std::max(0, (width - 30) / 2);
        std::string pad(spacesX, ' ');
        std::cout << pad << promptStr;

        std::cout << "\033[?25h";
        name = InputControl::readEntryProtected();

        if (name == "0") {
            NameScreen::Result r;
            r.returned = true;
            return r;
        }

        if (!name.empty() && name.length() <= 20) {
            break;
        }
    }

    std::cout << "\033[?25l";

    std::vector<std::string> info = { "O identificador \"" + name + "\" sera persistido na heap." };
    bool confirmed = IDEMenuScreen::displayChooseConfirmationWithArtSideBySide("NOME", name, info, {});

    if (!confirmed) {
        return display();
    }

    NameScreen::Result r;
    r.name = name;
    return r;
}
