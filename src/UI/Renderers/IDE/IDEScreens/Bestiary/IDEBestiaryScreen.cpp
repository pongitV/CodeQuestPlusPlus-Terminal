#include "UI/Renderers/IDE/IDEScreens/Bestiary/IDEBestiaryScreen.h"
#include "UI/Renderers/IDE/IDETheme.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "Domain/Characters/Character.h"
#include "Domain/Characters/Races/BaseRace.h"
#include <iostream>
#include <vector>

void IDEBestiaryScreen::display(const std::vector<Character*>& enemies) {
    int width = Appearance::getTerminalWidth();
    int height = Appearance::getTerminalHeight();

    if (enemies.empty()) {
        Appearance::clearScreen();
        std::vector<std::string> emptyLines;
        emptyLines.push_back(IDETheme::comment("// [CATALOGO_VAZIO: Nenhum header de monster detectado no disco]"));
        emptyLines.push_back(IDETheme::keyword("namespace ") + IDETheme::type("Monsters") + IDETheme::punctuation(" { /* 0 entries */ }"));
        emptyLines.push_back("");
        emptyLines.push_back(IDETheme::comment("// [ENTER] Retornar..."));

        std::vector<std::string> tabs = { "BestiaryIndex.hpp" };
        auto editorView = IDETheme::renderEditorView(tabs, 0, "// include/Monsters/BestiaryIndex.hpp", emptyLines, width, height);
        for (const auto& l : editorView) std::cout << l << "\n";
        std::cout << "\033[J" << std::flush;
        InputControl::waitForEnter();
        return;
    }

    int selection = 0;
    while (true) {
        Appearance::clearScreen();
        width = Appearance::getTerminalWidth();
        height = Appearance::getTerminalHeight();

        std::vector<std::string> codeLines;
        codeLines.push_back(IDETheme::preprocessor("#pragma once"));
        codeLines.push_back(IDETheme::comment("// Headers das entities de monster detectados no disco:"));
        codeLines.push_back("");

        for (size_t i = 0; i < enemies.size(); ++i) {
            std::string name = enemies[i]->getName();
            std::string line = "#include <Monsters/" + name + ".hpp>";
            if (static_cast<int>(i) == selection) {
                codeLines.push_back(std::string(IDETheme::COLOR_ACTIVE_TAB) + "> " + line + " <" + std::string(IDETheme::COLOR_RESET));
            } else {
                codeLines.push_back("  " + IDETheme::preprocessor(line));
            }
        }

        std::vector<std::string> tabs = {
            "BestiaryIndex.hpp",
            "MonsterRegistry.cpp"
        };
        auto editorView = IDETheme::renderEditorView(tabs, 0, "// include/Monsters/BestiaryIndex.hpp", codeLines, width, height, "[W/S] Navegar | [ENTER] Inspecionar Classe | [ESC/0] Fechar");

        for (const auto& l : editorView) std::cout << l << "\n";
        std::cout << "\033[J" << std::flush;

        char key = InputControl::readKey();
        if (key == 'w' || key == 'W' || key == 72) {
            selection = (selection - 1 + static_cast<int>(enemies.size())) % static_cast<int>(enemies.size());
        } else if (key == 's' || key == 'S' || key == 80) {
            selection = (selection + 1) % static_cast<int>(enemies.size());
        } else if (key == '\r' || key == '\n') {
            displayDetail(enemies[selection]);
        } else if (key == '0' || key == 27) {
            break;
        }
    }
}

void IDEBestiaryScreen::displayDetail(Character* enemy) {
    if (!enemy) return;

    Appearance::clearScreen();
    int width = Appearance::getTerminalWidth();
    int height = Appearance::getTerminalHeight();

    std::string className = enemy->getRace() ? enemy->getRace()->getRaceName() : "Monster";
    std::vector<std::string> tabs = {
        className + ".hpp",
        "Disassembly.asm"
    };

    std::vector<std::string> lines;
    lines.push_back(IDETheme::preprocessor("#pragma once"));
    lines.push_back(IDETheme::preprocessor("#include \"MonsterBase.hpp\""));
    lines.push_back("");
    lines.push_back(IDETheme::keyword("namespace ") + IDETheme::type("Domain::Entities::Monsters") + IDETheme::punctuation(" {"));
    lines.push_back("");
    lines.push_back(IDETheme::keyword("class ") + IDETheme::type(className) + IDETheme::punctuation(" final : public ") + IDETheme::type("MonsterBase") + IDETheme::punctuation(" {"));
    lines.push_back(IDETheme::keyword("public:"));
    lines.push_back("    " + IDETheme::type("const char*") + " " + IDETheme::variable("identifier") + "  = " + IDETheme::stringLiteral(enemy->getName()) + ";");
    lines.push_back("    " + IDETheme::type("int") + "         " + IDETheme::variable("maxHealth") + "   = " + IDETheme::number(enemy->getMaxHealth()) + ";");
    lines.push_back("    " + IDETheme::type("int") + "         " + IDETheme::variable("strength") + "    = " + IDETheme::number(enemy->getStrength()) + ";");
    lines.push_back("    " + IDETheme::type("int") + "         " + IDETheme::variable("resistance") + "  = " + IDETheme::number(enemy->getResistance()) + ";");
    lines.push_back("");
    lines.push_back("    " + IDETheme::keyword("void ") + IDETheme::function("onSpawn") + IDETheme::punctuation("() override;"));
    lines.push_back("    " + IDETheme::keyword("void ") + IDETheme::function("onDeath") + IDETheme::punctuation("() override;"));
    lines.push_back(IDETheme::punctuation("};"));
    lines.push_back("");
    lines.push_back(IDETheme::punctuation("} // namespace Domain::Entities::Monsters"));

    auto editorView = IDETheme::renderEditorView(tabs, 0, "// include/Monsters/" + className + ".hpp", lines, width, height, "[ENTER] Retornar ao Catalogo");

    for (const auto& l : editorView) std::cout << l << "\n";
    std::cout << "\033[J" << std::flush;

    InputControl::waitForEnter();
}
