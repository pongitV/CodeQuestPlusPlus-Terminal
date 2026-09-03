#include "UI/Renderers/IDE/IDEScreens/Defeat/IDEDefeatScreen.h"
#include "UI/Renderers/IDE/IDETheme.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include <iostream>
#include <vector>

void IDEDefeatScreen::display(
    Character* /*currentPlayer*/,
    int /*obtainedGoldQuantity*/,
    int /*obtainedXpQuantity*/,
    int totalDamageCaused,
    int totalDamageReceived,
    int totalHealingReceived,
    int combatTurns
) {
    Appearance::clearScreen();
    int width = Appearance::getTerminalWidth();
    int height = Appearance::getTerminalHeight();

    std::vector<std::string> lines;
    lines.push_back(std::string(IDETheme::COLOR_FLASH_HIT) + "FATAL_EXCEPTION: PlayerLifeZeroException at PC 0x7ffd004a" + std::string(IDETheme::COLOR_RESET));
    lines.push_back("");
    lines.push_back(IDETheme::comment("// === DUMP DA CALL STACK ==="));
    lines.push_back("[frame 0] " + IDETheme::function("Combat::checkVictoryOrDefeatCondition") + "() at Combat.cpp:241");
    lines.push_back("[frame 1] " + IDETheme::function("Combat::executeTurnForAllEnemies") + "() at Combat.cpp:239");
    lines.push_back("[frame 2] " + IDETheme::function("Hero::onHit") + "(damage: " + std::to_string(totalDamageReceived) + ") -> hp <= 0;");
    lines.push_back("");
    lines.push_back(IDETheme::comment("// Telemetria pós-mortem da execution:"));
    lines.push_back("  - turnsExecuted:   " + std::to_string(combatTurns));
    lines.push_back("  - totalDmgDealt:   " + std::to_string(totalDamageCaused));
    lines.push_back("  - totalDmgTaken:   " + std::to_string(totalDamageReceived));
    lines.push_back("  - totalHealSum:    " + std::to_string(totalHealingReceived));

    std::vector<std::string> tabs = {
        "CrashDump.dmp",
        "StackTrace.log"
    };
    auto editorView = IDETheme::renderEditorView(tabs, 0, "// trace/dumps/CrashDump.dmp > FATAL_EXCEPTION", lines, width, height, "[ENTER] Recarregar ponto de restauracao");

    for (const auto& l : editorView) std::cout << l << "\n";
    std::cout << "\033[J" << std::flush;

    InputControl::waitForEnter();
}
