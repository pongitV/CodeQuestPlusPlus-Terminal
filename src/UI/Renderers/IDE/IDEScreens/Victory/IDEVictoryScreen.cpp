#include "UI/Renderers/IDE/IDEScreens/Victory/IDEVictoryScreen.h"
#include "UI/Renderers/IDE/IDETheme.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include <iostream>
#include <vector>

void IDEVictoryScreen::display(
    Character* /*currentPlayer*/,
    int obtainedGoldQuantity,
    int obtainedXpQuantity,
    int totalDamageCaused,
    int totalDamageReceived,
    int totalHealingReceived,
    int combatTurns,
    const std::vector<std::string>& /*enemiesDefeated*/,
    int parriesPerfect,
    int biggerDamage,
    int /*parriesTempted*/,
    int /*parriesEffective*/,
    int itemsConsumed,
    const std::vector<std::pair<std::string, int>>& dropsUnique,
    bool canRiseLevel,
    const std::vector<std::string>& /*newDiscoveries*/,
    const std::string& titleMap
) {
    Appearance::clearScreen();
    int width = Appearance::getTerminalWidth();
    int height = Appearance::getTerminalHeight();

    std::vector<std::string> lines;
    lines.push_back(IDETheme::comment("// Término de process com status: 0x00 (STATUS_SUCCESS)"));
    lines.push_back(IDETheme::keyword("struct ") + IDETheme::type("ExecutionReport") + IDETheme::punctuation(" {"));
    lines.push_back("    " + IDETheme::type("const char*") + " " + IDETheme::variable("sector") + "        = " + IDETheme::stringLiteral(titleMap) + ";");
    lines.push_back("    " + IDETheme::type("uint32_t") + "    " + IDETheme::variable("turnsRun") + "      = " + IDETheme::number(combatTurns) + ";");
    lines.push_back("    " + IDETheme::type("int") + "         " + IDETheme::variable("damageDealt") + "   = " + IDETheme::number(totalDamageCaused) + ";");
    lines.push_back("    " + IDETheme::type("int") + "         " + IDETheme::variable("damageTaken") + "   = " + IDETheme::number(totalDamageReceived) + ";");
    lines.push_back("    " + IDETheme::type("int") + "         " + IDETheme::variable("healingSum") + "    = " + IDETheme::number(totalHealingReceived) + ";");
    lines.push_back("    " + IDETheme::type("int") + "         " + IDETheme::variable("maxHitPeak") + "    = " + IDETheme::number(biggerDamage) + ";");
    lines.push_back("    " + IDETheme::type("int") + "         " + IDETheme::variable("parries") + "       = " + IDETheme::number(parriesPerfect) + ";");
    lines.push_back("    " + IDETheme::type("int") + "         " + IDETheme::variable("itemsUsed") + "     = " + IDETheme::number(itemsConsumed) + ";");
    lines.push_back(IDETheme::punctuation("} report;"));
    lines.push_back("");

    lines.push_back(IDETheme::comment("// Alocações da heap commitadas na instance do player:"));
    lines.push_back(IDETheme::variable("hero") + IDETheme::punctuation("->") + IDETheme::function("addGold") + IDETheme::punctuation("(") + IDETheme::number(obtainedGoldQuantity) + IDETheme::punctuation(");"));
    lines.push_back(IDETheme::variable("hero") + IDETheme::punctuation("->") + IDETheme::function("addXp") + IDETheme::punctuation("(") + IDETheme::number(obtainedXpQuantity) + IDETheme::punctuation(");"));

    for (const auto& [item, count] : dropsUnique) {
        lines.push_back(IDETheme::variable("hero.inventory") + IDETheme::punctuation("->") + IDETheme::function("push") + IDETheme::punctuation("(") + IDETheme::stringLiteral(item) + IDETheme::punctuation(", count: ") + IDETheme::number(count) + IDETheme::punctuation(");"));
    }

    if (canRiseLevel) {
        lines.push_back("");
        lines.push_back(IDETheme::comment("// [ASSERT_TRUE] hero->canLevelUp() == true. Execute hero->levelUp() na ficha de attributes."));
    }

    std::vector<std::string> tabs = {
        "ProcessExit.cpp",
        "MemoryGC.trace"
    };
    auto editorView = IDETheme::renderEditorView(tabs, 0, "// src/Systems/Execution/ProcessExit.cpp > ExecutionReport", lines, width, height, "[ENTER] Retornar ao loop de exploracao");

    for (const auto& l : editorView) std::cout << l << "\n";
    std::cout << "\033[J" << std::flush;

    InputControl::waitForEnter();
}
