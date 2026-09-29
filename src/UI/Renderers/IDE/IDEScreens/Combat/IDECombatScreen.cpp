#include "UI/Renderers/IDE/IDEScreens/Combat/IDECombatScreen.h"
#include "UI/Renderers/IDE/IDETheme.h"
#include "UI/PerspectiveManager.h"
#include "UI/Screens/Combat/CombatScreen.h"
#include "Domain/Characters/Character.h"
#include "Domain/Characters/Races/BaseRace.h"
#include "Systems/Inventory/Inventory.h"
#include "Domain/Items/Item.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <chrono>
#include <algorithm>

namespace {
    std::string sanitizeIdentifier(const std::string& name) {
        std::string res = name;
        for (char& c : res) {
            if (!std::isalnum(static_cast<unsigned char>(c))) c = '_';
        }
        return res.empty() ? "Monster" : res;
    }
}

void IDECombatScreen::renderCombatFrame(
    const std::string& combatTitle,
    const std::vector<Character*>& enemies,
    Character* currentPlayer,
    const std::vector<std::string>& activePanelLines,
    Character* targetAnimation,
    int frameAnimation,
    bool isHealing,
    bool isDeath,
    int damageAnimation
) {
    int width = Appearance::getTerminalWidth();
    int height = Appearance::getTerminalHeight();

    std::vector<std::string> allCenteredLines;

    // 1. Horde de Inimigos declarados como classes C++ vivas
    if (!enemies.empty()) {
        std::vector<std::vector<std::string>> hordeBlocks;
        for (size_t i = 0; i < enemies.size(); ++i) {
            Character* enemy = enemies[i];
            std::string enemyClass = enemy->getRace() ? sanitizeIdentifier(enemy->getRace()->getRaceName()) : "Monster";
            std::string varName = "enemy_" + std::to_string(i);

            std::vector<std::string> block;
            block.push_back(IDETheme::keyword("class ") + IDETheme::type(enemyClass) + IDETheme::punctuation(" final : public ") + IDETheme::type("Monster") + IDETheme::punctuation(" {"));
            block.push_back(IDETheme::keyword("public:"));
            block.push_back("    " + IDETheme::keyword("struct ") + IDETheme::type("Vitals") + IDETheme::punctuation(" {"));

            std::string hpLine = "        " + IDETheme::type("int") + " " + IDETheme::variable("hp") + IDETheme::punctuation(" = ");
            if (enemy == targetAnimation && damageAnimation > 0 && frameAnimation > 0) {
                if (isHealing) {
                    hpLine += std::string(IDETheme::COLOR_FLASH_CURE) + std::to_string(enemy->getHealth()) + " [+" + std::to_string(damageAnimation) + "!]" + std::string(IDETheme::COLOR_RESET);
                } else {
                    hpLine += std::string(IDETheme::COLOR_FLASH_HIT) + std::to_string(enemy->getHealth()) + " [-" + std::to_string(damageAnimation) + "!]" + std::string(IDETheme::COLOR_RESET);
                }
            } else {
                hpLine += IDETheme::number(enemy->getHealth());
            }
            hpLine += IDETheme::punctuation("; ") + IDETheme::comment(IDETheme::renderCodeHealthBar(enemy->getHealth(), enemy->getMaxHealth(), 6));
            block.push_back(hpLine);
            block.push_back("    " + IDETheme::punctuation("} vitals;"));

            block.push_back("    " + IDETheme::keyword("struct ") + IDETheme::type("Stats") + IDETheme::punctuation(" {"));
            block.push_back("        " + IDETheme::type("int") + " " + IDETheme::variable("strength") + IDETheme::punctuation("   = ") + IDETheme::number(enemy->getStrength()) + IDETheme::punctuation(";"));
            block.push_back("        " + IDETheme::type("int") + " " + IDETheme::variable("resistance") + IDETheme::punctuation(" = ") + IDETheme::number(enemy->getResistance()) + IDETheme::punctuation(";"));
            block.push_back("    " + IDETheme::punctuation("} stats;"));

            if (enemy == targetAnimation && isDeath) {
                block.push_back("    " + std::string(IDETheme::COLOR_FLASH_HIT) + "// [DESTRUCTOR ~" + enemyClass + "()]" + std::string(IDETheme::COLOR_RESET));
            } else {
                block.push_back("    " + IDETheme::type("void") + " " + IDETheme::function("onHit") + IDETheme::punctuation("(") + IDETheme::type("int") + " " + IDETheme::variable("dmg") + IDETheme::punctuation(");"));
            }
            block.push_back(IDETheme::punctuation("} ") + IDETheme::variable(varName) + IDETheme::punctuation(";"));
            hordeBlocks.push_back(block);
        }

        size_t maxRows = 0;
        for (const auto& b : hordeBlocks) maxRows = std::max(maxRows, b.size());

        int naturalBlockWidth = 0;
        for (const auto& b : hordeBlocks) {
            for (const auto& line : b) {
                naturalBlockWidth = std::max(naturalBlockWidth, Appearance::getVisualLength(line));
            }
        }
        naturalBlockWidth = std::max(36, naturalBlockWidth + 1);

        std::vector<std::string> rawHordeLines;
        for (size_t row = 0; row < maxRows; ++row) {
            std::string lineOut = "";
            for (size_t col = 0; col < hordeBlocks.size(); ++col) {
                const auto& b = hordeBlocks[col];
                std::string cell = (row < b.size()) ? b[row] : "";
                int vLen = Appearance::getVisualLength(cell);
                lineOut += cell;
                if (col + 1 < hordeBlocks.size()) {
                    if (vLen < naturalBlockWidth) lineOut += std::string(naturalBlockWidth - vLen, ' ');
                    lineOut += "    ";
                }
            }
            rawHordeLines.push_back(lineOut);
        }

        auto centeredHorde = IDETheme::centerBlock(rawHordeLines, width);
        allCenteredLines.insert(allCenteredLines.end(), centeredHorde.begin(), centeredHorde.end());
        allCenteredLines.push_back("");
    }

    // 2. Status do Jogador (HeroState)
    auto playerStatus = getPlayerStatusBarLines(currentPlayer, Color::RESET, damageAnimation, frameAnimation, isHealing);
    auto centeredPlayer = IDETheme::centerBlock(playerStatus, width);
    allCenteredLines.insert(allCenteredLines.end(), centeredPlayer.begin(), centeredPlayer.end());

    // 3. Mensagens fixas
    if (!m_fixedMessages.empty()) {
        allCenteredLines.push_back("");
        std::vector<std::string> fixLines;
        for (const auto& msg : m_fixedMessages) {
            fixLines.push_back("// " + msg);
        }
        auto centeredFixed = IDETheme::centerBlock(fixLines, width);
        allCenteredLines.insert(allCenteredLines.end(), centeredFixed.begin(), centeredFixed.end());
    }

    // 4. Aguarda confirmacao do jogador com Enter
    if (!InputControl::enterPromptText.empty()) {
        allCenteredLines.push_back("");
        std::vector<std::string> pLines = { "// [WAIT] " + InputControl::enterPromptText };
        auto centeredPrompt = IDETheme::centerBlock(pLines, width);
        allCenteredLines.insert(allCenteredLines.end(), centeredPrompt.begin(), centeredPrompt.end());
    }

    // 5. Painel interativo ativo (Menu de Acoes, Alvo, Escudo, Trace log)
    if (!activePanelLines.empty()) {
        allCenteredLines.push_back("");
        auto centeredPanel = IDETheme::centerBlock(activePanelLines, width);
        allCenteredLines.insert(allCenteredLines.end(), centeredPanel.begin(), centeredPanel.end());
    }

    // 6. Monta o editor com abas fixadas no topo (linha 0) e conteudo centralizado no viewport
    std::vector<std::string> tabs = {
        "CombatSession.cpp",
        "CallStack.trace",
        "EntityInspector.hpp"
    };
    std::string threadName = combatTitle.empty() ? "CombatSession::runTick" : combatTitle;
    std::string breadcrumb = "// src/Systems/Combat/CombatSession.cpp > void " + threadName + "()";

    int reservedLines = 2; // Linha 0 (abas) + Linha 1 (breadcrumb)
    int availableH = std::max(0, height - reservedLines);
    int topPadding = IDETheme::calculateTopPadding(static_cast<int>(allCenteredLines.size()), availableH);

    std::ostringstream frame;
    frame << "\033[H"; // Cursor home absoluto
    frame << IDETheme::renderTabBar(tabs, 0, width) << "\033[K\n";
    frame << IDETheme::comment(breadcrumb) << "\033[K\n";

    for (int i = 0; i < topPadding; ++i) {
        frame << "\033[K\n";
    }
    for (const auto& l : allCenteredLines) {
        frame << l << "\033[K\n";
    }
    frame << "\033[J"; // Limpeza total abaixo

    std::cout << frame.str() << std::flush;
}

void IDECombatScreen::displayLogoForCombatScreen(const std::string& screenTitle, bool /*animar*/) {
    int width = Appearance::getTerminalWidth();
    std::vector<std::string> tabs = {
        "CombatSession.cpp",
        "CallStack.trace",
        "EntityInspector.hpp"
    };
    std::cout << IDETheme::renderTabBar(tabs, 0, width) << "\n";
    std::string title = screenTitle.empty() ? "COMBAT_ROUTINE" : screenTitle;
    std::cout << IDETheme::comment("// src/Systems/Combat/CombatSession.cpp > " + title) << "\n\n";
}

void IDECombatScreen::animateCombatIntro(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer) {
    Appearance::clearScreen();
    int width = Appearance::getTerminalWidth();
    int height = Appearance::getTerminalHeight();
    
    std::vector<std::string> lines;
    lines.push_back(IDETheme::comment("// Alocação dinâmica da session de combat na heap: " + title));
    lines.push_back(IDETheme::keyword("auto* ") + IDETheme::variable("session") + IDETheme::punctuation(" = ")
                  + IDETheme::keyword("new ") + IDETheme::type("CombatSession") + IDETheme::punctuation("();"));
    lines.push_back("");

    if (currentPlayer) {
        lines.push_back(IDETheme::keyword("auto* ") + IDETheme::variable("hero") + IDETheme::punctuation(" = ")
                      + IDETheme::keyword("new ") + IDETheme::type("Hero") + IDETheme::punctuation("(")
                      + IDETheme::stringLiteral(currentPlayer->getName()) + IDETheme::punctuation("); // HP: ")
                      + IDETheme::number(currentPlayer->getHealth()));
    }

    for (size_t i = 0; i < enemies.size(); ++i) {
        std::string enemyClass = enemies[i]->getRace() ? sanitizeIdentifier(enemies[i]->getRace()->getRaceName()) : "Monster";
        std::string instName = "enemy_" + std::to_string(i);
        lines.push_back(IDETheme::keyword("auto* ") + IDETheme::variable(instName) + IDETheme::punctuation(" = ")
                      + IDETheme::keyword("new ") + IDETheme::type(enemyClass) + IDETheme::punctuation("(); // HP: ")
                      + IDETheme::number(enemies[i]->getHealth()));
    }
    lines.push_back("");
    lines.push_back(IDETheme::comment("// [ENTER] Iniciar o loop de turnos..."));

    std::vector<std::string> tabs = { "CombatSession.cpp", "CallStack.trace" };
    auto editorView = IDETheme::renderEditorView(tabs, 0, "// src/Systems/Combat/CombatSession.cpp > init()", lines, width, height);

    std::ostringstream frame;
    frame << "\033[H";
    for (const auto& l : editorView) frame << l << "\033[K\n";
    frame << "\033[J";

    std::cout << frame.str() << std::flush;
    InputControl::clearBuffer();
    
    while (true) {
        if (InputControl::pressedKey()) {
            char c = InputControl::readKey();
            if (c == '\r' || c == '\n' || c == ' ') break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

std::vector<std::string> IDECombatScreen::getPlayerStatusBarLines(
    Character* currentPlayer,
    Color /*corDestaque*/,
    int damageAnimation,
    int frameAnimation,
    bool isHealing
) {
    std::vector<std::string> lines;
    if (!currentPlayer) return lines;

    std::string hpDisplay = IDETheme::renderCodeHealthBar(currentPlayer->getHealth(), currentPlayer->getMaxHealth(), 10);
    if (damageAnimation > 0 && frameAnimation > 0) {
        if (isHealing) {
            hpDisplay += " " + std::string(IDETheme::COLOR_FLASH_CURE) + " +" + std::to_string(damageAnimation) + " HP " + std::string(IDETheme::COLOR_RESET);
        } else {
            hpDisplay += " " + std::string(IDETheme::COLOR_FLASH_HIT) + " -" + std::to_string(damageAnimation) + " HP " + std::string(IDETheme::COLOR_RESET);
        }
    }

    lines.push_back(IDETheme::comment("// Instance ativa do player no frame:"));
    lines.push_back(IDETheme::keyword("class ") + IDETheme::type("Hero") + IDETheme::punctuation(" {"));
    lines.push_back(IDETheme::keyword("public:"));
    lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("int") + " " + IDETheme::variable("hp") + IDETheme::punctuation(" = ") + hpDisplay + IDETheme::punctuation(";"));
    int pGold = currentPlayer->getInventory() ? currentPlayer->getInventory()->getGold() : 0;
    lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("int") + " " + IDETheme::variable("gold") + IDETheme::punctuation(" = ") + IDETheme::number(pGold) + IDETheme::punctuation(";"));

    Item* weapon = currentPlayer->getWeapons();
    std::string wName = weapon ? weapon->getItemName() : "nullptr";
    lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("Weapon*") + " " + IDETheme::variable("eqWeapon") + IDETheme::punctuation(" = ") + (weapon ? ("&heap[" + IDETheme::stringLiteral(wName) + "]") : "nullptr") + IDETheme::punctuation(";"));

    lines.push_back(IDETheme::punctuation("} hero;"));
    return lines;
}

void IDECombatScreen::displayEnemyHordeSideBySide(
    const std::vector<Character*>& /*inimigos*/,
    Character* /*animacaoAlvo*/,
    int /*animacaoQuadro*/,
    bool /*ehCura*/,
    bool /*animarSurgimento*/,
    bool /*ehMorte*/,
    Item* /*armaAtacante*/,
    int /*animacaoDano*/,
    const std::vector<std::string>& /*animacaoDrops*/
) {
    // Delegado para renderCombatFrame()
}

void IDECombatScreen::animateDamageToEnemy(
    const std::string& combatTitle,
    const std::vector<Character*>& enemies,
    Character* targetAnimation,
    Character* attacker,
    Character* currentPlayer,
    const std::vector<Character*>& /*aliados*/,
    int damageAnimation
) {
    std::string targetName = targetAnimation ? targetAnimation->getName() : "enemy";
    std::string attackerName = attacker ? attacker->getName() : "hero";

    std::vector<std::string> traceLines;
    traceLines.push_back(IDETheme::comment("// --- TRACE DE EXECUTION (PC: 0x0040A1F0) ---"));
    traceLines.push_back(IDETheme::variable(attackerName) + IDETheme::punctuation("->")
                       + IDETheme::function("performAttack") + IDETheme::punctuation("(&")
                       + IDETheme::variable(targetName) + IDETheme::punctuation(");"));
    traceLines.push_back(IDETheme::variable(targetName) + IDETheme::punctuation(".")
                       + IDETheme::function("onHit") + IDETheme::punctuation("(")
                       + IDETheme::number(damageAnimation) + IDETheme::punctuation("); // ")
                       + IDETheme::variable(targetName + ".hp -= ") + IDETheme::number(damageAnimation));

    for (int frame = 1; frame <= 2; ++frame) {
        renderCombatFrame(combatTitle, enemies, currentPlayer, traceLines, targetAnimation, frame, false, false, damageAnimation);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void IDECombatScreen::animateCureToEnemy(
    const std::string& combatTitle,
    const std::vector<Character*>& enemies,
    Character* /*animacaoAlvo*/,
    Character* currentPlayer,
    const std::vector<Character*>& /*aliados*/,
    int /*animacaoCura*/
) {
    renderCombatFrame(combatTitle, enemies, currentPlayer, {});
}

void IDECombatScreen::animateDamageToPlayer(
    const std::string& combatTitle,
    const std::vector<Character*>& enemies,
    Character* /*animacaoAlvo*/,
    Character* currentPlayer,
    const std::vector<Character*>& /*aliados*/,
    bool isParry,
    int damageAnimation
) {
    std::vector<std::string> traceLines;
    traceLines.push_back(IDETheme::comment("// --- INTERRUPT DE DANO RECEBIDO ---"));
    if (isParry) {
        traceLines.push_back(IDETheme::variable("hero") + IDETheme::punctuation(".")
                           + IDETheme::function("executeParry") + IDETheme::punctuation("(); // Dano mitigado via postura de parry"));
    } else {
        traceLines.push_back(IDETheme::variable("hero") + IDETheme::punctuation(".")
                           + IDETheme::function("onHit") + IDETheme::punctuation("(")
                           + IDETheme::number(damageAnimation) + IDETheme::punctuation("); // hero.hp -= ")
                           + IDETheme::number(damageAnimation));
    }

    for (int frame = 1; frame <= 2; ++frame) {
        renderCombatFrame(combatTitle, enemies, currentPlayer, traceLines, nullptr, frame, false, false, damageAnimation);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void IDECombatScreen::animateCureToPlayer(
    const std::string& combatTitle,
    const std::vector<Character*>& enemies,
    Character* /*animacaoAlvo*/,
    Character* currentPlayer,
    const std::vector<Character*>& /*aliados*/,
    int /*animacaoCura*/
) {
    std::vector<std::string> traceLines;
    traceLines.push_back(IDETheme::comment("// --- ROTINA DE HEALING ---"));
    traceLines.push_back(IDETheme::variable("hero") + IDETheme::punctuation(".")
                       + IDETheme::function("restoreHealth") + IDETheme::punctuation("();"));
    renderCombatFrame(combatTitle, enemies, currentPlayer, traceLines);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

void IDECombatScreen::animateEnemyDeath(
    const std::string& combatTitle,
    const std::vector<Character*>& enemies,
    Character* enemyDead,
    Character* currentPlayer,
    const std::vector<Character*>& /*aliados*/,
    const std::vector<std::string>& drops
) {
    std::string deadName = enemyDead ? enemyDead->getName() : "Monster";

    std::vector<std::string> gcLines;
    gcLines.push_back(IDETheme::comment("// --- DESPACHO DE DESTRUCTOR (Garbage Collector) ---"));
    gcLines.push_back(IDETheme::keyword("delete ") + IDETheme::variable("&" + deadName) + IDETheme::punctuation(";")
                    + IDETheme::comment(" // Executando destructor virtual ~" + deadName + "()"));

    if (!drops.empty()) {
        gcLines.push_back(IDETheme::comment("// Alocando drops no vector heap do inventory:"));
        for (const auto& d : drops) {
            gcLines.push_back(IDETheme::variable("hero.inventory") + IDETheme::punctuation("->")
                            + IDETheme::function("push_back") + IDETheme::punctuation("(std::make_unique<")
                            + IDETheme::type("Item") + IDETheme::punctuation(">(")
                            + IDETheme::stringLiteral(d) + IDETheme::punctuation("));"));
        }
    }

    renderCombatFrame(combatTitle, enemies, currentPlayer, gcLines, enemyDead, 1, false, true, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void IDECombatScreen::updateScreenStatic(
    const std::string& combatTitle,
    const std::vector<Character*>& enemies,
    Character* currentPlayer,
    const std::vector<Character*>& /*aliados*/,
    bool /*animarEntrada*/,
    std::function<void(std::vector<std::string>&)> /*callbackSobreposicao*/
) {
    renderCombatFrame(combatTitle, enemies, currentPlayer, {});
}

void IDECombatScreen::addFixedMessage(const std::string& msg) {
    m_fixedMessages.push_back(msg);
}

void IDECombatScreen::cleanMessagesFixed() {
    m_fixedMessages.clear();
}

void IDECombatScreen::configureContext3D(bool /*modo3D*/, const std::vector<std::string>& /*matriz*/, float /*postX*/, float /*postY*/, float /*angulo*/, const std::string& /*titulo*/) {}
void IDECombatScreen::setShiftVisible(int /*deslocamento*/, const std::string& /*nome*/) {}
void IDECombatScreen::selectHUDAlly(Character* /*jogadorAtual*/, const std::vector<Character*>& /*aliados*/) {}

int IDECombatScreen::getPlayerAction(
    int currentTurn,
    Character* /*personagemAtuante*/,
    const std::vector<Character*>& enemies,
    Character* currentPlayer,
    const std::vector<Character*>& allies
) {
    struct MenuAction {
        int code;
        std::string enumCase;
        std::string callStmt;
    };

    std::vector<MenuAction> actionList = {
        {1, "Action::ATTACK",             "hero->performAttack(target);"},
        {2, "Action::DEFEND",             "hero->setDefensiveStance(shield);"},
        {3, "Action::CAST_SKILL",         "hero->executeClassSkill(skillId);"},
        {4, "Action::ACCESS_INVENTORY",   "hero->openInventoryHeap();"},
        {5, "Action::INSPECT_ATTRIBUTES", "hero->inspectMemorySheet();"},
        {6, "Action::OPEN_BESTIARY",      "BestiaryDatabase::openCatalog();"},
        {0, "Action::TOGGLE_VIEW",        "PerspectiveManager::toggle();"}
    };

    int selected = 0;
    int totalActions = static_cast<int>(actionList.size());
    InputControl::clearBuffer();

    while (true) {
        std::vector<std::string> menuLines;
        menuLines.push_back(IDETheme::comment("// Dispatcher de instruções do turno do Hero:"));
        menuLines.push_back(IDETheme::keyword("void ") + IDETheme::type("Hero::") + IDETheme::function("dispatchTurnAction") + IDETheme::punctuation("(CombatDispatcher* dispatcher) {"));
        menuLines.push_back("    " + IDETheme::keyword("switch ") + IDETheme::punctuation("(") + IDETheme::variable("instruction") + IDETheme::punctuation(") {"));

        for (int i = 0; i < totalActions; ++i) {
            std::string num = (i == totalActions - 1) ? "[V] " : "[" + std::to_string(i + 1) + "] ";
            std::string caseText = "case " + actionList[i].enumCase + ":";
            std::string fullLine = num + caseText + " " + actionList[i].callStmt;

            if (i == selected) {
                menuLines.push_back(std::string(IDETheme::COLOR_ACTIVE_TAB) + "        > " + fullLine + " <" + std::string(IDETheme::COLOR_RESET));
            } else {
                menuLines.push_back("          " + fullLine);
            }
        }
        menuLines.push_back("    " + IDETheme::punctuation("}"));
        menuLines.push_back(IDETheme::punctuation("}"));

        renderCombatFrame("Turno " + std::to_string(currentTurn), enemies, currentPlayer, menuLines);

        char key = InputControl::readKey();

        if (key == 'w' || key == 'W' || key == 72) {
            selected = (selected - 1 + totalActions) % totalActions;
        } else if (key == 's' || key == 'S' || key == 80) {
            selected = (selected + 1) % totalActions;
        } else if (key == '1') {
            return 1;
        } else if (key == '2') {
            return 2;
        } else if (key == '3') {
            return 3;
        } else if (key == '4') {
            return 4;
        } else if (key == '5') {
            return 5;
        } else if (key == '6') {
            return 6;
        } else if (key == 'v' || key == 'V' || (selected == totalActions - 1 && (key == '\r' || key == '\n'))) {
            while (GetAsyncKeyState('V') & 0x8000) {
                std::this_thread::sleep_for(std::chrono::milliseconds(15));
            }
            InputControl::clearBuffer();
            Appearance::clearScreen();
            PerspectiveManager::getInstance().toggleView();
            CombatScreen::updateScreenStatic("Turno " + std::to_string(currentTurn), enemies, currentPlayer, allies, false);
            return CombatScreen::getPlayerAction(currentTurn, currentPlayer, enemies, currentPlayer, allies);
        } else if (key == '\r' || key == '\n') {
            return actionList[selected].code;
        }
    }
}

int IDECombatScreen::getTargetAttack(
    const std::string& combatTitle,
    const std::vector<Character*>& enemies,
    Character* currentPlayer,
    const std::vector<Character*>& /*aliados*/
) {
    if (enemies.empty()) return 0;
    if (enemies.size() == 1) return 0;

    int selected = 0;
    int total = static_cast<int>(enemies.size());
    InputControl::clearBuffer();

    while (true) {
        std::vector<std::string> lines;
        lines.push_back(IDETheme::comment("// Resolução do pointer da entity alvo:"));
        lines.push_back(IDETheme::type("Character*") + " " + IDETheme::variable("target") + IDETheme::punctuation(" = ") + IDETheme::keyword("nullptr") + IDETheme::punctuation(";"));
        lines.push_back("");

        for (int i = 0; i < total; ++i) {
            std::string name = enemies[i]->getName();
            std::string line = "[" + std::to_string(i + 1) + "] target = &" + "enemy_" + std::to_string(i)
                             + "; // " + name + " (HP: " + std::to_string(enemies[i]->getHealth()) + ")";
            if (i == selected) {
                lines.push_back(std::string(IDETheme::COLOR_ACTIVE_TAB) + "> " + line + " <" + std::string(IDETheme::COLOR_RESET));
            } else {
                lines.push_back("  " + line);
            }
        }
        lines.push_back("");
        lines.push_back("  [0] target = nullptr; // Cancelar atribuicao de pointer");

        renderCombatFrame(combatTitle, enemies, currentPlayer, lines);

        char key = InputControl::readKey();
        if (key == '0' || key == 27) return -1;
        if (key >= '1' && key <= '9') {
            int idx = key - '1';
            if (idx >= 0 && idx < total) return idx;
        }
        if (key == 'w' || key == 'W' || key == 72) {
            selected = (selected - 1 + total) % total;
        } else if (key == 's' || key == 'S' || key == 80) {
            selected = (selected + 1) % total;
        } else if (key == '\r' || key == '\n') {
            return selected;
        }
    }
}

int IDECombatScreen::getTargetItem(
    const std::string& /*tituloCombate*/,
    const std::vector<Character*>& /*inimigos*/,
    Character* /*jogadorAtual*/,
    const std::vector<Character*>& /*aliados*/
) {
    return 0;
}

int IDECombatScreen::chooseShield(const std::string& /*nomePersonagem*/, const std::vector<Item*>& shields) {
    if (shields.empty()) return -1;

    int selected = 0;
    int total = static_cast<int>(shields.size());
    InputControl::clearBuffer();

    while (true) {
        std::vector<std::string> lines;
        lines.push_back(IDETheme::comment("// Vinculação do pointer de shield:"));
        lines.push_back(IDETheme::type("Shield*") + " " + IDETheme::variable("selectedShield") + IDETheme::punctuation(" = ") + IDETheme::keyword("nullptr") + IDETheme::punctuation(";"));
        lines.push_back("");

        for (size_t i = 0; i < shields.size(); ++i) {
            std::string line = "[" + std::to_string(i + 1) + "] selectedShield = &inventory[" + IDETheme::stringLiteral(shields[i]->getItemName()) + "];";
            if (static_cast<int>(i) == selected) {
                lines.push_back(std::string(IDETheme::COLOR_ACTIVE_TAB) + "> " + line + " <" + std::string(IDETheme::COLOR_RESET));
            } else {
                lines.push_back("  " + line);
            }
        }
        lines.push_back("");
        lines.push_back("  [0] selectedShield = nullptr; // Cancelar defensive stance");

        renderCombatFrame("Defesa", {}, nullptr, lines);

        char key = InputControl::readKey();
        if (key == '0' || key == 27) return 0;
        if (key >= '1' && key <= '9') {
            int idx = key - '0';
            if (idx >= 1 && idx <= total) return idx;
        }
        if (key == 'w' || key == 'W' || key == 72) {
            selected = (selected - 1 + total) % total;
        } else if (key == 's' || key == 'S' || key == 80) {
            selected = (selected + 1) % total;
        } else if (key == '\r' || key == '\n') {
            return selected + 1;
        }
    }
}

void IDECombatScreen::notifyEnemiesMoreAct() {
    int width = Appearance::getTerminalWidth();
    std::cout << "\n" << IDETheme::centerLine(IDETheme::comment("// [SCHEDULER] Inimigos com maior dexterity escalados primeiro no queue..."), width) << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
}

void IDECombatScreen::notifyShiftExtra(int dexterityPlayer, int maxEnemyDexterity) {
    int width = Appearance::getTerminalWidth();
    std::cout << "\n" << IDETheme::centerLine(IDETheme::comment("// [DISPATCH] Turno extra concedido pelo dispatcher! Dex(" + std::to_string(dexterityPlayer) + ") > (" + std::to_string(maxEnemyDexterity) + ")"), width) << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void IDECombatScreen::notifyUnpreventionInventory() {
    int width = Appearance::getTerminalWidth();
    std::cout << "\n" << IDETheme::centerLine(IDETheme::comment("// [INVENTORY] Turno consumido pela alocação do item na heap."), width) << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
}

void IDECombatScreen::notifyWithoutShields(const std::string& characterName) {
    int width = Appearance::getTerminalWidth();
    std::cout << "\n" << IDETheme::centerLine(IDETheme::comment("// [ASSERT_WARN] " + characterName + ": nenhum shield instanciado no inventory."), width) << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void IDECombatScreen::notifyImbalanceDefense(const std::string& characterName) {
    int width = Appearance::getTerminalWidth();
    std::cout << "\n" << IDETheme::centerLine(IDETheme::comment("// [WARN] " + characterName + ": defensive stance em cooldown."), width) << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void IDECombatScreen::notifyPostureDefensive(const std::string& characterName, const std::string& nameShield) {
    int width = Appearance::getTerminalWidth();
    std::cout << "\n" << IDETheme::centerLine(IDETheme::comment("// [OK] " + characterName + " assumiu defensive stance com [" + nameShield + "]."), width) << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void IDECombatScreen::notifyActionInvalidates() {
    int width = Appearance::getTerminalWidth();
    std::cout << "\n" << IDETheme::centerLine(IDETheme::comment("// [EXCEPTION] Instrução inválida fornecida ao dispatcher."), width) << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
}

void IDECombatScreen::notifyCancellationItem() {
    int width = Appearance::getTerminalWidth();
    std::cout << "\n" << IDETheme::centerLine(IDETheme::comment("// [ABORT] Operação com item cancelada no inventory."), width) << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
}

void IDECombatScreen::notifyUnmetRequirement(const std::string& requirementMessage) {
    int width = Appearance::getTerminalWidth();
    std::cout << "\n" << IDETheme::centerLine(IDETheme::comment("// [STATIC_ASSERT_FAILED] Requisito não atendido: " + requirementMessage), width) << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(600));
}
