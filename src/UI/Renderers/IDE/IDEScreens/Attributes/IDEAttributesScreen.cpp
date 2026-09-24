#include "UI/Renderers/IDE/IDEScreens/Attributes/IDEAttributesScreen.h"
#include "UI/Renderers/IDE/IDETheme.h"
#include "UI/Screens/Attributes/AttributesScreen.h"
#include "Domain/Characters/Character.h"
#include "Domain/Characters/Races/BaseRace.h"
#include "Domain/Characters/Classes/BaseClass.h"
#include "Domain/Items/Item.h"
#include "Systems/Inventory/Inventory.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>

namespace {
    void printScreen(const std::vector<std::string>& tabs, int activeTab, const std::string& breadcrumb, const std::vector<std::string>& contentLines, const std::string& footer = "") {
        Appearance::clearScreen();
        int width = Appearance::getTerminalWidth();
        int height = Appearance::getTerminalHeight();

        auto editorView = IDETheme::renderEditorView(tabs, activeTab, breadcrumb, contentLines, width, height, footer);
        for (const auto& l : editorView) {
            std::cout << l << "\n";
        }
        std::cout << "\033[J" << std::flush;
    }
}

void IDEAttributesScreen::display(Character* player) {
    managePlayerCharacterSheet(player);
}

void IDEAttributesScreen::displayDetailsAttributes(Character* currentPlayer) {
    if (!currentPlayer) return;

    std::vector<std::string> tabs = {
        "StatFormulas.cpp",
        "BuildRecommender.hpp"
    };

    std::vector<std::string> lines;
    lines.push_back(IDETheme::preprocessor("#pragma once"));
    lines.push_back(IDETheme::keyword("namespace ") + IDETheme::type("Engine::Calculators") + IDETheme::punctuation(" {"));
    lines.push_back("");
    lines.push_back(IDETheme::comment("    // Fórmula de cálculo de dano físico (physical damage)"));
    lines.push_back("    " + IDETheme::keyword("inline int ") + IDETheme::function("computePhysicalDamage") + IDETheme::punctuation("(int str, int weaponAtk) noexcept {"));
    lines.push_back("        " + IDETheme::keyword("return ") + IDETheme::punctuation("(str * 2) + weaponAtk;"));
    lines.push_back("    " + IDETheme::punctuation("}"));
    lines.push_back("");
    lines.push_back(IDETheme::comment("    // Fórmula de cálculo de dano mágico (magical damage)"));
    lines.push_back("    " + IDETheme::keyword("inline int ") + IDETheme::function("computeMagicalDamage") + IDETheme::punctuation("(int intel, int staffAtk) noexcept {"));
    lines.push_back("        " + IDETheme::keyword("return ") + IDETheme::punctuation("static_cast<int>(intel * 1.5f) + staffAtk;"));
    lines.push_back("    " + IDETheme::punctuation("}"));
    lines.push_back("");
    lines.push_back(IDETheme::comment("    // Curva percentual de mitigação de armor"));
    lines.push_back("    " + IDETheme::keyword("inline float ") + IDETheme::function("computeMitigationPct") + IDETheme::punctuation("(int resistance) noexcept {"));
    lines.push_back("        " + IDETheme::keyword("return ") + IDETheme::punctuation("1.0f - (100.0f / (100.0f + static_cast<float>(resistance)));"));
    lines.push_back("    " + IDETheme::punctuation("}"));
    lines.push_back("");
    lines.push_back(IDETheme::punctuation("} // namespace Engine::Calculators"));

    printScreen(tabs, 0, "// src/Engine/Calculators/StatFormulas.cpp", lines, "[ENTER] Retornar");
    InputControl::waitForEnter();
}

void IDEAttributesScreen::managePlayerCharacterSheet(Character* currentPlayer) {
    if (!currentPlayer) return;

    int currentTab = 0; // 0 = CharacterSheet.hpp, 1 = SkillsAndGear.hpp, 2 = StatFormulas.cpp, 3 = MemoryLayout.map

    while (true) {
        std::vector<std::string> tabs = {
            "CharacterSheet.hpp",
            "SkillsAndGear.hpp",
            "StatFormulas.cpp",
            "MemoryLayout.map"
        };

        if (currentTab == 0) {
            // --- FICHA PRINCIPAL (CharacterSheet.hpp) ---
            std::vector<std::string> lines;
            std::string raceName = currentPlayer->getRace() ? currentPlayer->getRace()->getRaceName() : "Humano";
            std::string className = currentPlayer->getClassName();
            int pGold = currentPlayer->getInventory() ? currentPlayer->getInventory()->getGold() : 0;

            lines.push_back(IDETheme::preprocessor("#pragma once"));
            lines.push_back(IDETheme::preprocessor("#include \"Core/Entities/Character.hpp\""));
            lines.push_back("");
            lines.push_back(IDETheme::keyword("namespace ") + IDETheme::type("Domain::Characters") + IDETheme::punctuation(" {"));
            lines.push_back("");
            lines.push_back(IDETheme::keyword("class ") + IDETheme::type("Hero") + IDETheme::punctuation(" final : public ") + IDETheme::type("Character") + IDETheme::punctuation(" {"));
            lines.push_back(IDETheme::keyword("private:"));
            lines.push_back("    " + IDETheme::type("const char*") + " " + IDETheme::variable("m_name") + "       = " + IDETheme::stringLiteral(currentPlayer->getName()) + ";");
            lines.push_back("    " + IDETheme::type("Race") + "        " + IDETheme::variable("m_race") + "       = " + IDETheme::type("Race::") + raceName + ";");
            lines.push_back("    " + IDETheme::type("Class") + "       " + IDETheme::variable("m_class") + "      = " + IDETheme::type("Class::") + className + ";");
            lines.push_back("    " + IDETheme::type("uint32_t") + "    " + IDETheme::variable("m_level") + "      = " + IDETheme::number(currentPlayer->getLevel()) + ";");
            lines.push_back("    " + IDETheme::type("uint32_t") + "    " + IDETheme::variable("m_gold") + "       = " + IDETheme::number(pGold) + ";");
            
            std::string xpBar = IDETheme::renderCodeHealthBar(currentPlayer->getCurrentXp(), currentPlayer->getXpForRise(), 10);
            lines.push_back("    " + IDETheme::type("uint32_t") + "    " + IDETheme::variable("m_currentXp") + "  = " + IDETheme::number(currentPlayer->getCurrentXp()) + "; // " + xpBar);
            lines.push_back("");

            lines.push_back(IDETheme::keyword("public:"));
            lines.push_back("    " + IDETheme::keyword("struct ") + IDETheme::type("HealthPool") + IDETheme::punctuation(" {"));
            std::string hpBar = IDETheme::renderCodeHealthBar(currentPlayer->getHealth(), currentPlayer->getMaxHealth(), 10);
            lines.push_back("        " + IDETheme::type("int") + " " + IDETheme::variable("currentHp") + " = " + IDETheme::number(currentPlayer->getHealth()) + "; // " + hpBar);
            lines.push_back("        " + IDETheme::type("int") + " " + IDETheme::variable("maxHp") + "     = " + IDETheme::number(currentPlayer->getMaxHealth()) + ";");
            lines.push_back("    " + IDETheme::punctuation("} hp;"));
            lines.push_back("");

            lines.push_back("    " + IDETheme::keyword("struct ") + IDETheme::type("Attributes") + IDETheme::punctuation(" {"));
            lines.push_back("        " + IDETheme::type("int") + " " + IDETheme::variable("strength") + "     = " + IDETheme::number(currentPlayer->getStrength()) + "; " + IDETheme::comment(IDETheme::renderCodeHealthBar(currentPlayer->getStrength(), 50, 6)));
            lines.push_back("        " + IDETheme::type("int") + " " + IDETheme::variable("dexterity") + "    = " + IDETheme::number(currentPlayer->getDexterity()) + "; " + IDETheme::comment(IDETheme::renderCodeHealthBar(currentPlayer->getDexterity(), 50, 6)));
            lines.push_back("        " + IDETheme::type("int") + " " + IDETheme::variable("resistance") + "   = " + IDETheme::number(currentPlayer->getResistance()) + "; " + IDETheme::comment(IDETheme::renderCodeHealthBar(currentPlayer->getResistance(), 50, 6)));
            lines.push_back("        " + IDETheme::type("int") + " " + IDETheme::variable("constitution") + " = " + IDETheme::number(currentPlayer->getConstitution()) + "; " + IDETheme::comment(IDETheme::renderCodeHealthBar(currentPlayer->getConstitution(), 50, 6)));
            lines.push_back("        " + IDETheme::type("int") + " " + IDETheme::variable("intelligence") + " = " + IDETheme::number(currentPlayer->getIntelligence()) + "; " + IDETheme::comment(IDETheme::renderCodeHealthBar(currentPlayer->getIntelligence(), 50, 6)));
            lines.push_back("        " + IDETheme::type("int") + " " + IDETheme::variable("wisdom") + "       = " + IDETheme::number(currentPlayer->getWisdom()) + "; " + IDETheme::comment(IDETheme::renderCodeHealthBar(currentPlayer->getWisdom(), 50, 6)));
            lines.push_back("    " + IDETheme::punctuation("} stats;"));
            lines.push_back("");

            PowerCombat power = AttributesScreen::calculatePowerCombat(currentPlayer, 1.0);
            lines.push_back("    " + IDETheme::keyword("struct ") + IDETheme::type("CombatTelemetry") + IDETheme::punctuation(" {"));
            lines.push_back("        " + IDETheme::type("int") + "   " + IDETheme::variable("physicalDmg") + "   = " + IDETheme::number(power.damagePhysIs) + ";");
            lines.push_back("        " + IDETheme::type("int") + "   " + IDETheme::variable("magicalDmg") + "    = " + IDETheme::number(power.damageMagicIs) + ";");
            lines.push_back("        " + IDETheme::type("int") + "   " + IDETheme::variable("flatDefense") + "   = " + IDETheme::number(power.defFixed) + ";");
            std::ostringstream ssPct;
            ssPct << std::fixed << std::setprecision(1) << power.mitigation << "f";
            lines.push_back("        " + IDETheme::type("float") + " " + IDETheme::variable("mitigationPct") + " = " + ssPct.str() + ";");
            lines.push_back("    " + IDETheme::punctuation("} telemetry;"));
            lines.push_back("");

            lines.push_back(IDETheme::comment("    // Despacho de methods / Call stack interativa:"));
            if (currentPlayer->canLevelUp()) {
                lines.push_back("    " + IDETheme::keyword("[1] void ") + IDETheme::function("levelUp") + "(); " + std::string(IDETheme::COLOR_FLASH_CURE) + " // XP DISPONIVEL! Execute rotina de level" + std::string(IDETheme::COLOR_RESET));
            } else {
                lines.push_back("    " + IDETheme::comment("[1] void levelUp(); // XP insuficiente (" + std::to_string(currentPlayer->getCurrentXp()) + "/" + std::to_string(currentPlayer->getXpForRise()) + ")"));
            }
            lines.push_back("    " + IDETheme::keyword("[2] void ") + IDETheme::function("openSkillsAndGear") + "();");
            lines.push_back("    " + IDETheme::keyword("[3] void ") + IDETheme::function("openStatFormulas") + "();");
            lines.push_back("    " + IDETheme::keyword("[4] void ") + IDETheme::function("openMemoryLayout") + "();");
            lines.push_back("    " + IDETheme::keyword("[0] void ") + IDETheme::function("returnToLoop") + "();");
            lines.push_back(IDETheme::punctuation("};"));
            lines.push_back("");
            lines.push_back(IDETheme::punctuation("} // namespace Domain::Characters"));

            printScreen(tabs, 0, "// src/Domain/Characters/Hero.hpp", lines, "[1-4] Disparar Metodo | [0 ou ESC] Retornar");

            char key = InputControl::readKey();
            if (key == '0' || key == 27) break;
            if (key == '1' && currentPlayer->canLevelUp()) {
                // [PT-BR] Rotina de level up com visualizacao de template C++
                // [EN-US] Level-up routine with live C++ template visualization
                std::vector<std::string> lvlLines;
                lvlLines.push_back(IDETheme::preprocessor("#pragma once"));
                lvlLines.push_back(IDETheme::comment("// Instanciação de template para alocação de attributes de level:"));
                lvlLines.push_back(IDETheme::keyword("template") + IDETheme::punctuation(" <") + IDETheme::type("AttributeType") + " " + IDETheme::variable("Stat") + IDETheme::punctuation(">"));
                lvlLines.push_back(IDETheme::keyword("void ") + IDETheme::type("Hero::") + IDETheme::function("allocateLevelPoint") + IDETheme::punctuation("() {"));
                lvlLines.push_back("    " + IDETheme::keyword("switch ") + IDETheme::punctuation("(") + IDETheme::variable("Stat") + IDETheme::punctuation(") {"));

                std::vector<std::string> enumLabels = {
                    "AttributeType::MAX_HEALTH",
                    "AttributeType::STRENGTH",
                    "AttributeType::DEXTERITY",
                    "AttributeType::RESISTANCE",
                    "AttributeType::CONSTITUTION",
                    "AttributeType::INTELLIGENCE",
                    "AttributeType::WISDOM"
                };

                for (int i = 1; i <= 7; ++i) {
                    auto clone = currentPlayer->clone();
                    clone->levelUp(static_cast<AttributeType>(i));

                    int valOld = 0, valNew = 0;
                    switch (i) {
                        case 1: valOld = currentPlayer->getMaxHealth(); valNew = clone->getMaxHealth(); break;
                        case 2: valOld = currentPlayer->getStrength(); valNew = clone->getStrength(); break;
                        case 3: valOld = currentPlayer->getDexterity(); valNew = clone->getDexterity(); break;
                        case 4: valOld = currentPlayer->getResistance(); valNew = clone->getResistance(); break;
                        case 5: valOld = currentPlayer->getConstitution(); valNew = clone->getConstitution(); break;
                        case 6: valOld = currentPlayer->getIntelligence(); valNew = clone->getIntelligence(); break;
                        case 7: valOld = currentPlayer->getWisdom(); valNew = clone->getWisdom(); break;
                    }
                    int gain = valNew - valOld;
                    lvlLines.push_back("        " + IDETheme::keyword("case ") + IDETheme::type(enumLabels[i - 1]) + IDETheme::punctuation(":"));
                    lvlLines.push_back("            " + IDETheme::variable("delta") + " = [" + std::to_string(i) + "] " + std::to_string(valOld) + " -> " + std::to_string(valNew) + " " + std::string(IDETheme::COLOR_FLASH_CURE) + "(+" + std::to_string(gain) + ")" + std::string(IDETheme::COLOR_RESET) + ";");
                    lvlLines.push_back("            " + IDETheme::keyword("break;"));
                }
                lvlLines.push_back("        " + IDETheme::keyword("default:"));
                lvlLines.push_back("            " + IDETheme::keyword("return;") + " // [0] Cancelar alocacao");
                lvlLines.push_back("    " + IDETheme::punctuation("}"));
                lvlLines.push_back("    " + IDETheme::variable("this->m_level") + IDETheme::punctuation("++;"));
                lvlLines.push_back(IDETheme::punctuation("}"));

                std::vector<std::string> lvlTabs = { "LevelUpRoutine.hpp" };
                printScreen(lvlTabs, 0, "// src/Domain/Characters/LevelUpRoutine.hpp", lvlLines, "[1-7] Alocar Atributo | [0] Abortar Operação");

                char cLvl = InputControl::readKey();
                if (cLvl >= '1' && cLvl <= '7') {
                    int chosenStat = cLvl - '0';
                    currentPlayer->levelUp(static_cast<AttributeType>(chosenStat));
                }
            } else if (key == '2') {
                currentTab = 1;
            } else if (key == '3') {
                currentTab = 2;
            } else if (key == '4') {
                currentTab = 3;
            }
        } else if (currentTab == 1) {
            // --- HABILIDADES E EQUIPAMENTOS (SkillsAndGear.hpp) ---
            std::vector<std::string> lines;
            lines.push_back(IDETheme::preprocessor("#pragma once"));
            lines.push_back(IDETheme::preprocessor("#include \"Core/Items/Equipment.hpp\""));
            lines.push_back("");
            lines.push_back(IDETheme::keyword("namespace ") + IDETheme::type("Domain::Skills") + IDETheme::punctuation(" {"));
            lines.push_back("");
            lines.push_back(IDETheme::comment("    // Passives despachadas"));
            lines.push_back("    " + IDETheme::keyword("void ") + IDETheme::function("onPassiveRaceTrigger") + IDETheme::punctuation("() {"));
            std::string racePassive = currentPlayer->getRace() ? currentPlayer->getRace()->getDescriptionSkillRace() : "Nenhuma";
            lines.push_back("        " + IDETheme::comment("// " + racePassive));
            lines.push_back("    " + IDETheme::punctuation("}"));
            lines.push_back("");

            lines.push_back(IDETheme::comment("    // Capability ativa da class"));
            std::string skillName = currentPlayer->getClass() ? currentPlayer->getClass()->getNameSkillClass() : "Nenhuma";
            std::string skillDesc = currentPlayer->getClass() ? currentPlayer->getClass()->getDescriptionSkillClass() : "";
            lines.push_back("    " + IDETheme::keyword("void ") + IDETheme::function("executeClassSkill_" + skillName) + IDETheme::punctuation("() {"));
            lines.push_back("        " + IDETheme::comment("// " + skillDesc));
            lines.push_back("    " + IDETheme::punctuation("}"));
            lines.push_back("");

            lines.push_back(IDETheme::comment("    // Pointers de hardware do equipment"));
            Item* w = currentPlayer->getWeapons();
            Item* s = currentPlayer->getShield();
            Item* a = currentPlayer->getArmor();
            lines.push_back("    " + IDETheme::type("Weapon*") + " " + IDETheme::variable("eqWeapon") + " = " + (w ? ("&heap[" + IDETheme::stringLiteral(w->getItemName()) + "]") : "nullptr") + ";");
            lines.push_back("    " + IDETheme::type("Shield*") + " " + IDETheme::variable("eqShield") + " = " + (s ? ("&heap[" + IDETheme::stringLiteral(s->getItemName()) + "]") : "nullptr") + ";");
            lines.push_back("    " + IDETheme::type("Armor*") + "  " + IDETheme::variable("eqArmor")  + " = " + (a ? ("&heap[" + IDETheme::stringLiteral(a->getItemName()) + "]") : "nullptr") + ";");
            lines.push_back("");
            lines.push_back(IDETheme::punctuation("} // namespace Domain::Skills"));

            printScreen(tabs, 1, "// src/Domain/Skills/SkillsAndGear.hpp", lines, "[1] Hero.hpp | [0/ESC] Retornar");
            char k = InputControl::readKey();
            if (k == '1' || k == '0' || k == 27) currentTab = 0;
        } else if (currentTab == 2) {
            // --- FORMULAS (StatFormulas.cpp) ---
            std::vector<std::string> lines;
            lines.push_back(IDETheme::preprocessor("#pragma once"));
            lines.push_back(IDETheme::keyword("namespace ") + IDETheme::type("Engine::Formulas") + IDETheme::punctuation(" {"));
            lines.push_back("");
            lines.push_back("    " + IDETheme::keyword("inline int ") + IDETheme::function("computeRawPhysicalDamage") + IDETheme::punctuation("(int str, int wAtk) {"));
            lines.push_back("        " + IDETheme::keyword("return ") + IDETheme::punctuation("(str * 2) + wAtk;"));
            lines.push_back("    " + IDETheme::punctuation("}"));
            lines.push_back("");
            lines.push_back("    " + IDETheme::keyword("inline int ") + IDETheme::function("computeMagicalDamage") + IDETheme::punctuation("(int intel, int staffAtk) {"));
            lines.push_back("        " + IDETheme::keyword("return ") + IDETheme::punctuation("static_cast<int>(intel * 1.5f) + staffAtk;"));
            lines.push_back("    " + IDETheme::punctuation("}"));
            lines.push_back("");
            lines.push_back("    " + IDETheme::keyword("inline float ") + IDETheme::function("computeMitigationPct") + IDETheme::punctuation("(int res) {"));
            lines.push_back("        " + IDETheme::keyword("return ") + IDETheme::punctuation("1.0f - (100.0f / (100.0f + static_cast<float>(res)));"));
            lines.push_back("    " + IDETheme::punctuation("}"));
            lines.push_back("");
            lines.push_back(IDETheme::punctuation("} // namespace Engine::Formulas"));

            printScreen(tabs, 2, "// src/Engine/Formulas/StatFormulas.cpp", lines, "[1] Hero.hpp | [0/ESC] Retornar");
            char k = InputControl::readKey();
            if (k == '1' || k == '0' || k == 27) currentTab = 0;
        } else if (currentTab == 3) {
            // --- MEMORY LAYOUT (MemoryLayout.map) ---
            std::vector<std::string> lines;
            lines.push_back(IDETheme::comment("// Memory layout da class Hero (sizeof = 0x38 bytes, align = 8):"));
            lines.push_back(IDETheme::comment("// +0x00: vptr -> &vtable da class Hero"));
            lines.push_back(IDETheme::punctuation("+0x08: ") + IDETheme::type("uint32_t") + " " + IDETheme::variable("m_level") + "            [offset: 8,  size: 4] = " + IDETheme::number(currentPlayer->getLevel()));
            lines.push_back(IDETheme::punctuation("+0x0C: ") + IDETheme::type("uint32_t") + " " + IDETheme::variable("m_currentXp") + "        [offset: 12, size: 4] = " + IDETheme::number(currentPlayer->getCurrentXp()));
            lines.push_back(IDETheme::punctuation("+0x10: ") + IDETheme::type("uint32_t") + " " + IDETheme::variable("m_xpForRise") + "        [offset: 16, size: 4] = " + IDETheme::number(currentPlayer->getXpForRise()));
            lines.push_back(IDETheme::punctuation("+0x14: ") + IDETheme::type("int32_t") + "  " + IDETheme::variable("hp.currentHp") + "       [offset: 20, size: 4] = " + IDETheme::number(currentPlayer->getHealth()));
            lines.push_back(IDETheme::punctuation("+0x18: ") + IDETheme::type("int32_t") + "  " + IDETheme::variable("hp.maxHp") + "           [offset: 24, size: 4] = " + IDETheme::number(currentPlayer->getMaxHealth()));
            lines.push_back(IDETheme::punctuation("+0x1C: ") + IDETheme::type("int32_t") + "  " + IDETheme::variable("stats.strength") + "     [offset: 28, size: 4] = " + IDETheme::number(currentPlayer->getStrength()));
            lines.push_back(IDETheme::punctuation("+0x20: ") + IDETheme::type("int32_t") + "  " + IDETheme::variable("stats.dexterity") + "    [offset: 32, size: 4] = " + IDETheme::number(currentPlayer->getDexterity()));
            lines.push_back(IDETheme::punctuation("+0x24: ") + IDETheme::type("int32_t") + "  " + IDETheme::variable("stats.resistance") + "   [offset: 36, size: 4] = " + IDETheme::number(currentPlayer->getResistance()));
            lines.push_back(IDETheme::punctuation("+0x28: ") + IDETheme::type("int32_t") + "  " + IDETheme::variable("stats.constitution") + " [offset: 40, size: 4] = " + IDETheme::number(currentPlayer->getConstitution()));
            lines.push_back(IDETheme::punctuation("+0x2C: ") + IDETheme::type("int32_t") + "  " + IDETheme::variable("stats.intelligence") + " [offset: 44, size: 4] = " + IDETheme::number(currentPlayer->getIntelligence()));
            lines.push_back(IDETheme::punctuation("+0x30: ") + IDETheme::type("Weapon*") + "  " + IDETheme::variable("eqWeapon") + "           [offset: 48, size: 8] = " + (currentPlayer->getWeapons() ? "0x7ffd01" : "nullptr"));
            lines.push_back(IDETheme::punctuation("+0x38: ") + IDETheme::type("Shield*") + "  " + IDETheme::variable("eqShield") + "           [offset: 56, size: 8] = " + (currentPlayer->getShield() ? "0x7ffd08" : "nullptr"));

            printScreen(tabs, 3, "// dump/memory/Hero_MemoryLayout.map", lines, "[1] Hero.hpp | [0/ESC] Retornar");
            char k = InputControl::readKey();
            if (k == '1' || k == '0' || k == 27) currentTab = 0;
        }
    }
}
